//
// Created by kfzjw008 on 2023-05-16.
//
#include <string.h>
#include "macroInstruction.h"
#include "stdio.h"
/*#include "logLib.h"*/
#include "../global.h"
#include "../util.h"
/********************************************************************
     工程：天基载荷-哪吒架构
     作者：Zhang Junwei
     修改者：Zhang Junwei
     测试：He Rui ,Qin Sheng
     描述：该文件包含实现天基载荷中哪吒架构宏指令操作的函数。
           这些函数包括宏指令的添加、删除和初始化等，用于管理宏指令表。
     主要函数：
           addMacro - 添加新的宏指令。
           deleteMacro - 删除特定ID的宏指令。
           initializeMacroInstructionTable - 初始化宏指令表。
           addMacroInstruction - 将宏指令添加到宏指令表。
     版本：1.0 - 2023-11-18
     修改：
           - 初始创建 - 2023-11-18
     参考文献：
           - 支持自适应科学探测的航天器智能能力可持续构建架构-需求规格说明
           - 支持自适应科学探测的航天器智能能力可持续构建架构-设计文档
********************************************************************/
/********************************************************************
功能：   初始化宏指令表，清空所有已存储的宏指令，并重置相关计数器。
参数：   无。
返回值：	无。
主要思路：
        函数首先将宏指令计数器重置为0，表示清空宏指令库。
        然后将下一个可用的宏指令序号重置为初始值（例如8），准备接收新的宏指令。
调用方法：
        initializeMacroInstructionTable();
        // 无需传入任何参数
编写日期：2023-11-18
修改记录：
********************************************************************/
void initializeMacroInstructionTable() {
    // 清空宏指令库
    // 重置宏指令个数
    g_MacroInstructionCount = 0;

    // 重置下一个可分配的宏指令序号
    g_NextAvailableMacroInstructionNumber = 8;
}

/********************************************************************
功能：   向宏指令表中添加一个新的宏指令。
参数：
        id - 宏指令的唯一标识符。
        type - 宏指令的类型。
        checksum - 宏指令内容的校验和。
        length - 宏指令内容的长度。
        content - 指向宏指令内容的指针。
返回值：
        1 - 如果宏指令成功添加到表中。
        0 - 如果添加失败，可能是由于宏指令表已满。
主要思路：
        首先检查宏指令表是否已满。然后，将宏指令的ID、类型、
        校验和和长度打包到一个32位整数中，并使用outpDw函数将其写入宏指令表。
        接着，将宏指令的内容逐字节写入宏指令表的后续位置。
        最后，更新宏指令表中的计数器和下一个可用的宏指令编号。
调用方法：
        uint8_t result = addMacroInstruction(id, type, checksum, length, content);
        // 其中id为宏指令ID，type为宏指令类型，checksum为校验和，length为宏指令长度，content为宏指令内容
编写日期：2023-11-10
修改记录：
********************************************************************/
uint8_t addMacroInstruction(uint8_t id, uint8_t type, uint8_t checksum, uint16_t length, uint8_t *content) {
    if (g_MacroInstructionCount >= MAX_MEMORY_SIZE / 100) {
        return 0; // 已达到宏指令库的最大容量
    }

    // 计算宏指令存储地址
    uint32_t address = g_NextAvailableMacroInstructionNumber * 100;

    // 存储宏指令第一部分（ID, 类型, 校验和, 长度）
    uint32_t firstPart = (id << 24) | (type << 16) | (checksum << 8) | length;
    outpDw(address, firstPart);

    // 存储宏指令内容
    for (int i = 0; i < length; i++) {
        outpDw(address + 1 + i, content[i]);
    }

    // 更新全局变量
    g_MacroInstructionCount++;
    g_NextAvailableMacroInstructionNumber++;

    return 1;
}
/********************************************************************
功能：   添加一个新的宏指令到宏指令表中。供数据注入指令直接调用。
参数：
        id - 宏指令的唯一标识符。
        release - 宏指令释放权限标志。
        delete - 宏指令删除权限标志。
        sequence - 是否为宏序列。
        data - 是否为宏数据。
        program - 是否为宏程序。
        staticMacro - 是否为静态宏。
        dynamicMacro - 是否为动态宏。
        temporaryMacro - 是否为临时宏。
        content - 指向宏指令内容的指针。
        length - 宏指令内容的长度。
返回值：
        1 - 如果宏指令成功添加。
        0 - 如果添加失败，通常是由于不满足约束条件。
主要思路：
        首先检查输入的宏指令类型和生命周期标志是否满足约束条件。
        然后，根据输入参数构建宏指令类型，并计算校验和。
        最后，调用addMacroInstruction函数来添加宏指令到宏指令表中。
调用方法：
        uint8_t result = addMacro(id, release, delete, sequence, data, program,
                                  staticMacro, dynamicMacro, temporaryMacro, content, length);
        // 其中id为宏指令ID，release等为宏指令的各种标志位，content为指向宏指令内容的指针，length为宏指令内容的长度
        uint8_t macroId = 1;
        uint8_t macroContent[] = {  };
        uint16_t contentLength = sizeof(macroContent) / sizeof(macroContent[0]);
        // 添加宏指令，示例宏类型设置为允许释放，禁止删除，是宏序列，不是宏数据，不是宏程序，是静态宏，不是动态宏，不是临时宏
        uint8_t success = addMacro(macroId, 1, 0, 1, 0, 0, 1, 0, 0, macroContent, contentLength);
编写日期：2023-11-18
修改记录：
********************************************************************/

uint8_t addMacro(uint8_t id, uint8_t release, uint8_t delete, uint8_t sequence, uint8_t data, uint8_t program,
                 uint8_t staticMacro, uint8_t dynamicMacro, uint8_t temporaryMacro, uint8_t *content, uint16_t length) {
    // 检查约束条件
    int macroTypeCount = (sequence != 0) + (data != 0) + (program != 0);
    int macroLifeCount = (staticMacro != 0) + (dynamicMacro != 0) + (temporaryMacro != 0);

    // 约束限制
    if (macroTypeCount != 1 || macroLifeCount != 1) {
        return 0; // 不满足约束条件，添加失败
    }

    // 构建宏类型
    uint8_t type = release | (delete << 1) | (sequence << 2) | (data << 3) | (program << 4) |
                   (staticMacro << 5) | (dynamicMacro << 6) | (temporaryMacro << 7);
    // 重新计算宏序列长度
    uint16_t newLength = length * 4-2; // 每个指令编码和间隔共占4字节


    // 计算校验和
    uint8_t checksum = calculateChecksum(content, length);

    // 调用添加宏指令函数
    return addMacroInstruction(id, type, checksum, newLength, content);
}


/********************************************************************
功能：   宏指令直接操作内存区删除。删除指定ID的宏指令。
参数：
        macroId - 要删除的宏指令的ID。
返回值：
        1 - 如果宏指令成功删除。
        0 - 如果未找到指定ID的宏指令或删除操作失败。
主要思路：
        遍历宏指令表，寻找与给定ID匹配的宏指令。如果找到，
        则从表中删除该宏指令，并将后续宏指令向上移动一行，
        以填补删除的空间。然后更新宏指令表中的宏指令数量。
调用方法：
        uint8_t result = deleteMacro(macroId);
        // 其中macroId为要删除的宏指令的ID
        // 示例：尝试删除ID为1的宏指令
        uint8_t success = deleteMacro(1);
编写日期：2023-11-18
修改记录：
********************************************************************/
// 删除宏指令
uint8_t deleteMacro(uint16_t macroId) {
    for (uint16_t i = 0; i < g_MacroInstructionCount; i++) {
        // 检查宏指令ID和是否可以删除
        if (g_macroInstructionTable[i][0] == macroId ) {
            // 删除宏指令并移动后续宏指令
            for (uint16_t j = i; j < g_MacroInstructionCount - 1; j++) {
                memcpy(g_macroInstructionTable[j], g_macroInstructionTable[j + 1], MAX_MACRO_LENGTH * sizeof(uint16_t));
            }
            // 更新宏指令个数
            g_MacroInstructionCount--;
            return 1; // 删除成功
        }
    }
    return 0; // 未找到可删除的宏指令或不允许删除
}

/********************************************************************
功能：   删除指定范围的宏指令，根据输入参数控制删除条件。供数据注入指令直接调用。
参数：
        type1 - bit1、0，指示宏指令表区。
        type2 - bit2，指示是否需要考虑宏指令的删除状态。
        type3 - bit3，指示是否可以删除静态部分的宏指令。
        type4 - bit4，指示是否无条件删除所有宏指令。
        type5 - bit5，指示是否删除一个或多个宏指令。
        n - 宏指令的数量。
        macroIds - 指向宏指令ID数组的指针。
返回值：
        0 - 删除操作全部失败。
        1 - 删除操作全部成功。
        2 - 部分删除成功。
        3 - 输入格式错误或约束不符合要求。
主要思路：
        根据输入的条件参数和宏指令ID列表，检查是否满足删除条件，
        并调用deleteMacro函数进行删除操作。删除条件包括宏指令的类型和
        是否允许删除等。支持删除所有宏指令或指定的宏指令。
        如果bit4（即type4参数）为1，则无条件删除所有指定的宏指令，不考虑其他条件。
        如果bit5（即type5参数）为0，则只能删除一个宏指令，且n必须等于1。
        如果bit5为1，则删除的宏指令数量（n）必须大于1，并且与宏指令ID的个数相同。
        如果需要删除全部宏指令（即n为0），则调用initializeMacroInstructionTable函数来初始化宏指令区域。
        如果bit3（即type3参数）为0，则在尝试删除宏指令时，如果该宏指令是静态宏，不执行删除操作（即删除失败）。
        如果bit2为0，则只有在宏指令允许删除的情况下才进行删除。
        如果bit2为1，则无条件删除，不考虑宏指令的允许删除状态。
调用方法：
        uint8_t result = deleteMacros(type1, type2, type3, type4, type5, n, macroIds);
        // 其中type1至type5为范围类型的5个参数，n为宏指令数量，macroIds为宏指令ID数组
编写日期：2023-11-18
修改记录：
********************************************************************/
uint8_t deleteMacros(uint8_t type1, uint8_t type2, uint8_t type3, uint8_t type4, uint8_t type5, uint8_t n, uint8_t* macroIds) {
    uint8_t successCount = 0;
    uint8_t failCount = 0;

    // 检查约束条件
    if ((type5 == 0 && n != 1) || (type5 == 1 && (n == 0 || n == 1))) {
        return 3; // 输入约束不符合要求
    }

    // 如果bit4为1，则无条件删除
    if (type4 == 1) {
        if (n == 0) {
            initializeMacroInstructionTable();
            return 1; // 删除全部宏指令
        } else {
            for (uint8_t i = 0; i < n; i++) {
                if (deleteMacro(macroIds[i])) {
                    successCount++;
                } else {
                    failCount++;
                }
            }
            return failCount == 0 ? 1 : (successCount == 0 ? 0 : 2); // 删除指定的宏指令
        }
    }

    // 按条件删除宏指令
    for (uint8_t i = 0; i < n; i++) {
        uint16_t macroId = macroIds[i];
        uint8_t macroType = g_macroInstructionTable[macroId][1]; // 获取宏指令类型
        uint8_t isStatic = (macroType >> 5) & 0x01; // 检查是否是静态宏
        uint8_t canDelete = (macroType >> 1) & 0x01; // 检查是否允许删除

        if ((type3 == 0 && isStatic) || (type2 == 0 && !canDelete)) {
            failCount++; // 静态宏且不允许删除静态宏 或者 不允许删除宏指令
        } else if (deleteMacro(macroId)) {
            successCount++;
        } else {
            failCount++;
        }
    }

    return failCount == 0 ? 1 : (successCount == 0 ? 0 : 2); // 返回删除结果
}
/********************************************************************
功能：   修改宏指令库中的宏类型。直接修改宏指令表
参数：
        id - 要修改的宏指令的ID。
        newType - 新的宏类型。
返回值：
        1 - 如果成功修改宏指令类型。
        0 - 如果未找到宏指令或宏类型不符合要求。
主要思路：
        首先检查新的宏类型是否符合特定约束条件。
        然后在宏指令表中查找对应ID的宏指令。
        如果找到，则更新其类型；否则返回失败。
调用方法：
        uint8_t result = modifyMacroType(id, newType);
        // 其中id为宏指令ID，newType为新的宏类型
编写日期：2023-11-18
修改记录：
********************************************************************/
// 函数来修改宏指令的类型
uint8_t modifyMacroType(uint8_t id, uint8_t newType) {
    // 检查宏类型是否符合约束条件
    int typeCount = ((newType >> 5) & 1) + ((newType >> 4) & 1) + ((newType >> 3) & 1);
    int lifeCount = ((newType >> 2) & 1) + ((newType >> 1) & 1) + ((newType >> 0) & 1);

    if (typeCount != 1 || lifeCount != 1) {
        return 0; // 宏类型不符合要求
    }

    // 遍历宏指令表以查找和修改宏指令
    for (uint16_t i = 0; i < g_MacroInstructionCount; i++) {
        if (g_macroInstructionTable[i][0] == id) {
            g_macroInstructionTable[i][1] = newType; // 更新宏指令的类型
            return 1; // 成功修改
        }
    }

    return 0; // 未找到对应ID的宏指令
}
/********************************************************************
功能：   修改宏指令库中的宏指令是否允许释放。
参数：
        id - 要修改的宏指令的ID。
        allowRelease - 是否允许释放的标志（0表示禁止，1表示允许）。
返回值：
        1 - 如果成功修改宏指令的释放属性。
        0 - 如果未找到宏指令或输入的标志不合法。
主要思路：
        遍历宏指令表以查找对应ID的宏指令。
        如果找到，则修改该宏指令的“是否允许释放”比特位。
        根据allowRelease参数设置相应的比特位。
调用方法：
        uint8_t result = modifyMacroRelease(id, allowRelease);
        // 其中id为宏指令ID，allowRelease为是否允许释放的标志
编写日期：2023-11-18
修改记录：
********************************************************************/
uint8_t modifyMacroRelease(uint8_t id, uint8_t allowRelease) {
    if (allowRelease > 1) {
        return 0; // 输入的标志不合法
    }

    // 遍历宏指令表以查找和修改宏指令
    for (uint16_t i = 0; i < g_MacroInstructionCount; i++) {
        if (g_macroInstructionTable[i][0] == id) {
            // 修改是否允许释放的比特位
            if (allowRelease) {
                g_macroInstructionTable[i][1] |= 0x10000000; // 设置允许释放
            } else {
                g_macroInstructionTable[i][1] &= ~0x10000000; // 设置禁止释放
            }
            return 1; // 成功修改
        }
    }

    return 0; // 未找到对应ID的宏指令
}
/********************************************************************
功能：
        批量修改宏指令的允许释放属性。根据给定的范围类型和宏指令ID列表，
        修改对应宏指令的“允许释放”属性。

参数：
        n - 宏指令ID的数量。
        ids - 指向宏指令ID数组的指针。
        rangeType - 定义要修改的宏指令表的范围类型。
            00：临时宏表中的宏ID。
            01：宏序列表中的宏ID。
            10：宏数据表中的宏ID。
            11：宏程序表中的宏ID。

返回值：
        1 - 如果所有指定的宏指令成功修改。
        0 - 如果遇到无效的范围类型或任何修改失败。

主要思路：
        首先根据范围类型选择对应的宏指令表。然后遍历提供的宏指令ID数组，
        并对每个ID调用 modifyMacroRelease 函数以修改其“允许释放”属性。
        如果范围类型无效或任何修改操作失败，则函数返回0。如果所有修改都
        成功，函数返回1。

调用方法：
        uint8_t result = batchModifyMacroRelease(n, ids, rangeType);
        // 其中 n 为宏指令ID数量，ids 为指向ID数组的指针，rangeType 为范围类型

编写日期：2023-11-18
修改记录：
注意：当前暂时没有实现切换table的功能
********************************************************************/
uint8_t batchModifyMacroRelease(uint8_t n, uint8_t *ids, uint8_t rangeType) {
    uint8_t *targetTable;

    // 根据范围类型选择对应的宏指令表
    switch (rangeType) {
        case 0x00:
            targetTable = g_temporaryMacroTable;
            break;
        case 0x01:
            targetTable = g_macroInstructionTable;
            break;
        case 0x10:
            targetTable = g_macroDataTable;
            break;
        case 0x11:
            targetTable = g_macroProgramTable;
            break;
        default:
            return 0; // 无效的范围类型
    }

    // 遍历ID并修改宏指令的允许释放属性
    for (uint8_t i = 0; i < n; i++) {
        if (!modifyMacroRelease(ids[i], 1)) { // 将允许释放设置为1
            return 0; // 修改失败
        }
    }

    return 1; // 成功完成所有修改
}
/********************************************************************
功能：
        批量修改宏指令的禁止释放属性。根据给定的范围类型和宏指令ID列表，
        修改对应宏指令的“禁止释放”属性。

参数：
        n - 宏指令ID的数量。
        ids - 指向宏指令ID数组的指针。
        rangeType - 定义要修改的宏指令表的范围类型。
            00：临时宏表中的宏ID。
            01：宏序列表中的宏ID。
            10：宏数据表中的宏ID。
            11：宏程序表中的宏ID。

返回值：
        1 - 如果所有指定的宏指令成功修改。
        0 - 如果遇到无效的范围类型或任何修改失败。

主要思路：
        首先根据范围类型选择对应的宏指令表。然后遍历提供的宏指令ID数组，
        并对每个ID调用 modifyMacroRelease 函数以修改其“禁止释放”属性。
        如果范围类型无效或任何修改操作失败，则函数返回0。如果所有修改都
        成功，函数返回1。

调用方法：
        uint8_t result = batchModifyMacroDisallowRelease(n, ids, rangeType);
        // 其中 n 为宏指令ID数量，ids 为指向ID数组的指针，rangeType 为范围类型

编写日期：2023-11-18
修改记录：
注意：当前暂时没有实现切换table的功能
********************************************************************/
uint8_t batchModifyMacroDisallowRelease(uint8_t n, uint8_t *ids, uint8_t rangeType) {
    uint8_t *targetTable;

    // 根据范围类型选择对应的宏指令表
    switch (rangeType) {
        case 0x00:
            targetTable = g_temporaryMacroTable;
            break;
        case 0x01:
            targetTable = g_macroInstructionTable;
            break;
        case 0x10:
            targetTable = g_macroDataTable;
            break;
        case 0x11:
            targetTable = g_macroProgramTable;
            break;
        default:
            return 0; // 无效的范围类型
    }

    // 遍历ID并修改宏指令的禁止释放属性
    for (uint8_t i = 0; i < n; i++) {
        if (!modifyMacroRelease(ids[i], 0)) { // 将禁止释放设置为0
            return 0; // 修改失败
        }
    }

    return 1; // 成功完成所有修改
}
/********************************************************************
功能：   修改宏指令库中的宏指令是否允许删除。
参数：
        id - 要修改的宏指令的ID。
        allowRelease - 是否允许删除的标志（0表示禁止，1表示允许）。
返回值：
        1 - 如果成功修改宏指令的删除属性。
        0 - 如果未找到宏指令或输入的标志不合法。
主要思路：
        遍历宏指令表以查找对应ID的宏指令。
        如果找到，则修改该宏指令的“是否允许删除”比特位。
        根据allowRelease参数设置相应的比特位。
调用方法：
        uint8_t result = modifyMacroRelease(id, allowRelease);
        // 其中id为宏指令ID，allowRelease为是否允许删除的标志
编写日期：2023-11-18
修改记录：
********************************************************************/
uint8_t modifyMacroDelete(uint8_t id, uint8_t allowRelease) {
    if (allowRelease > 1) {
        return 0; // 输入的标志不合法
    }

    // 遍历宏指令表以查找和修改宏指令
    for (uint16_t i = 0; i < g_MacroInstructionCount; i++) {
        if (g_macroInstructionTable[i][0] == id) {
            // 修改是否允许删除的比特位
            if (allowRelease) {
                g_macroInstructionTable[i][1] |= 0x01000000; // 设置允许删除
            } else {
                g_macroInstructionTable[i][1] &= ~0x01000000; // 设置禁止删除
            }
            return 1; // 成功修改
        }
    }

    return 0; // 未找到对应ID的宏指令
}
/********************************************************************
功能：
        批量修改宏指令的允许删除属性。根据给定的范围类型和宏指令ID列表，
        修改对应宏指令的“允许删除”属性。

参数：
        n - 宏指令ID的数量。
        ids - 指向宏指令ID数组的指针。
        rangeType - 定义要修改的宏指令表的范围类型。
            00：临时宏表中的宏ID。
            01：宏序列表中的宏ID。
            10：宏数据表中的宏ID。
            11：宏程序表中的宏ID。

返回值：
        1 - 如果所有指定的宏指令成功修改。
        0 - 如果遇到无效的范围类型或任何修改失败。

主要思路：
        首先根据范围类型选择对应的宏指令表。然后遍历提供的宏指令ID数组，
        并对每个ID调用 modifyMacroRelease 函数以修改其“允许删除”属性。
        如果范围类型无效或任何修改操作失败，则函数返回0。如果所有修改都
        成功，函数返回1。

调用方法：
        uint8_t result = batchModifyMacroRelease(n, ids, rangeType);
        // 其中 n 为宏指令ID数量，ids 为指向ID数组的指针，rangeType 为范围类型

编写日期：2023-11-18
修改记录：
注意：当前暂时没有实现切换table的功能
********************************************************************/
uint8_t batchDeleteMacroRelease(uint8_t n, uint8_t *ids, uint8_t rangeType) {
    uint8_t *targetTable;

    // 根据范围类型选择对应的宏指令表
    switch (rangeType) {
        case 0x00:
            targetTable = g_temporaryMacroTable;
            break;
        case 0x01:
            targetTable = g_macroInstructionTable;
            break;
        case 0x10:
            targetTable = g_macroDataTable;
            break;
        case 0x11:
            targetTable = g_macroProgramTable;
            break;
        default:
            return 0; // 无效的范围类型
    }

    // 遍历ID并修改宏指令的允许删除属性
    for (uint8_t i = 0; i < n; i++) {
        if (!modifyMacroDelete(ids[i], 1)) { // 将允许删除设置为1
            return 0; // 修改失败
        }
    }

    return 1; // 成功完成所有修改
}
/********************************************************************
功能：
        批量修改宏指令的禁止删除属性。根据给定的范围类型和宏指令ID列表，
        修改对应宏指令的“禁止删除”属性。

参数：
        n - 宏指令ID的数量。
        ids - 指向宏指令ID数组的指针。
        rangeType - 定义要修改的宏指令表的范围类型。
            00：临时宏表中的宏ID。
            01：宏序列表中的宏ID。
            10：宏数据表中的宏ID。
            11：宏程序表中的宏ID。

返回值：
        1 - 如果所有指定的宏指令成功修改。
        0 - 如果遇到无效的范围类型或任何修改失败。

主要思路：
        首先根据范围类型选择对应的宏指令表。然后遍历提供的宏指令ID数组，
        并对每个ID调用 modifyMacroRelease 函数以修改其“禁止删除”属性。
        如果范围类型无效或任何修改操作失败，则函数返回0。如果所有修改都
        成功，函数返回1。

调用方法：
        uint8_t result = batchModifyMacroDisallowRelease(n, ids, rangeType);
        // 其中 n 为宏指令ID数量，ids 为指向ID数组的指针，rangeType 为范围类型

编写日期：2023-11-18
修改记录：
注意：当前暂时没有实现切换table的功能
********************************************************************/
uint8_t batchDeleteMacroDisallowRelease(uint8_t n, uint8_t *ids, uint8_t rangeType) {
    uint8_t *targetTable;

    // 根据范围类型选择对应的宏指令表
    switch (rangeType) {
        case 0x00:
            targetTable = g_temporaryMacroTable;
            break;
        case 0x01:
            targetTable = g_macroInstructionTable;
            break;
        case 0x10:
            targetTable = g_macroDataTable;
            break;
        case 0x11:
            targetTable = g_macroProgramTable;
            break;
        default:
            return 0; // 无效的范围类型
    }

    // 遍历ID并修改宏指令的禁止删除属性
    for (uint8_t i = 0; i < n; i++) {
        if (!modifyMacroDelete(ids[i], 0)) { // 将禁止删除设置为0
            return 0; // 修改失败
        }
    }

    return 1; // 成功完成所有修改
}

/********************************************************************
功能：
        批量修改宏指令的类型。根据给定的宏指令ID列表和宏类型，
        更新对应宏指令的类型。

参数：
        n - 宏指令ID的数量。
        ids - 指向宏指令ID数组的指针。
        types - 指向宏类型数组的指针，每个宏类型为8bit。

返回值：
        1 - 如果所有指定的宏指令类型成功更新。
        0 - 如果任何修改失败。

主要思路：
        遍历提供的宏指令ID数组，对每个ID调用 modifyMacroType 函数，
        以更新其宏类型。如果任何更新操作失败，则函数返回0。如果所有
        更新都成功，函数返回1。

调用方法：
        uint8_t result = batchModifyMacroTypes(n, ids, types);
        // 其中 n 为宏指令ID数量，ids 为指向ID数组的指针，types 为指向宏类型数组的指针

编写日期：2023-11-18
修改记录：
********************************************************************/
uint8_t batchModifyMacroTypes(uint8_t n, uint8_t *ids, uint8_t *types) {
    for (uint8_t i = 0; i < n; i++) {
        if (!modifyMacroType(ids[i], types[i])) { // 更新宏指令的类型
            return 0; // 修改失败
        }
    }
    return 1; // 成功完成所有修改
}










