/*
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie firmware.
 *
 * Copyright 2018, Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lpsTdoa3Tag.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lpsTdoa3Tag.c. If not, see <http://www.gnu.org/licenses/>.
 */

/*

The tag is assumed to move around in a large system of anchors. Any anchor ids
can be used, and the same anchor id can even be used by multiple anchors as long
as they are not visible in the same area. It is assumed that the anchor density
is evenly distributed in the covered volume and that 5-20 anchors are visible
in every point. The tag is attached to a physical object and the expected
velocity is a few m/s, this means that anchors are within range for a time
period of seconds.

The implementation must handle
1. An infinite number of anchors, where around 20 are visible at one time
2. Any anchor ids
3. Dynamically changing visibility of anchors over time
4. Random TX times from anchors with possible packet collisions and packet loss

*/

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lpsTdoa3Tag.h"
#include "tdoaEngineInstance.h"
#include "tdoaStats.h"
#include "estimator.h"

#include "libdw1000.h"
#include "mac.h"

#include "param.h"

#define DEBUG_MODULE "TDOA3"
#include "debug.h"
#include "cfassert.h"
#include "eventtrigger.h"
#include "hmac_md5.h"

#include "queue.h"
#include "timers.h"

// Positions for sent LPP packets
#define LPS_TDOA3_TYPE 0
#define LPS_TDOA3_SEND_LPP_PAYLOAD 1

#define PACKET_TYPE_TDOA3 0x30

#define TDOA3_RECEIVE_TIMEOUT 10000

#define TIME_SCALEDOWN_FACTOR 1e3
#define SCALEDDOWN_UINT32_MAX (UINT32_MAX / TIME_SCALEDOWN_FACTOR)
#define GET_STD_TIME(time, wrapovers) ((time) / TIME_SCALEDOWN_FACTOR + (wrapovers) * SCALEDDOWN_UINT32_MAX)
#define SECS_PER_WRAP 17.2074010256
#define STD_TIME_TO_SEC(time) (((time) / SCALEDDOWN_UINT32_MAX) * SECS_PER_WRAP)

#define KEYCHAIN_SIZE 200
#define DATA_FREQ 10.0
#define DATA_PER_SEC (1.0 / DATA_FREQ)
#define LIFESPAN ((uint16_t)(KEYCHAIN_SIZE * DATA_PER_SEC))
// #define MAX_INTERVAL ((uint16_t)(KEYCHAIN_SIZE/TDOAS_PER_SEC))
#define LAST_KEY_INDEX (KEYCHAIN_SIZE - 1)
#define INTERVAL_LEN_IN_MS (DATA_PER_SEC * 1000)
#define DISCLOSURE_DELAY 1
#define DELAY_BEFORE_AUTH_AFTER_KEY_DISCLOSURE (INTERVAL_LEN_IN_MS/2)
#define AUTH_DELAY ((INTERVAL_LEN_IN_MS * DISCLOSURE_DELAY) + DELAY_BEFORE_AUTH_AFTER_KEY_DISCLOSURE)
#define MISSING_KEYS_TO_RECONSTRUCT 20
#define GOOD_KEY_LOOP_COUNT (KEYCHAIN_SIZE/20)

EVENTTRIGGER(intentionChanged, uint8, intent)
EVENTTRIGGER(syncStartTimeSet, uint32, syncStartTime)

typedef struct
{
  uint8_t type;
  uint8_t seq;
  uint32_t txTimeStamp;
  uint8_t remoteCount;
} __attribute__((packed)) rangePacketHeader3_t;

typedef struct
{
  uint8_t id;
  uint8_t seq;
  uint32_t rxTimeStamp;
  uint16_t distance;
} __attribute__((packed)) remoteAnchorDataFull_t;

typedef struct
{
  uint8_t id;
  uint8_t seq;
  uint32_t rxTimeStamp;
} __attribute__((packed)) remoteAnchorDataShort_t;

typedef struct
{
  rangePacketHeader3_t header;
  uint8_t remoteAnchorData;
} __attribute__((packed)) rangePacket3_t;

typedef struct
{
  tdoaMeasurement_t tdoaMeasurement;
  struct lppShortAnchorPos_s lastLPP[16];
} MLPP_t;

// Outgoing LPP packet
static lpsLppShortPacket_t lppPacket;

static bool rangingOk;

static uint16_t activeAnchors = 0;
static uint16_t anchorsCount = 0;
static uint16_t activeAnchorsCount = 0;
static float syncTime = 0;
static uint32_t syncMsgCounter = 0;
static uint32_t syncStartTime = 0;
static float syncResidual = 0;

static float stdDev = TDOA_ENGINE_MEASUREMENT_NOISE_STD;
static uint8_t intention = 0;
static uint16_t queueErrorCount = 0;

struct lppShortAnchorPos_s lastLPP[16] = {0};

// tdoaMeasurement_t lastMeasurement = {0};

#define KEY201                                                                                     \
  {                                                                                                \
    0x0e, 0x0a, 0x65, 0x85, 0x26, 0x0b, 0x06, 0x37, 0x31, 0x0f, 0x9b, 0x3a, 0x52, 0x4b, 0x5d, 0x49 \
  }
#define KEY51                                                                                      \
  {                                                                                                \
    0x29, 0xcf, 0xc6, 0x31, 0xdc, 0xd7, 0x5c, 0x96, 0xc3, 0xc7, 0x25, 0x29, 0xed, 0x87, 0x61, 0xf2 \
  }

static const md5_byte_t k0[HASH_LEN] = KEY201;
static md5_byte_t commitment[HASH_LEN] = KEY201;

static md5_byte_t keychain[KEYCHAIN_SIZE][HASH_LEN] = {0};
static bool disclosedKeychain[KEYCHAIN_SIZE] = {0};

static uint32_t _lastRX = 0;
static uint8_t _rxWrapovers = 0;
static int32_t lastOffset = 0;
static float lastClockCorrection;
static float syncError;

static uint32_t authCount = 0;
static uint32_t unauthCount = 0;
static uint32_t measCount = 0;
static uint32_t lppTypeCount = 0;
static uint32_t lppHeaderCount = 0;
static uint32_t lppLengthyCount = 0;
static uint32_t goodLppCount = 0;
static uint32_t badLppCount = 0;

static uint32_t pCount = 0;

static QueueHandle_t teslaQueue;

static void genMD5(md5_byte_t *input, uint8_t len, md5_byte_t *output)
{
  md5_state_t hash_state;
  md5_init(&hash_state);
  md5_append(&hash_state, input, len);
  md5_finish(&hash_state, output);
}

static uint8_t getCurrentMockIntervalBasedOnLastInfo()
{
  double time = syncTime;
  uint16_t whole = (uint16_t)time;
  // return whole % LIFESPAN;
  uint16_t centi = (time - whole) * 100;
  uint16_t cyclic = whole % LIFESPAN;
  uint16_t scaled = centi + cyclic * 100;
  uint8_t interval = scaled * DATA_PER_SEC;
  return interval;
}

static uint8_t getKeyIndexFor(uint8_t interval)
{
  return (LAST_KEY_INDEX - interval) % KEYCHAIN_SIZE;
}

static uint8_t getPreviousKeyIndexFor(uint8_t interval)
{
  uint8_t keyIndex;
  int prevI = interval - DISCLOSURE_DELAY;
  if (prevI < 0)
  {
    keyIndex = getKeyIndexFor(KEYCHAIN_SIZE + prevI);
  }
  else
  {
    keyIndex = getKeyIndexFor(prevI);
  }
  return keyIndex;
}
bool memeqzero(const void *data, size_t length)
{
  const unsigned char *p = data;
  size_t len;

  /* Check first 16 bytes manually */
  for (len = 0; len < 16; len++)
  {
    if (!length)
      return true;
    if (*p)
      return false;
    p++;
    length--;
  }
  /* Now we know that's zero, memcmp with self. */
  return memcmp(data, p, length) == 0;
}

static uint8_t currentInterval = 0;

static int keyCount;

// this is a heavy procedure impacting tdaos per sec
static bool isGoodKey(md5_byte_t *key)
{
  keyCount = 0;
  md5_byte_t output[HASH_LEN];
  genMD5(key, HASH_LEN, output);
  for (uint8_t i = 0; i <= GOOD_KEY_LOOP_COUNT; i++)
  {
    keyCount++;
    if (memcmp(output, commitment, HASH_LEN) == 0 || memcmp(output, k0, HASH_LEN) == 0)
    { // match
      memcpy(commitment, key, HASH_LEN);
      return true;
    }
    genMD5(output, HASH_LEN, output);
    // genMD5(&output, HASH_LEN, output);
  }
  return false;
}

static bool isValidTimeStamp(const int64_t anchorRxTime)
{
  return anchorRxTime != 0;
}

static int updateRemoteData(tdoaAnchorContext_t *anchorCtx, const void *payload)
{
  const rangePacket3_t *packet = (rangePacket3_t *)payload;
  const void *anchorDataPtr = &packet->remoteAnchorData;
  for (uint8_t i = 0; i < packet->header.remoteCount; i++)
  {
    remoteAnchorDataFull_t *anchorData = (remoteAnchorDataFull_t *)anchorDataPtr;

    uint8_t remoteId = anchorData->id;
    int64_t remoteRxTime = anchorData->rxTimeStamp;
    uint8_t remoteSeqNr = anchorData->seq & 0x7f;

    if (isValidTimeStamp(remoteRxTime))
    {
      tdoaStorageSetRemoteRxTime(anchorCtx, remoteId, remoteRxTime, remoteSeqNr);
    }

    bool hasDistance = ((anchorData->seq & 0x80) != 0);
    if (hasDistance)
    {
      int64_t tof = anchorData->distance;
      if (isValidTimeStamp(tof))
      {
        tdoaStorageSetTimeOfFlight(anchorCtx, remoteId, tof);

        uint8_t anchorId = tdoaStorageGetId(anchorCtx);
        tdoaStats_t *stats = &tdoaEngineState.stats;
        if (anchorId == stats->anchorId && remoteId == stats->remoteAnchorId)
        {
          stats->tof = (uint16_t)tof;
        }
      }

      anchorDataPtr += sizeof(remoteAnchorDataFull_t);
    }
    else
    {
      anchorDataPtr += sizeof(remoteAnchorDataShort_t);
    }
  }

  return (uint8_t *)anchorDataPtr - (uint8_t *)packet;
}
static uint8_t disclosureCount = 0;
static uint8_t currentKeyIndex = 0;

static uint8_t bufferCount = 0;

/*
typedef struct nav_s {
  float position[3];
  uint8_t I;
} nav_t;

static nav_t getPosI(point_t *p, uint8_t I) {
  nav_t nav = {p->x, p->y, p->z , I};
  return nav;
}
*/

static bool authenticate(uint8_t anchorId) {

  struct lppShortAnchorPos_s *lpp = &lastLPP[anchorId];
  uint8_t interval = lpp->interval;

  md5_byte_t mac[HASH_LEN];
  uint8_t newKeyIndex = getKeyIndexFor(interval);
  md5_byte_t *key = keychain[newKeyIndex];
  hmac_md5((md5_byte_t *)lpp, 12, key, HASH_LEN, mac);

  bool match = memcmp(lpp->mac, mac, HASH_LEN) == 0;

  // we're done with it, clean buffer for new data
  memset(lpp, 0, sizeof(struct lppShortAnchorPos_s));

  return match;
  
}

static void authCallback(TimerHandle_t xTimer)
{
  
  tdoaMeasurement_t measurement;
  xQueueReceive(teslaQueue, &measurement, 0);
  
  bool isAuthentic = authenticate(measurement.anchorIdA) && authenticate(measurement.anchorIdB);
  
  if (isAuthentic) {
    authCount++;
    estimatorEnqueueTDOA(&measurement);
  } else {
    unauthCount++;
  }

  bufferCount--;

  xTimerDelete(xTimer, 0);

}

/* 2000 tdoas per sec
static void handleLppShortPacket(tdoaAnchorContext_t* anchorCtx, const uint8_t *data, const int length) {
  uint8_t type = data[0];

  if (type == LPP_SHORT_ANCHORPOS) {
    struct lppShortAnchorPos_s *newpos = (struct lppShortAnchorPos_s*)&data[1];
    tdoaStorageSetAnchorPosition(anchorCtx, newpos->x, newpos->y, newpos->z);
  }
}
*/

static void handleLppShortPacket(const uint32_t localTime, const bool timeIsGood, tdoaAnchorContext_t *anchorCtx, const uint8_t *data, const int length) {

  uint8_t type = data[0];

  if (type == LPP_SHORT_ANCHORPOS) {
    lppTypeCount++;
    struct lppShortAnchorPos_s *newpos = (struct lppShortAnchorPos_s *)&data[1];
    tdoaStorageSetAnchorPosition(anchorCtx, newpos->x, newpos->y, newpos->z);
    
    uint8_t anchorId = tdoaStorageGetId(anchorCtx);
    uint32_t incomingGlobalTime = newpos->globalTime;
    double clockCorrection = tdoaStorageGetClockCorrection(anchorCtx);
    lastClockCorrection = clockCorrection;

    // && anchorId >= 8
    if (timeIsGood && incomingGlobalTime != 0)  { 
      if (lastOffset) {
        static double residualSum = 0;
        double cTimeOld = STD_TIME_TO_SEC(localTime * lastClockCorrection + lastOffset);
        int32_t offset = incomingGlobalTime - localTime;
        double cTimeNew = STD_TIME_TO_SEC(localTime * clockCorrection + offset);
        syncResidual = (float)(cTimeNew - cTimeOld);
        double residual = pow(cTimeNew - cTimeOld, 2.0);
        residualSum += residual;
        if (STD_TIME_TO_SEC(localTime - syncStartTime) < 3600) { // 10 min
          syncError = sqrtf(residualSum / ++syncMsgCounter);
        }
      } else {
        syncStartTime = localTime;
      }
      lastOffset = incomingGlobalTime - localTime;
      syncTime = STD_TIME_TO_SEC(localTime * clockCorrection + lastOffset);
    }

    // no impact til here

    currentInterval = getCurrentMockIntervalBasedOnLastInfo();

    //tdoaStorageSetAnchorPosition(anchorCtx, newpos->x, newpos->y, newpos->z);

    if (isGoodKey(newpos->disclosedKey))
    {
      goodLppCount++;
      // this packet is revealing the key used in previous interval, hence (I-d)
      uint8_t keyIndex = getPreviousKeyIndexFor(newpos->interval);
      currentKeyIndex = keyIndex;
      if (memcmp(keychain[keyIndex], newpos->disclosedKey, HASH_LEN) != 0)
      { // different than what we already know
        disclosureCount++;
      }
      disclosedKeychain[keyIndex] = true;
      memcpy(keychain[keyIndex], newpos->disclosedKey, HASH_LEN);
      

      // make sure 5 keys are there
      for (uint8_t i=0; i < min(MISSING_KEYS_TO_RECONSTRUCT, KEYCHAIN_SIZE-1-keyIndex); i++) {
        if (disclosedKeychain[keyIndex+i+1] == false) {
          genMD5(keychain[keyIndex+i], HASH_LEN, keychain[keyIndex+i+1]);
          disclosedKeychain[keyIndex+i+1] = true;
        }
      }

      size_t len = sizeof(struct lppShortAnchorPos_s);

      if (memeqzero(&lastLPP[anchorId], len))
      { // set only if zero
        memcpy(&lastLPP[anchorId], newpos, len);
        // pdMS_TO_TICKS(INTERVAL_LEN_IN_MS*2)
      }
    } else {
      badLppCount++;
    }
  }
}

static void handleLppPacket(const uint32_t localTime, const bool timeIsGood, const int dataLength, int rangePacketLength, const packet_t *rxPacket, tdoaAnchorContext_t *anchorCtx)
{
  const int32_t payloadLength = dataLength - MAC802154_HEADER_LENGTH;
  const int32_t startOfLppDataInPayload = rangePacketLength;
  const int32_t lppDataLength = payloadLength - startOfLppDataInPayload;
  const int32_t lppTypeInPayload = startOfLppDataInPayload + 1;

  if (lppDataLength > 0)
  {
    lppLengthyCount++;
    const uint8_t lppPacketHeader = rxPacket->payload[startOfLppDataInPayload];
    if (lppPacketHeader == LPP_HEADER_SHORT_PACKET)
    {
      lppHeaderCount++;
      const int32_t lppTypeAndPayloadLength = lppDataLength - 1;
      handleLppShortPacket(localTime, timeIsGood, anchorCtx, &rxPacket->payload[lppTypeInPayload], lppTypeAndPayloadLength);
    }
  }
}

struct BufferedPacket
{
  packet_t packet;
} __attribute__((packed));

static void rxcallback(dwDevice_t *dev)
{
  tdoaStats_t *stats = &tdoaEngineState.stats;
  STATS_CNT_RATE_EVENT(&stats->packetsReceived);

  int dataLength = dwGetDataLength(dev);
  packet_t rxPacket;

  dwGetData(dev, (uint8_t *)&rxPacket, dataLength);
  const uint8_t anchorId = rxPacket.sourceAddress & 0xff;

  // if (anchorId < 8)
  //   return;

  dwTime_t arrival = {.full = 0};
  dwGetReceiveTimestamp(dev, &arrival);
  const int64_t rxAn_by_T_in_cl_T = arrival.full;

  _rxWrapovers = _lastRX > arrival.high32 ? _rxWrapovers + 1 : _rxWrapovers;

  _lastRX = arrival.high32;

  const uint32_t localTime = GET_STD_TIME(_lastRX, _rxWrapovers);

  const rangePacket3_t *packet = (rangePacket3_t *)rxPacket.payload;

  if (packet->header.type == PACKET_TYPE_TDOA3)
  {
    const int64_t txAn_in_cl_An = packet->header.txTimeStamp;
    
    const uint8_t seqNr = packet->header.seq & 0x7f;
    
    //const uint32_t incomingGlobalTime = packet->header.globalTime;

    tdoaAnchorContext_t anchorCtx;
    uint32_t now_ms = T2M(xTaskGetTickCount());

    tdoaEngineGetAnchorCtxForPacketProcessing(&tdoaEngineState, anchorId, now_ms, &anchorCtx);

    int rangeDataLength = updateRemoteData(&anchorCtx, packet);

    bool timeIsGood = tdoaEngineProcessPacket(&tdoaEngineState, &anchorCtx, txAn_in_cl_An, rxAn_by_T_in_cl_T);

    tdoaStorageSetRxTxData(&anchorCtx, rxAn_by_T_in_cl_T, txAn_in_cl_An, seqNr);

    handleLppPacket(localTime ,timeIsGood, dataLength, rangeDataLength, &rxPacket, &anchorCtx);
    pCount++;
    //handleLppPacket(dataLength, rangeDataLength, &rxPacket, &anchorCtx);
    rangingOk = true;
  }
}

static void setRadioInReceiveMode(dwDevice_t *dev)
{
  dwNewReceive(dev);
  dwSetDefaults(dev);
  dwStartReceive(dev);
}

static void sendLppShort(dwDevice_t *dev, lpsLppShortPacket_t *packet)
{
  static packet_t txPacket;
  dwIdle(dev);

  MAC80215_PACKET_INIT(txPacket, MAC802154_TYPE_DATA);

  txPacket.payload[LPS_TDOA3_TYPE] = LPP_HEADER_SHORT_PACKET;
  memcpy(&txPacket.payload[LPS_TDOA3_SEND_LPP_PAYLOAD], packet->data, packet->length);

  txPacket.pan = 0xbccf;
  txPacket.sourceAddress = 0xbccf000000000000 | 0xff;
  txPacket.destAddress = 0xbccf000000000000 | packet->dest;

  dwNewTransmit(dev);
  dwSetDefaults(dev);
  dwSetData(dev, (uint8_t *)&txPacket, MAC802154_HEADER_LENGTH + 1 + packet->length);

  dwStartTransmit(dev);
}

static bool sendLpp(dwDevice_t *dev)
{
  bool lppPacketToSend = lpsGetLppShort(&lppPacket);
  if (lppPacketToSend)
  {
    sendLppShort(dev, &lppPacket);
    return true;
  }

  return false;
}

static uint32_t onEvent(dwDevice_t *dev, uwbEvent_t event)
{
  switch (event)
  {
  case eventPacketReceived:
    rxcallback(dev);
    break;
  case eventTimeout:
    break;
  case eventReceiveTimeout:
    break;
  case eventReceiveFailed:
    break;
  case eventPacketSent:
    // Service packet sent, the radio is back to receive automatically
    break;
  default:
    ASSERT_FAILED();
  }

  if (!sendLpp(dev))
  {
    setRadioInReceiveMode(dev);
  }

  uint32_t now_ms = T2M(xTaskGetTickCount());
  tdoaStatsUpdate(&tdoaEngineState.stats, now_ms);

  return MAX_TIMEOUT;
}

static void sendTdoaToEstimatorCallback(tdoaMeasurement_t *tdoaMeasurement)
{
  // Override the default standard deviation set by the TDoA engine.
  tdoaMeasurement->stdDev = stdDev;

  estimatorEnqueueTDOA(tdoaMeasurement);

  measCount++;

  xQueueSend(teslaQueue, tdoaMeasurement, 0);
  bufferCount++;
  TimerHandle_t authTimerHandle = xTimerCreate("authTimer", pdMS_TO_TICKS(AUTH_DELAY), pdFALSE, 0, authCallback);
  if (authTimerHandle != NULL) {
    xTimerStart(authTimerHandle, 0);
  }

#ifdef CONFIG_DECK_LOCO_2D_POSITION
  heightMeasurement_t heightData;
  heightData.timestamp = xTaskGetTickCount();
  heightData.height = DECK_LOCO_2D_POSITION_HEIGHT;
  heightData.stdDev = 0.0001;
  estimatorEnqueueAbsoluteHeight(&heightData);
#endif
}

static bool getAnchorPosition(const uint8_t anchorId, point_t *position)
{
  tdoaAnchorContext_t anchorCtx;
  uint32_t now_ms = T2M(xTaskGetTickCount());

  bool contextFound = tdoaStorageGetAnchorCtx(tdoaEngineState.anchorInfoArray, anchorId, now_ms, &anchorCtx);
  if (contextFound)
  {
    tdoaStorageGetAnchorPosition(&anchorCtx, position);
    return true;
  }

  return false;
}

static uint8_t getAnchorIdList(uint8_t unorderedAnchorList[], const int maxListSize)
{
  anchorsCount = tdoaStorageGetListOfAnchorIds(tdoaEngineState.anchorInfoArray, unorderedAnchorList, maxListSize);
  return anchorsCount;
}

static uint8_t getActiveAnchorIdList(uint8_t unorderedAnchorList[], const int maxListSize)
{
  uint32_t now_ms = T2M(xTaskGetTickCount());
  activeAnchorsCount = tdoaStorageGetListOfActiveAnchorIds(tdoaEngineState.anchorInfoArray, unorderedAnchorList, maxListSize, now_ms);
  uint16_t flags = 0;
  for (int i = 0; i < activeAnchorsCount; i++)
  {
    flags |= 1 << unorderedAnchorList[i];
  }
  activeAnchors = flags;
  return activeAnchorsCount;
}

static void Initialize(dwDevice_t *dev)
{
  uint32_t now_ms = T2M(xTaskGetTickCount());
  tdoaEngineInit(&tdoaEngineState, now_ms, sendTdoaToEstimatorCallback, LOCODECK_TS_FREQ, TdoaEngineMatchingAlgorithmRandom);

#ifdef CONFIG_DECK_LOCO_2D_POSITION
  DEBUG_PRINT("2D positioning enabled at %f m height\n", DECK_LOCO_2D_POSITION_HEIGHT);
#endif

  dwSetReceiveWaitTimeout(dev, TDOA3_RECEIVE_TIMEOUT);

  dwCommitConfiguration(dev);

  rangingOk = false;
  teslaQueue = xQueueCreate(10, sizeof(tdoaMeasurement_t));
}

static bool isRangingOk()
{
  return rangingOk;
}

void setIntentionCallback(void)
{
  // The parameter has been updated before the callback and the new parameter value can be used
  eventTrigger_intentionChanged_payload.intent = intention;
  eventTrigger(&eventTrigger_intentionChanged);
}

void setSyncStartTimeCallback(void)
{
  // The parameter has been updated before the callback and the new parameter value can be used
  eventTrigger_syncStartTimeSet_payload.syncStartTime = syncStartTime;
  eventTrigger(&eventTrigger_syncStartTimeSet);
}

uwbAlgorithm_t uwbTdoa3TagAlgorithm = {
    .init = Initialize,
    .onEvent = onEvent,
    .isRangingOk = isRangingOk,
    .getAnchorPosition = getAnchorPosition,
    .getAnchorIdList = getAnchorIdList,
    .getActiveAnchorIdList = getActiveAnchorIdList,
};

PARAM_GROUP_START(tdoa3)
/**
 * @brief The measurement noise to use when sending TDoA measurements to the estimator.
 */
PARAM_ADD(PARAM_FLOAT, stddev, &stdDev)
PARAM_ADD_WITH_CALLBACK(PARAM_UINT8, intent, &intention, &setIntentionCallback)
PARAM_ADD_WITH_CALLBACK(PARAM_UINT32, startSyncTime, &syncStartTime, &setSyncStartTimeCallback)

PARAM_GROUP_STOP(tdoa3)

LOG_GROUP_START(tdoa3)

LOG_ADD(LOG_UINT16, activeanchors, &activeAnchors)
LOG_ADD(LOG_UINT8, aacount, &activeAnchorsCount)
LOG_ADD(LOG_UINT8, acount, &anchorsCount)
LOG_ADD(LOG_FLOAT, synctime, &syncTime)
LOG_ADD(LOG_FLOAT, syncrmse, &syncError)
LOG_ADD(LOG_UINT32, syncmsgcount, &syncMsgCounter)
LOG_ADD(LOG_FLOAT, syncresidual, &syncResidual)

LOG_GROUP_STOP(tdoa3)
