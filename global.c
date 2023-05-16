//
// Created by kfzjw008 on 2023-05-16.
//

#include "global.h"
#include <stdint.h>

uint16_t g_InjectionPacketBuffer = 0;//	注入包缓冲区
uint16_t g_packetHeaderLength = 0;//	包头长度
uint16_t g_packetFooterLength = 0;//	包尾长度
uint16_t g_InjectionPacketBufferNum = 0;//	注入包计数
uint16_t g_dataInjectionFlag = 0;//	数据注入操作标识
uint16_t g_InjectionPacketBufferCheckFlag = 0;//	注入包检查标识
uint16_t packageSequenceIdentifier = 0;//	包序号标识
uint16_t g_injectionPacketRxCount = 0;//	注入包接收计数
uint16_t g_injectionFrameRxCount = 0;//	注入帧接收计数
uint16_t g_injectionSequenceOrChecksumArray = 0;//	注入序号或校验和数组
uint16_t g_instructionPool = 0;//	指令池
uint16_t g_instructionPoolNum = 0;//
uint16_t g_instructionTable = 0;//	指令表
uint16_t instructionEncodingLength = 0;//	指令编码长度
uint16_t instructionRoutingLength = 0;//	指令路由长度
uint16_t g_instructionTableNum = 0;//	指令表计数
uint16_t g_instructionTableNumID = 0;//	指令表ID计数
uint16_t g_instructionCount = 0;//	指令个数
uint16_t g_instructionReleaseCount = 0;//	指令释放计数
uint16_t g_instructionForwardCount = 0;//	指令转发计数
uint16_t g_instructionParsingAndExecutionOperationFlag = 0;//	指令解析和执行操作标识
uint16_t g_eventSearchTable = 0;//	事件检索表
uint16_t g_eventItemTable = 0;//	事件项表
uint16_t g_eventTableCapacity = 0;//	事件表容量
uint16_t g_pendingEventCount = 0;//	待执行事件计数
uint16_t g_appendedEventCount = 0;//	追加事件计数
uint16_t g_currentEventItemID = 0;//	当前事件项ID
uint16_t g_eventCount = 0;//	事件表项数
uint16_t g_macroInstructionTable = 0;//	宏指令库
uint16_t g_macroInstructionLookupTable_SE = 0;//	宏指令检索表-序列
uint16_t g_MacroInstructionCount = 0;//	宏指令个数
uint16_t g_NextAvailableMacroInstructionNumber = 0;//	下一个可分配的宏指令序号
uint16_t g_TemporaryMacroInstructionGenerationCount = 0;//	临时宏指令生成个数
uint16_t g_macroInstructionLookupTable_DA = 0;//	宏指令检索表-数据
uint16_t g_macroInstructionLookupTable_PR = 0;//	宏指令检索表-程序
uint16_t g_macroInstructionLookupTable_TE = 0;//	宏指令检索表-临时
uint16_t g_parameterMonitoringTable = 0;//	参数监视表
uint16_t g_monitoredParametersNum = 0;//	监视参数个数
uint16_t g_monitoringStatus = 0;//	监视状态
uint16_t g_actionStatus = 0;//	动作状态
uint16_t g_actionCount = 0;//	动作计数
uint16_t g_totalActionCount = 0;//	动作总计数
uint16_t g_countdownExceedLimit = 0;//	超限倒计数

//到此为止，以上为所有文档内函数