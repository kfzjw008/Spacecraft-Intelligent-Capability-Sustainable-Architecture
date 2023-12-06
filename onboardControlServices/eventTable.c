//
// Created by herui on 2023-11-23.
//
#include <stdio.h>
#include <stdint.h>
#include "eventTable.h"
#include "../global.h"
#include <stdbool.h>
/********************************************************************
     工程：天基载荷-哪吒架构
     作者：He Rui
     修改者：He Rui
     测试：
     描述：该文件包含实现天基载荷中哪吒架构事件表操作的函数。
           这些函数包括事件项的添加、删除和初始化等，用于管理事件表。
     主要函数：
           addEvent - 添加新的事件项。
           deleteEvent - 删除特定ID的事件项。
           initializeEventTable - 初始化事件表。

     版本：1.0 - 2023-11-23
     修改：
           - 初始创建 - 2023-11-23
     参考文献：
           - 支持自适应科学探测的航天器智能能力可持续构建架构-需求规格说明
           - 支持自适应科学探测的航天器智能能力可持续构建架构-设计文档
********************************************************************/
/********************************************************************
功能：   事件表形成
参数：   无。
返回值：	无。
主要思路：
        根据事件项时间码对事件检索表重排序，当时间相同时，按其在指令包中出现的顺序排序。
调用方法：
        formEventTable();
        // 无需传入任何参数
编写日期：2023-11-23
修改记录：
********************************************************************/
void FormEventTable() {
    uint8_t* eventItemTable = (uint8_t*)EVENT_TAB_STR_ADRS;
    uint32_t* eventSearchTable = (uint32_t*)EVENT_SEA_STR_ADRS;

    // 计算事件项表和事件检索表的长度
    uint32_t eventItemCount = (EVENT_TAB_END_ADRS - EVENT_TAB_STR_ADRS) / EVENT_ITEM_SIZE;
    uint32_t eventSearchItemCount = (EVENT_SEA_END_ADRS - EVENT_SEA_STR_ADRS) / sizeof(uint32_t);

    // 初始化事件检索表
    for (uint32_t i = 0; i < eventItemCount; ++i) {
        eventSearchTable[i] = EVENT_TAB_STR_ADRS + i * EVENT_ITEM_SIZE;
    }

    // 冒泡排序
    for (uint32_t i = 0; i < eventItemCount - 1; ++i) {
        for (uint32_t j = 0; j < eventItemCount - i - 1; ++j) {
            uint32_t* item1Addr = (uint32_t*)(eventSearchTable[j] + TIME_CODE_OFFSET);
            uint32_t* item2Addr = (uint32_t*)(eventSearchTable[j + 1] + TIME_CODE_OFFSET);
            
            if (*item1Addr > *item2Addr) {
                // 交换事件检索表中的项
                uint32_t temp = eventSearchTable[j];
                eventSearchTable[j] = eventSearchTable[j + 1];
                eventSearchTable[j + 1] = temp;
            }
        }
    }
    g_currentEventItemID = 0;//	事件项ID为0，表示事件检索表中的第一个地址值
}
// //测试函数
// int main() {
//     FormEventTable();
//     return 0;
// }


/********************************************************************
功能：   从事件表中释放指定的事件项，并将其指令编码存入指令动作业务的指令存储位置
参数：   eventItemAddress 事件项的存储首地址
返回值： 无
主要思路：根据提供的地址，访问事件项，提取指令编码，并将其存入指定的存储位置
调用方法：ReleaseEventItem(EVENT_TAB_STR_ADRS + offset);
编写日期：2023-11-23
修改记录：[任何修改的记录]
********************************************************************/
void ReleaseEventItem(uint32_t eventItemAddress) {
    // 确保地址在有效范围内
    if (eventItemAddress < EVENT_TAB_STR_ADRS || eventItemAddress > EVENT_TAB_END_ADRS) {
        return; // 地址无效，直接返回
    }

    uint16_t* instructionCodeAddress = (uint16_t*)(eventItemAddress + 4); // 指令编码在事件项中的偏移为4字节
    uint16_t instructionCode = *instructionCodeAddress; // 提取指令编码

    // 将指令编码存入指定的指令动作业务的存储位置
    uint16_t* targetAddress = (uint16_t*)EVENT_TO_INSEXE_ADRS;
    *targetAddress = instructionCode;

    g_currentEventItemID = g_currentEventItemID + 1;

}

// // 示例主函数
// int main() {
//     // 假设事件项的地址
//     uint32_t exampleEventItemAddress = EVENT_TAB_STR_ADRS + 0x10; // 示例地址
//     ReleaseEventItem(exampleEventItemAddress);
//     return 0;
// }

/********************************************************************
功能：   根据事件项检索表和当前事件项ID获得当前事件项，执行定时业务
参数：   无
返回值： 无
主要思路：根据当前事件项ID获取事件项，计算时间差，根据时间差决定是否立即执行或启用倒计时计数器
调用方法：TimeEventItem();
编写日期：2023-11-23
修改记录：[任何修改的记录]
********************************************************************/
void TimeEventItem() {
    if (!loopTimingActive) {
        return;
    }
    uint32_t* eventSearchTable = (uint32_t*)EVENT_SEA_STR_ADRS;
    uint32_t currentEventItemAddress = eventSearchTable[g_currentEventItemID];

    // 获取当前事件项的时间码
    uint32_t* timeCodeAddress = (uint32_t*)(currentEventItemAddress + 2); // 假设时间码位于地址偏移2字节处
    uint32_t timeCode = *timeCodeAddress;

    // 计算时间差
    uint32_t currentTime = GetCurrentSystemTime();
    uint32_t timeDiff = timeCode > currentTime ? timeCode - currentTime : 0;

    // 判断时间差并执行相应操作
    if (timeDiff < 1) {
        // 时间差小于1秒，立即执行
        ReleaseEventItem(currentEventItemAddress);
    } else {
        // 时间差大于1秒，启用倒计时计数器
        while (timeDiff >= 1) {
            sleep(1); // 每秒减1，sleep函数暂停执行1秒
            timeDiff--;
        }
        ReleaseEventItem(currentEventItemAddress);
    }
}

// // 示例主函数
// int main() {
//     TimeEventItem();
//     return 0;
// }

/********************************************************************
功能：   开启循环定时，循环执行TimeEventItem函数
参数：   无
返回值： 无
主要思路：设置循环控制标志为true，循环执行TimeEventItem函数
调用方法：StartLoopTiming();
编写日期：2023-11-23
修改记录：[任何修改的记录]
********************************************************************/
void StartLoopTiming() {
    loopTimingActive = true;
    while (loopTimingActive) {
        TimeEventItem(); // 
    }
}

/********************************************************************
功能：   关闭循环定时，停止循环执行TimeEventItem函数
参数：   无
返回值： 无
主要思路：设置循环控制标志为false
调用方法：StopLoopTiming();
编写日期：2023-11-23
修改记录：[任何修改的记录]
********************************************************************/
void StopLoopTiming() {
    loopTimingActive = false;
}

