//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_COMMANDPARSING_H
#define SICSA_COMMANDPARSING_H
/*************主要头文件*********************/
#include <stdint.h>
#include<string.h>
#include <stdio.h>
#include"../global.h"
#include <malloc.h>

/*************业务函数*********************/
void fcommandParsingInit();//业务相关值和数据结构初始化
void fcommandParsingMain();//具体的业务操作
void fcommandParsingEnd(); // 清理指令解析模块资源
/*************主要函数*********************/
uint8_t requestInstructionParsing(uint8_t data[]);
uint8_t instructionParsing(uint8_t data[]);
uint8_t instructionPacketParsing(uint16_t data[]);
void releaseCommand(uint8_t *command);
void forwardCommand(uint8_t *command);



/*************业务参数*********************/
uint16_t g_instructionTableNum = 0;//	指令表计数
uint16_t g_instructionTableNumID;//	指令表ID计数  干什么的？




#endif //SICSA_COMMANDPARSING_H
