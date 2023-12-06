//
// Created by kfzjw008 on 2023-11-18.
//

#include "testMacro.h"
#include "../onboardControlServices/macroInstruction.h"
#include "../util.h"
#include <stdint.h>



int main2() {
    // 定义宏指令内容
    uint8_t macroContent[] = {0x01, 0x00, 0x02, 0x00, 0x03, 0x00}; // 示例宏指令内容
    uint16_t contentLength = sizeof(macroContent) / sizeof(macroContent[0]);

    // 宏指令ID
    uint8_t macroId = 1; // 示例ID

    // 宏类型（例如：允许释放，禁止删除，是宏序列，不是宏数据，是宏程序，不是静态宏，是动态宏，不是临时宏）
    uint8_t macroType = 0b01011000;

    // 计算校验和
    uint8_t checksum = calculateChecksum(macroContent, contentLength);

    // 调用添加宏指令函数
    addMacroInstruction(macroId, macroType, checksum, contentLength, macroContent);

    // 其他代码...

    return 0;
}
