/**
Demo FloyDeck for TUWIEN
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "commander.h"

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"

#include "log.h"
#include "param.h"

#define DEBUG_MODULE "Take Off"

static void setHoverSetpoint(setpoint_t *setpoint, float vx, float vy, float z, float yawrate)
{
  setpoint->mode.z = modeAbs;
  setpoint->position.z = z;


  setpoint->mode.yaw = modeVelocity;
  setpoint->attitudeRate.yaw = yawrate;


  setpoint->mode.x = modeVelocity;
  setpoint->mode.y = modeVelocity;
  setpoint->velocity.x = vx;
  setpoint->velocity.y = vy;

  setpoint->velocity_body = true;
}

void appMain()
{
  static setpoint_t setpoint;
  //systemWaitStart();
  bool takeoff = true;
  bool gotopos1 = false;

  DEBUG_PRINT("Waiting for activation ...\n");
  vTaskDelay(M2T(30000));
  float height = 0.4f;
  int counter = 0;
  while(takeoff) {
    counter++;
    vTaskDelay(M2T(10));
    if (gotopos1) {
      setHoverSetpoint(&setpoint, 0.3f, 0, height, 0);
      commanderSetSetpoint(&setpoint, 3);
    } else {
      setHoverSetpoint(&setpoint, 0, 0, height, 0);
      commanderSetSetpoint(&setpoint, 3);
    }
    if (counter > 500)
    	gotopos1 = true;

    if (counter > 1000)
    	takeoff = false ;
    }
    setHoverSetpoint(&setpoint, 0,0,0,0);
    commanderSetSetpoint(&setpoint, 3);
  }
