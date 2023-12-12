//
// Created by herui on 2023-11-23.
//
#include "eventTable.h"
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

/********************************************************************
 * 功能：维护事件表，清理过期或已执行的事件项
 * 参数：无
 * 返回值：无
 * 主要思路：定期检查事件表中的事件项，移除过期或已经执行的事件
 * 调用方法：maintainEventTable();
 * 编写日期：2023-11-23
 * 修改记录：[任何修改的记录]
 ********************************************************************/
void maintainEventTable() {
    // 实现维护事件表的逻辑...
    // 遍历事件表，查找过期或已执行的事件项
    // 移除过期或已执行的事件项
    // 可能需要记录或通知相关业务事件项已被移除
}

/********************************************************************
 * 功能：管理事件表，根据输入执行不同的操作（增加、删除、修改）
 * 参数：operation - 操作类型（增加、删除、修改）
 *       eventId - 事件ID
 *       eventData - 事件相关数据，根据操作类型可能包含时间码、指令编码等
 * 返回值：操作成功与否的标志
 * 主要思路：根据提供的操作类型，对事件表进行相应的增删改查操作
 * 调用方法：manageEventTable(operation, eventId, eventData);
 * 编写日期：2023-11-23
 * 修改记录：[任何修改的记录]
 ********************************************************************/
uint8_t manageEventTable(uint8_t operation, uint16_t eventId, void* eventData) {
    switch (operation) {
        case ADD_EVENT:
            // 增加事件项
            // 调用之前定义的 addEvent 函数
            addEvent(eventId, ...); // ... 表示其他需要的参数
            break;
        case DELETE_EVENT:
            // 删除事件项
            // 调用之前定义的 deleteEvent 函数
            deleteEvent(eventId);
            break;
        case UPDATE_EVENT:
            // 修改事件项
            // 调用之前定义的 updateEventInstruction 函数或其他更新函数
            updateEventInstruction(eventId, ...); // ... 表示其他需要的参数
            break;
            // 可以根据需要添加更多的操作类型
        default:
            // 未识别的操作类型
            return 0; // 表示操作失败
    }
    return 1; // 表示操作成功
}

/********************************************************************
 * 功能：事件表主要业务逻辑处理函数。
 * 参数：无。
 * 返回值：无。
 * 主要思路：循环检查事件表，执行必要的事件项处理，如触发事件执行。
 * 调用方法：feventTableMain();
 * 编写日期：2023-11-23
 * 修改记录：无
 ********************************************************************/
void feventTableMain() {
    // 初始化事件表
    feventTableInit();

    // 事件表处理循环
    while (true) {
        // 执行事件表维护，清除无效或已执行的事件项
        maintainEventTable();

        // 执行事件表管理，如添加、删除或更新事件项
        // 这里可以根据具体业务逻辑添加代码

        // 处理当前的事件项
        TimeEventItem();

        // 检查是否需要退出循环，例如接收到停止指令
        if (/* 接收到退出信号 */) {
            break;
        }
    }

    // 清理事件表模块资源
    feventTableEnd();
}

/********************************************************************
 * 功能：事件表模块初始化函数。
 * 参数：无。
 * 返回值：无。
 * 主要思路：设置事件表初始状态，为事件处理准备环境。
 * 调用方法：feventTableInit();
 * 编写日期：2023-11-23
 * 修改记录：无
 ********************************************************************/
void feventTableInit() {
    // 初始化事件表的全局变量、数据结构等
    initializeEventTable();

    // 如果有定时器或计数器，进行初始化设置
    // 初始化循环定时控制标志
    loopTimingActive = true;

    // 这里可以添加其他必要的初始化代码
}

/********************************************************************
 * 功能：清理事件表模块占用的资源。
 * 参数：无。
 * 返回值：无。
 * 主要思路：释放事件表模块分配的资源，重置状态。
 * 调用方法：feventTableEnd();
 * 编写日期：2023-11-23
 * 修改记录：无
 ********************************************************************/
void feventTableEnd() {
    // 清理事件表资源
    // ... (如果有需要释放的资源，添加相应代码)

    // 重置事件表模块相关的全局变量
    loopTimingActive = false;
    g_currentEventItemID = 0;
    // ... (如果有其他相关全局变量，添加重置代码)

    // 这里可以添加其他必要的清理代码
}

/********************************************************************
 * 功能：替换事件表
 * 参数：无
 * 返回值：无
 * 主要思路：
 *     - 停止当前事件项定时。
 *     - 将旧事件表中由宏指令序列自动加入的，且未执行的事件项加入新事件表。
 *     - 覆盖旧事件表。
 *     - 重新执行事件定时。
 * 调用方法：eventTablerePlacement();
 * 编写日期：2023-11-23
 * 修改记录：无
 ********************************************************************/
void eventTablerePlacement() {
    // 停止当前事件项定时
    StopLoopTiming();

    // TODO: 事件表覆盖和追加的具体实现
    // 这可能涉及到对事件表的直接内存操作或调用特定的数据结构操作函数

    // 将旧事件表中未执行的事件项加入新事件表
    // 注意：此处需要具体的事件表数据结构和操作函数来实现

    // 覆盖旧事件表
    // 注意：此处需要具体的事件表数据结构和操作函数来实现

    // 重新执行事件定时
    StartLoopTiming();
}

/********************************************************************
 * 功能：追加事件表
 * 参数：无
 * 返回值：无
 * 主要思路：
 *     - 按照时间顺序关系将追加的事件项插入原事件表中。
 *     - 如果追加事件项个数超出容量，则暂存至缓冲区。
 * 调用方法：eventTableAppend();
 * 编写日期：2023-11-23
 * 修改记录：无
 ********************************************************************/
void eventTableAppend() {
    // TODO: 实现将新事件项按照时间顺序插入到原事件表中
    // 这可能涉及到对事件表的直接内存操作或调用特定的数据结构操作函数

    // 如果追加事件项的个数与原事件表的待执行事件个数之和大于容量常数
    // TODO: 实现将多出的追加事件项暂时保存在接收缓冲区中
    // 注意：此处需要具体的事件表数据结构和操作函数来实现

    // TODO: 检查事件表空间，继续追加事件项
}
