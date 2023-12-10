//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_COMMANDPARSING_H
#define SICSA_COMMANDPARSING_H
#include <stdint.h>
#include<string.h>
#include <stdio.h>
#include"../global.h"



void fcommandParsingInit();
void fcommandParsingMain();
void fcommandParsingEnd(); // 清理指令解析模块资源
#endif //SICSA_COMMANDPARSING_H
