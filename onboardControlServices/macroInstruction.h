//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_MACROINSTRUCTION_H
#define SICSA_MACROINSTRUCTION_H


#include <stdint.h>
void fmacroInstructionInit();
void fmacroInstructionMain();
void fmacroInstructionEnd(); // 清理宏指令模块资源
uint8_t addMacroInstruction(uint8_t id, uint8_t type, uint8_t checksum, uint16_t length, uint8_t *content);
void initializeMacroInstructionTable();
uint8_t addMacro(uint8_t id, uint8_t release, uint8_t delete, uint8_t sequence, uint8_t data, uint8_t program,
                 uint8_t staticMacro, uint8_t dynamicMacro, uint8_t temporaryMacro, uint8_t *content, uint16_t length) ;
uint8_t deleteMacro(uint16_t macroId);
uint8_t deleteMacros(uint8_t type1, uint8_t type2, uint8_t type3, uint8_t type4, uint8_t type5, uint8_t n, uint8_t* macroIds);
uint8_t modifyMacroType(uint8_t id, uint8_t newType);
uint8_t modifyMacroRelease(uint8_t id, uint8_t allowRelease);
uint8_t batchModifyMacroRelease(uint8_t n, uint8_t *ids, uint8_t rangeType);
uint8_t batchModifyMacroDisallowRelease(uint8_t n, uint8_t *ids, uint8_t rangeType);
uint8_t modifyMacroDelete(uint8_t id, uint8_t allowRelease);
uint8_t batchDeleteMacroRelease(uint8_t n, uint8_t *ids, uint8_t rangeType);
uint8_t batchDeleteMacroDisallowRelease(uint8_t n, uint8_t *ids, uint8_t rangeType);









#endif //SICSA_MACROINSTRUCTION_H
