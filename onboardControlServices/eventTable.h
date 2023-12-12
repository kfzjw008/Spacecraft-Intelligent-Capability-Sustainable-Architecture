//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_EVENTTABLE_H
#define SICSA_EVENTTABLE_H

#endif //SICSA_EVENTTABLE_H

#define EVENT_ITEM_SIZE 10  // 事件项大小
#define TIME_CODE_OFFSET 2   //时间码偏移
/*************主要头文件*********************/
#include <stdint.h>
#include<string.h>
#include <stdio.h>
#include"../global.h"
#include <malloc.h>
#include <stdbool.h>

/*************业务函数*********************/

void feventTableMain();
void feventTableInit();
void feventTableEnd(); // 清理事件表模块资源


/*************主要函数*********************/
void FormEventTable();
void ReleaseEventItem(uint32_t eventItemAddress);
void TimeEventItem();
void StartLoopTiming();
void StopLoopTiming();
void eventTableAppend();
void eventTablerePlacement();
uint8_t manageEventTable(uint8_t operation, uint16_t eventId, void* eventData);
void maintainEventTable() ;
/*************业务参数*********************/
// 假设的辅助函数，需要根据实际情况实现
uint32_t GetCurrentSystemTime(); // 获取当前系统时间

volatile bool loopTimingActive = false;// 全局变量，控制循环定时