/**
Demo FloyDeck for TUWIEN
 */


#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_conf.h"
#include "md5.h"
#include "functions.h"
#include "sender.h"
#include "receiver.h"

#define DEBUG_MODULE ""
#include "debug.h"
#include "locodeck.h"
#include "lpsTdoa2Tag.h"

char data_raw[12]= "Hello World";
char msg1[12]= "TU WIEN";

void appMain() {
  DEBUG_PRINT("Waiting for activation ...\n");
  uint8_t *keyObject= malloc(16);
  keyObject = md5String(data_raw);
  while(1) {
    vTaskDelay(M2T(2000));
    //uint8_t *input = &sentence[0];
    //uint8_t output[16];
    //uint8_t *key0;
    
    //uint8_t *MACmsg;
    /*uint8_t *resultpow = malloc(16);
    uint8_t *mac1 = malloc(16);*/
    //uint8_t *mac_result;
    //uint8_t *zero = malloc (16);
    //uint8_t anchor_list =malloc(16);

    point_t position;
    uint8_t unorderedAnchorList[7];

    
    locoDeckGetAnchorIdList(unorderedAnchorList, 7);
    locoDeckGetActiveAnchorIdList(unorderedAnchorList, 7);
    
    // We print the result of each packets
    //anchor_list=locoDeckGetActiveAnchorIdList(anchor_list, 8);

    //DEBUG_PRINT("%02x", unorderedAnchorList[0]);
    //keyObject = md5Number(keyObject);
    //resultpow=md5Pow(keyObject,1);//Index
    //mac1=mac(keyObject,data_raw);
    //mac1=&anchor_list;
   // print_hash(resultpow);
   for(unsigned int i = 0; i < 7; ++i){
    locoDeckGetAnchorPosition(i, &position);


    //The VALUES ARE IN FLOAT THEY ARE ONLY CONVERT TO DOUBLE FOR THE PRINT 
    double x =position.x ;
    double y =position.y;
    double z =position.z;
		//DEBUG_PRINT("%02x", unorderedAnchorList[i]);
    DEBUG_PRINT("pos %d : (%f/%f/%f) x,y,z  ", i ,x ,y,z );
    x =0 ;
    y =0;
    z =0;
    
	  }
   /*for(unsigned int i = 0; i < 16; ++i){
		DEBUG_PRINT("%02x", mac1[i]);
    
	  }
  	DEBUG_PRINT("\n");*/

    /*ErrorStatus err = ERROR;
    err = HASH_MD5(input, 16, output);*/
    //DEBUG_PRINT("%d \n", *resultpow);
  }
}
