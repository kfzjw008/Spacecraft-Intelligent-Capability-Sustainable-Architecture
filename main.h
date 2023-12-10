//
// Created by kfzjw008 on 2023-05-17.
//

#ifndef SICSA_MAIN_H
#define SICSA_MAIN_H
#include "onboardControlServices/commandParsing.h"
#include "onboardControlServices/dataInjection.h"
#include "onboardControlServices/eventTable.h"
#include "onboardControlServices/macroInstruction.h"
#include "onboardControlServices/onlineMonitoring.h"
#ifdef __linux__
// Linux平台需要的头文件
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(_WIN64)
// Windows平台需要的头文件
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#endif

void nezhaInit();
void nezha();
void nezhaEnd();
void mainInit();
void collectDataCommunication();
uint8_t checkDataInjectionTag();
uint8_t checkCommandParsingTag();
uint8_t checkMacroInstructionTag();
uint8_t checkEventTableTag();
uint8_t checkOnlineMonitoringTag();

uint8_t g_EndlessFLag;//	系统停止标识，0为正常运行，1为停止
#endif //SICSA_MAIN_H
