//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_EVENTTABLE_H
#define SICSA_EVENTTABLE_H

#endif //SICSA_EVENTTABLE_H

#define EVENT_ITEM_SIZE 10  // 事件项大小
#define TIME_CODE_OFFSET 2   //时间码偏移


void FormEventTable();
void feventTableMain();
void feventTableInit();
void feventTableEnd(); // 清理事件表模块资源



// 假设的辅助函数，需要根据实际情况实现
uint32_t GetCurrentSystemTime(); // 获取当前系统时间

volatile bool loopTimingActive = false;// 全局变量，控制循环定时