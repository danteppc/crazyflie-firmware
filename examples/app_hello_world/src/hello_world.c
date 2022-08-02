/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2019 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * hello_world.c - App layer application of a simple hello world debug print every
 *   2 seconds.
 */


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "FreeRTOS.h"
#include "task.h"
#include "log.h"
#include "param.h"

#define DEBUG_MODULE "HELLOWORLD"
#include "debug.h"
static const uint16_t unlockThLow = 100;
static const uint16_t unlockThHigh = 300;
static const uint16_t stoppedTh = 500;

// Handling the height setpoint
static const float spHeight = 0.5f;
static const uint16_t radius = 300;

// Some wallfollowing parameters and logging
bool goLeft = false;
float distanceToWall = 0.5f;
float maxForwardSpeed = 0.5f;

float cmdVelX = 0.0f;
float cmdVelY = 0.0f;
float cmdAngWRad = 0.0f;
float cmdAngWDeg = 0.0f;

void appMain() {
  DEBUG_PRINT("Waiting for activation ...\n");

  while(1) {
    vTaskDelay(M2T(2000));
    DEBUG_PRINT("Hello World!\n");
  }
}

PARAM_GROUP_START(app)
PARAM_ADD(PARAM_UINT8, goLeft, &goLeft)
PARAM_ADD(PARAM_FLOAT, distanceWall, &distanceToWall)
PARAM_GROUP_STOP(app)

LOG_GROUP_START(app)
LOG_ADD(LOG_FLOAT, cmdVelX, &cmdVelX)
LOG_ADD(LOG_FLOAT, cmdVelY, &cmdVelY)
LOG_GROUP_STOP(app)