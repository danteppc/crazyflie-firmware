/*
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * LPS node firmware.
 *
 * Copyright 2016, Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lpsTdoa2Tag.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lpsTdoa2Tag.c.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "autoconf.h"
#include "log.h"
#include "param.h"
#include "lpsTdoa2Tag.h"

#include "stabilizer_types.h"
#include "cfassert.h"

#include "estimator.h"

#include "physicalConstants.h"
#include "tdoaEngineInstance.h"

#include "debug.h"

#include "hmac_md5.h"

#if ANCHOR_STORAGE_COUNT < LOCODECK_NR_OF_TDOA2_ANCHORS
  #error "Tdoa engine storage is too small"
#endif
#if REMOTE_ANCHOR_DATA_COUNT < LOCODECK_NR_OF_TDOA2_ANCHORS
  #error "Tdoa engine storage is too small"
#endif
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

// Config
static lpsTdoa2AlgoOptions_t defaultOptions = {
   .anchorAddress = {
     0xbccf000000000000,
     0xbccf000000000001,
     0xbccf000000000002,
     0xbccf000000000003,
     0xbccf000000000004,
     0xbccf000000000005,
     0xbccf000000000006,
     0xbccf000000000007,
   },
};

static lpsTdoa2AlgoOptions_t* options = &defaultOptions;

// State
typedef struct {
  uint32_t anchorStatusTimeout;
} history_t;

static uint8_t previousAnchor;
// Holds data for the latest packet from all anchors
static history_t history[LOCODECK_NR_OF_TDOA2_ANCHORS];


// LPP packet handling
static lpsLppShortPacket_t lppPacket;
static bool lppPacketToSend;
static int lppPacketSendTryCounter;

static void lpsHandleLppShortPacket(const uint8_t srcId, const uint8_t *data, tdoaAnchorContext_t* anchorCtx);

// Log data
static float logUwbTdoaDistDiff[LOCODECK_NR_OF_TDOA2_ANCHORS];
static float logClockCorrection[LOCODECK_NR_OF_TDOA2_ANCHORS];
static uint16_t logAnchorDistance[LOCODECK_NR_OF_TDOA2_ANCHORS];

static bool rangingOk;
static float stdDev = TDOA_ENGINE_MEASUREMENT_NOISE_STD;

uint32_t tesla_counter = 0;
bool tesla_init = false;


// The default receive time in the anchors for messages from other anchors is 0
// and is overwritten with the actual receive time when a packet arrives.
// That is, if no message was received the rx time will be 0.
static bool isValidTimeStamp(const int64_t anchorRxTime) {
  return anchorRxTime != 0;
}

static bool isConsecutiveIds(const uint8_t previousAnchor, const uint8_t currentAnchor) {
  return (((previousAnchor + 1) & 0x07) == currentAnchor);
}

static void updateRemoteData(tdoaAnchorContext_t* anchorCtx, const rangePacket2_t* packet) {
  const uint8_t anchorId = tdoaStorageGetId(anchorCtx);
  for (uint8_t i = 0; i < LOCODECK_NR_OF_TDOA2_ANCHORS; i++) {
    if (anchorId != i) {
      uint8_t remoteId = i;
      int64_t remoteRxTime = packet->timestamps[i];
      uint8_t remoteSeqNr = packet->sequenceNrs[i] & 0x7f;

      if (isValidTimeStamp(remoteRxTime)) {
        tdoaStorageSetRemoteRxTime(anchorCtx, remoteId, remoteRxTime, remoteSeqNr);
      }

      bool hasDistance = (packet->distances[i] != 0);
      if (hasDistance) {
        int64_t tof = packet->distances[i];
        if (isValidTimeStamp(tof)) {
          tdoaStorageSetTimeOfFlight(anchorCtx, remoteId, tof);

          if (isConsecutiveIds(previousAnchor, anchorId)) {
            logAnchorDistance[anchorId] = packet->distances[previousAnchor];
          }
        }
      }
    }
  }
}

static void handleLppPacket(const int dataLength, const packet_t* rxPacket, tdoaAnchorContext_t* anchorCtx) {
  const int32_t payloadLength = dataLength - MAC802154_HEADER_LENGTH;
  const int32_t startOfLppDataInPayload = LPS_TDOA2_LPP_HEADER;
  const int32_t lppDataLength = payloadLength - startOfLppDataInPayload;

  if (lppDataLength > 0) {
    const uint8_t lppPacketHeader = rxPacket->payload[LPS_TDOA2_LPP_HEADER];
    if (lppPacketHeader == LPP_HEADER_SHORT_PACKET) {
      int srcId = -1;

      for (int i=0; i < LOCODECK_NR_OF_TDOA2_ANCHORS; i++) {
        if (rxPacket->sourceAddress == options->anchorAddress[i]) {
          srcId = i;
          break;
        }
      }

      if (srcId >= 0) {
        lpsHandleLppShortPacket(srcId, &rxPacket->payload[LPS_TDOA2_LPP_TYPE], anchorCtx);
      }
    }
  }
}

// Send an LPP packet, the radio will automatically go back in RX mode
static void sendLppShort(dwDevice_t *dev, lpsLppShortPacket_t *packet)
{
  static packet_t txPacket;
  dwIdle(dev);

  MAC80215_PACKET_INIT(txPacket, MAC802154_TYPE_DATA);

  txPacket.payload[LPS_TDOA2_TYPE_INDEX] = LPP_HEADER_SHORT_PACKET;
  memcpy(&txPacket.payload[LPS_TDOA2_SEND_LPP_PAYLOAD_INDEX], packet->data, packet->length);

  txPacket.pan = 0xbccf;
  txPacket.sourceAddress = 0xbccf000000000000 | 0xff;
  txPacket.destAddress = options->anchorAddress[packet->dest];

  dwNewTransmit(dev);
  dwSetDefaults(dev);
  dwSetData(dev, (uint8_t*)&txPacket, MAC802154_HEADER_LENGTH+1+packet->length);

  dwWaitForResponse(dev, true);
  dwStartTransmit(dev);
}

static uint8_t buffered_mac[8][8] = {{'\0'},{'\0'},{'\0'},{'\0'},{'\0'},{'\0'},{'\0'},{'\0'}};
static float buffered_m[8][3];

// this is supposed to be sent by LPP, but here we buffered to save memory since we know they don't change. TODO: dynamic buffer
static const float constellation[8][3] = {{ -1.0,-1.0,0.0 },
                                          { -1.0,+1.0,1.0 },
                                          { +1.0,+1.0,0.0 },
                                          { +1.0,-1.0,1.0 },
                                          { -1.0,-1.0,1.0 },
                                          { -1.0,+1.0,0.0 },
                                          { +1.0,+1.0,1.0 },
                                          { +1.0,-1.0,0.0 }};
                                        
static const uint8_t hashebytes[8][16] = {
    {0x1f,0x2e,0x2b,0x19,0xf2,0xb9,0xdb,0x68,0x5e,0xf0,0x5b,0x65,0x38,0x5a,0x40,0x62},// Anchor('0',-1,-1,0)
    {0xaf,0xc1,0xd3,0xc0,0xf6,0x82,0xb3,0xe6,0x9b,0xe9,0xff,0xfe,0x71,0x39,0xe0,0x68},// Anchor('1',-1,+1,1)
    {0xe4,0x8f,0xd3,0xe1,0xc9,0x4b,0xec,0xc9,0x27,0x61,0x82,0x7a,0x68,0x00,0x3c,0xdc},// Anchor('2',+1,+1,0)
    {0xa9,0x2c,0x1a,0xef,0x23,0xfb,0x38,0xdf,0xeb,0x27,0x78,0xbc,0xe8,0x0f,0x7b,0xf8},// Anchor('3',+1,-1,1)
    {0xcb,0x4d,0x44,0x6a,0x2e,0xbf,0xf9,0x49,0x5e,0x60,0x24,0x7b,0x83,0x5f,0xa1,0xf2},// Anchor('4',-1,-1,1)
    {0x06,0xe3,0xf9,0xff,0x1f,0x8d,0xb3,0x29,0x87,0x8c,0x17,0x15,0x29,0xd5,0x94,0x8a},// Anchor('5',-1,+1,0)
    {0xc9,0xcc,0xdc,0xfe,0xa6,0x75,0x0d,0xda,0x1c,0x5e,0x82,0x0f,0x4e,0xca,0xcb,0x5e},// Anchor('6',+1,+1,1)
    {0x39,0xb5,0xd4,0xeb,0x7f,0xed,0x44,0xdd,0x43,0x70,0x21,0x2c,0xff,0x27,0x43,0x17} // Anchor('7',+1,-1,0)
};
 
  
#define TESLA_TOTAL_DURATION 10
md5_byte_t keychains[8][TESLA_TOTAL_DURATION] = {{'0'},{'1'},{'2'},{'3'},{'4'},{'5'},{'6'},{'7'}};

static void genMD5(md5_byte_t *input, uint8_t len, md5_byte_t *output) {

  md5_state_t hash_state;

    md5_init(&hash_state);
    md5_append(&hash_state, input, len);
    md5_finish(&hash_state, output);
}
/* len= 10
30cf554cd26b8c97c4ff
keychain for anchor id=1
31c4ff00935c2884ec16
keychain for anchor id=2
32c8996d6fd9ae029ece
keychain for anchor id=3
33ec16bfd6730386dcb9

keychain for anchor id=4
34a885a09a6d6fd9ae02

keychain for anchor id=5
35e4c119e5730386dcb9
keychain for anchor id=6
3616bfd6730386dcb9f3
keychain for anchor id=7
378f32c8996d6fd9ae02
static unsigned char k0s[8] = {0xff,0x16,0xce,0xb9,0x02,0xb9,0xf3,0x02};

*/
/*
len = 16
keychain for anchor id=0
30cf554cd26b8c97c4ff00935c2884ec
keychain for anchor id=1
31c4ff00935c2884ec16bfd6730386dc
keychain for anchor id=2
32c8996d6fd9ae029ecebdab24c3d77c
keychain for anchor id=3
33ec16bfd6730386dcb9f3634aff0093
keychain for anchor id=4
34a885a09a6d6fd9ae029ecebdab24c3
keychain for anchor id=5
35e4c119e5730386dcb9f3634aff0093
keychain for anchor id=6
3616bfd6730386dcb9f3634aff00935c
keychain for anchor id=7
378f32c8996d6fd9ae029ecebdab24c3
*/
//static unsigned char k0s[8] = {0xec,0xdc,0x7c,0x93,0xc3,0x93,0x5c,0xc3};

static unsigned char commitments[8] = {0xff,0x16,0xce,0xb9,0x02,0xb9,0xf3,0x02}; 
static const unsigned char k0s[8] = {0xff,0x16,0xce,0xb9,0x02,0xb9,0xf3,0x02};

static uint32_t iterations_to_find[10] = {0,0,0,0,0,0,0,0,0,0};

#define ITERATIONS_TO_FIND_K0 TESLA_TOTAL_DURATION
static const char ids[8] = {'0','1','2','3','4','5','6','7'};

static bool isLastKeyByte(md5_byte_t keybyte) {
  bool isLast = false;
  for (uint8_t i = 0; i < 8 ; i++) 
    if (ids[i] == keybyte) 
      isLast = true;  
  return isLast;
}

static bool isGoodKey(md5_byte_t *keybyte, uint8_t anchorId) {
  md5_byte_t itrating_keybyte = *keybyte;
    for (uint8_t i = 0; i <= ITERATIONS_TO_FIND_K0; i++) {
        md5_byte_t output[16];
        genMD5(&itrating_keybyte, 1, output);
        if (output[0] == commitments[anchorId] || output[0] == k0s[anchorId]) {
          if (!isLastKeyByte(*keybyte)) {
            commitments[anchorId] = *keybyte; // update key commitment
          } else {
            memcpy(commitments,k0s,8); // artifically reset commitments
          }
            iterations_to_find[i]=iterations_to_find[i]+1;
            return true;
            break;
        } else {
           memcpy(&itrating_keybyte, &output[0], 1);
        }
    }
    return false;
}

static uint16_t passed_packets = 0;
static uint16_t unpassed_packets = 0;

static uint32_t prevCounter = 0;


static md5_byte_t currentKey[8] = {'0','0','0','0','0','0','0','0'};
static uint32_t currentInterval = 1;

static uint32_t anchorTeslaCounter = 0;

static uint32_t currentIntervalLoco = 0;
static md5_byte_t currentKeyLoco;


static bool rxcallback(dwDevice_t *dev) {
    tdoaStats_t* stats = &tdoaEngineState.stats;
    STATS_CNT_RATE_EVENT(&stats->packetsReceived);
    
    int dataLength = dwGetDataLength(dev);
    packet_t rxPacket;
    
    dwGetData(dev, (uint8_t*)&rxPacket, dataLength);
    const rangePacket2_t* packet = (rangePacket2_t*)rxPacket.payload;
    
    bool lppSent = false;
    if (packet->type == PACKET_TYPE_TDOA2) {
        const uint8_t anchor = rxPacket.sourceAddress & 0xff;
        
        // Check if we need to send the current LPP packet
        if (lppPacketToSend && lppPacket.dest == anchor) {
            sendLppShort(dev, &lppPacket);
            lppSent = true;
        }
                
        dwTime_t arrival = {.full = 0};
        dwGetReceiveTimestamp(dev, &arrival);
        
        if (tesla_counter) {
            tesla_counter = tesla_counter;
        }
        
        if (tesla_init && prevCounter == 0) {
          prevCounter = tesla_counter;
        }

        if (tesla_init && (tesla_counter - prevCounter > 10000)) { // wait 10 secs after init
            if (anchor < LOCODECK_NR_OF_TDOA2_ANCHORS) {
                
                const uint8_t *data = &rxPacket.payload[LPS_TDOA2_LPP_TYPE];
                uint8_t type = data[0];
                if (type == LPP_SHORT_ANCHORPOS) {
                    struct lppShortAnchorPos_s *newpos = (struct lppShortAnchorPos_s*)&data[1];
                    anchorTeslaCounter = newpos->tesla_counter;
                    currentIntervalLoco = newpos->currentInterval;
                    currentKeyLoco = newpos->currentKeyByte;
                    if (*buffered_mac[anchor] == '\0') { // is empty
                        memcpy(buffered_mac[anchor], newpos->hash, 8);
                        buffered_m[anchor][0] = newpos->x;
                        buffered_m[anchor][1] = newpos->y;
                        buffered_m[anchor][2] = newpos->z;
                        return lppSent;
                    } else { // intention: it cannot be the case that buffer is null but a key has not been disclosed
                        if (isGoodKey(&newpos->key[0], anchor) == false) {
                            return lppSent;
                        }
                        currentInterval = MAX((uint32_t)(tesla_counter/1000),1);
                        currentKey[0] = (uint8_t)keychains[anchor][(TESLA_TOTAL_DURATION-2)-(currentInterval%(TESLA_TOTAL_DURATION-1))];
                        md5_byte_t digest[16];
                        // the fact that m1 and m1' are the same removes the need for msg buffer, at least in the lab, TODO: do it properly
                        hmac_md5((unsigned char *)newpos, 12+8, currentKey, 8, digest);
                        // we actually need to do h(buffered_m[anchor]) || buffered_mac[anchor] == digest but we skip this in our experiment, TODO: do it properly
                        //if (memcmp(digest, buffered_mac[anchor], 8) == 0) {
                        if (memcmp(digest, newpos->hash, 8) == 0) {
                            // authentic
                            passed_packets++;
                            memcpy(buffered_mac[anchor], newpos->hash, 8);
                        } else {
                            unpassed_packets++;
                            memcpy(buffered_mac[anchor], newpos->hash, 8);
                            return lppSent; //mismatch
                        }
                    }
                }
                
                
                uint32_t now_ms = T2M(xTaskGetTickCount());
                
                const int64_t rxAn_by_T_in_cl_T = arrival.full;
                const int64_t txAn_in_cl_An = packet->timestamps[anchor];
                const uint8_t seqNr = packet->sequenceNrs[anchor] & 0x7f;
                
                tdoaAnchorContext_t anchorCtx;
                tdoaEngineGetAnchorCtxForPacketProcessing(&tdoaEngineState, anchor, now_ms, &anchorCtx);
                updateRemoteData(&anchorCtx, packet);
                tdoaEngineProcessPacket(&tdoaEngineState, &anchorCtx, txAn_in_cl_An, rxAn_by_T_in_cl_T);
                tdoaStorageSetRxTxData(&anchorCtx, rxAn_by_T_in_cl_T, txAn_in_cl_An, seqNr);
                
                logClockCorrection[anchor] = tdoaStorageGetClockCorrection(&anchorCtx);
                
                previousAnchor = anchor;
                
                handleLppPacket(dataLength, &rxPacket, &anchorCtx);
                
                rangingOk = true;
            }
        }
    }
    
    return lppSent;
}

static void setRadioInReceiveMode(dwDevice_t *dev) {
  dwNewReceive(dev);
  dwSetDefaults(dev);
  dwStartReceive(dev);
}

static uint32_t onEvent(dwDevice_t *dev, uwbEvent_t event) {
  switch(event) {
    case eventPacketReceived:
      if (rxcallback(dev)) {
        lppPacketToSend = false;
      } else {
        setRadioInReceiveMode(dev);

        // Discard lpp packet if we cannot send it for too long
        if (++lppPacketSendTryCounter >= TDOA2_LPP_PACKET_SEND_TIMEOUT) {
          lppPacketToSend = false;
        }
      }

      if (!lppPacketToSend) {
        // Get next lpp packet
        lppPacketToSend = lpsGetLppShort(&lppPacket);
        lppPacketSendTryCounter = 0;
      }
      break;
    case eventTimeout:
      setRadioInReceiveMode(dev);
      break;
    case eventReceiveTimeout:
      setRadioInReceiveMode(dev);
      break;
    case eventPacketSent:
      // Service packet sent, the radio is back to receive automatically
      break;
    default:
      ASSERT_FAILED();
  }

  uint32_t now = xTaskGetTickCount();
  uint16_t rangingState = 0;
  for (int anchor = 0; anchor < LOCODECK_NR_OF_TDOA2_ANCHORS; anchor++) {
    if (now < history[anchor].anchorStatusTimeout) {
      rangingState |= (1 << anchor);
    }
  }
  locoDeckSetRangingState(rangingState);

  return MAX_TIMEOUT;
}


static void sendTdoaToEstimatorCallback(tdoaMeasurement_t* tdoaMeasurement) {
  // Override the default standard deviation set by the TDoA engine.
  tdoaMeasurement->stdDev = stdDev;

  estimatorEnqueueTDOA(tdoaMeasurement);

  #ifdef CONFIG_DECK_LOCO_2D_POSITION
  heightMeasurement_t heightData;
  heightData.timestamp = xTaskGetTickCount();
  heightData.height = DECK_LOCO_2D_POSITION_HEIGHT;
  heightData.stdDev = 0.0001;
  estimatorEnqueueAbsoluteHeight(&heightData);
  #endif

  const uint8_t idA = tdoaMeasurement->anchorIds[0];
  const uint8_t idB = tdoaMeasurement->anchorIds[1];
  if (isConsecutiveIds(idA, idB)) {
    logUwbTdoaDistDiff[idB] = tdoaMeasurement->distanceDiff;
  }
}


static void Initialize(dwDevice_t *dev) {

  uint32_t now_ms = T2M(xTaskGetTickCount());
  tdoaEngineInit(&tdoaEngineState, now_ms, sendTdoaToEstimatorCallback, LOCODECK_TS_FREQ, TdoaEngineMatchingAlgorithmYoungest);

  previousAnchor = 0;

  lppPacketToSend = false;

  locoDeckSetRangingState(0);
  dwSetReceiveWaitTimeout(dev, TDOA2_RECEIVE_TIMEOUT);

  dwCommitConfiguration(dev);

  rangingOk = false;
    
  const int len = TESLA_TOTAL_DURATION;
  for (int index = 0; index < 8;index++) {
      for (int i = 1; i < len; i++) {
          md5_byte_t output[16];
          genMD5(&keychains[index][i-1], 1, output);
          keychains[index][i] = output[0];
      }
  }
}

static bool isRangingOk()
{
  return rangingOk;
}

static bool getAnchorPosition(const uint8_t anchorId, point_t* position) {
  tdoaAnchorContext_t anchorCtx;
  uint32_t now_ms = T2M(xTaskGetTickCount());

  bool contextFound = tdoaStorageGetAnchorCtx(tdoaEngineState.anchorInfoArray, anchorId, now_ms, &anchorCtx);
  if (contextFound) {
    tdoaStorageGetAnchorPosition(&anchorCtx, position);
    return true;
  }

  return false;
}

static uint8_t getAnchorIdList(uint8_t unorderedAnchorList[], const int maxListSize) {
  return tdoaStorageGetListOfAnchorIds(tdoaEngineState.anchorInfoArray, unorderedAnchorList, maxListSize);
}

static uint8_t getActiveAnchorIdList(uint8_t unorderedAnchorList[], const int maxListSize) {
  uint32_t now_ms = T2M(xTaskGetTickCount());
  return tdoaStorageGetListOfActiveAnchorIds(tdoaEngineState.anchorInfoArray, unorderedAnchorList, maxListSize, now_ms);
}

// Loco Posisioning Protocol (LPP) handling
static void lpsHandleLppShortPacket(const uint8_t srcId, const uint8_t *data, tdoaAnchorContext_t* anchorCtx)
{
  uint8_t type = data[0];
  if (type == LPP_SHORT_ANCHORPOS) {
    if (srcId < LOCODECK_NR_OF_TDOA2_ANCHORS) {
      struct lppShortAnchorPos_s *newpos = (struct lppShortAnchorPos_s*)&data[1];
      tdoaStorageSetAnchorPosition(anchorCtx, newpos->x, newpos->y, newpos->z);
      if (srcId == 3) { // why 3?
        if (newpos->hash) {
          //got it!
        }
      }
    }
  }
}

uwbAlgorithm_t uwbTdoa2TagAlgorithm = {
  .init = Initialize,
  .onEvent = onEvent,
  .isRangingOk = isRangingOk,
  .getAnchorPosition = getAnchorPosition,
  .getAnchorIdList = getAnchorIdList,
  .getActiveAnchorIdList = getActiveAnchorIdList,
};

void lpsTdoa2TagSetOptions(lpsTdoa2AlgoOptions_t* newOptions) {
  options = newOptions;
}

LOG_GROUP_START(tdoa2)
LOG_ADD(LOG_FLOAT, d7-0, &logUwbTdoaDistDiff[0])
LOG_ADD(LOG_FLOAT, d0-1, &logUwbTdoaDistDiff[1])
LOG_ADD(LOG_FLOAT, d1-2, &logUwbTdoaDistDiff[2])
LOG_ADD(LOG_FLOAT, d2-3, &logUwbTdoaDistDiff[3])
LOG_ADD(LOG_FLOAT, d3-4, &logUwbTdoaDistDiff[4])
LOG_ADD(LOG_FLOAT, d4-5, &logUwbTdoaDistDiff[5])
LOG_ADD(LOG_FLOAT, d5-6, &logUwbTdoaDistDiff[6])
LOG_ADD(LOG_FLOAT, d6-7, &logUwbTdoaDistDiff[7])

LOG_ADD(LOG_FLOAT, cc0, &logClockCorrection[0])
LOG_ADD(LOG_FLOAT, cc1, &logClockCorrection[1])
LOG_ADD(LOG_FLOAT, cc2, &logClockCorrection[2])
LOG_ADD(LOG_FLOAT, cc3, &logClockCorrection[3])
LOG_ADD(LOG_FLOAT, cc4, &logClockCorrection[4])
LOG_ADD(LOG_FLOAT, cc5, &logClockCorrection[5])
LOG_ADD(LOG_FLOAT, cc6, &logClockCorrection[6])
LOG_ADD(LOG_FLOAT, cc7, &logClockCorrection[7])

LOG_ADD(LOG_UINT16, dist7-0, &logAnchorDistance[0])
LOG_ADD(LOG_UINT16, dist0-1, &logAnchorDistance[1])
LOG_ADD(LOG_UINT16, dist1-2, &logAnchorDistance[2])
LOG_ADD(LOG_UINT16, dist2-3, &logAnchorDistance[3])
LOG_ADD(LOG_UINT16, dist3-4, &logAnchorDistance[4])
LOG_ADD(LOG_UINT16, dist4-5, &logAnchorDistance[5])
LOG_ADD(LOG_UINT16, dist5-6, &logAnchorDistance[6])
LOG_ADD(LOG_UINT16, dist6-7, &logAnchorDistance[7])

LOG_ADD(LOG_UINT32, teslaCounterCF, &tesla_counter)
LOG_ADD(LOG_UINT32, teslaIntervalCF, &currentInterval)
LOG_ADD(LOG_UINT8, teslaKeyCF, &currentKey[0])
LOG_ADD(LOG_UINT16, teslaMsgsAuthCount, &passed_packets)
//LOG_ADD(LOG_UINT16, teslaMsgsUnauthCount, &unpassed_packets)
LOG_ADD(LOG_UINT32, teslaCounterLoco, &anchorTeslaCounter)
LOG_ADD(LOG_UINT16, teslaUnpassed, &unpassed_packets)

//LOG_ADD(LOG_UINT8, teslaKeyLoco, currentKeyByte)
//LOG_ADD(LOG_UINT32, teslaIntervalLoco, currentInterval)
//LOG_ADD(LOG_UINT32, teslaCounterLoco, &tesla_counter)

LOG_GROUP_STOP(tdoa2)

PARAM_GROUP_START(tdoa2)
/**
 * @brief The measurement noise to use when sending TDoA measurements to the estimator.
 */
PARAM_ADD(PARAM_FLOAT, stddev, &stdDev)

PARAM_GROUP_STOP(tdoa2)
