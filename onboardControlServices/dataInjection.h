//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_DATAINJECTION_H
#define SICSA_DATAINJECTION_H
#define INJECTION_PACKET_TYPE 1
#define NO_SECONDARY_HEADER 0
#define APPLICATION_PROCESS_ID_WITHOUT_SECONDARY_HEADER 0x1334
#define APPLICATION_PROCESS_ID_WITH_SECONDARY_HEADER 0x1B34
/*************主要头文件*********************/
#include <stdint.h>
#include <stdio.h>
#include "../global.h"
#include <stdint.h>
#include <string.h>
#include <malloc.h>
/*************业务函数*********************/
void fdataInjectionInit();
void fdataInjectionMain();
void fdataInjectionEnd(); // 清理数据注入模块资源

/*************主要函数*********************/
void injectionPacketReception(uint8_t data[], uint8_t size);
uint16_t packetSplicing();
uint16_t packetIdentification(uint8_t *data, uint16_t size);
uint8_t isInstructionPacket(uint8_t data1,uint8_t data2);
void injectionPacketUnpacking(uint8_t data[]);
int packageValidity(uint8_t *item);
int packageUniqueness();
int packageLegitimacy(uint8_t data[]);
uint8_t packageRelevance(uint32_t item);
uint8_t  injectionPacketInspection(int size);

/*************业务参数*********************/

uint16_t g_InjectionPacketBufferNum;//	注入包计数
uint8_t g_InjectionPacketFullFlag; // 注入包是否处于已接收首包状态

uint8_t g_instructionPacketBufferNum = 0;





#endif //SICSA_DATAINJECTION_H

