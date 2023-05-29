//
// Created by kfzjw008 on 2023-05-16.
//

#include "dataInjection.h"
#include "../global.h"
#include <stdint.h>
#include <string.h>
#include <malloc.h>
//双星计划 格非 萤火一号博士论文 aos高级在轨系统
//注入包接收
/***********************************************************
程序名称： injectionPacketReception(uint8_t data[],uint8_t size)
功能描述：数据注入包接收
输入参数：uint8_t data[],uint8_t size
输出参数：无
说    明：
***********************************************************/
void injectionPacketReception(uint8_t data[], uint16_t size) {
    if (size == 0)size = INJECTDATA_SIZE;
    g_ingectSize = size;//默认INJECTDATA_SIZE字节
    uint16_t i = 0;
    uint16_t startIndex = 0;
    uint16_t endIndex = 0;
    while (i < size) {
        // 调用 packetIdentification 函数来检查是否可以构成完整的包
        uint16_t packetIndex = packetIdentification(&data[i], size - i);
        if (packetIndex == 0) {
            // 不可以构成完整的包，退出循环
            break;
        }
        // 计算注入包的起始和结束索引
        startIndex = i;
        endIndex = i + packetIndex - 1;

        // 将注入包复制到 g_InjectionPacketBuffer
        memcpy(g_InjectionPacketBuffer[g_InjectionPacketBufferNum], &data[startIndex], packetIndex);

        // 更新注入包计数
        g_InjectionPacketBufferNum++;
        // 移动到下一个注入包的起始位置
        i = endIndex + 1;
    }
}


/***********************************************************
程序名称：packetIdentification
功能描述：注入包识别
输入参数：uint8_t* data, uint16_t size
输出参数：0:不是包  数字:从0字节到数字字节构成一个包
说    明：none
***********************************************************/
uint16_t packetIdentification(uint8_t *data, uint16_t size) {
    // 检查数据长度是否足够
    if (size < 6) {
        return 0;  // 数据长度不足，无法构成完整的包
    }

    // 解析包版本号
    uint8_t version = (data[0] >> 5 & 0x07);
    // 解析包类型
    uint8_t packetType = (data[0] >> 3) & 0x01;
    // 解析副导头标志
    uint8_t secondaryHeaderFlag = (data[0] >> 4) & 0x01;
    // 解析应用过程标识符
    uint16_t applicationProcessID = ((data[0] & 0x07) << 8) | data[1];
    //解析序列标志
    uint8_t sequenceFlag = (data[2] >> 6 & 0x03);
    // 解析包序列计数
    uint16_t sequenceCount = ((data[2] & 0x3F) << 8) | data[3];
    // 解析包数据长度
    uint16_t Psize = ((data[4]) << 8) | data[5];

    //注入包没有副导头，不需要解析

    if (version == 0 && secondaryHeaderFlag == 0 && packetType == 1 && applicationProcessID == 0b0001001100110100 &&
        Psize <= INJECTDATA_SIZE)
        return Psize;  // 构成一个完整的包，返回包长度
    else return 0;
}


/***********************************************************
程序名称：
功能描述： 注入包生成
输入参数：uint8_t verison,uint8_t packetType,uint8_t flag,uint16_t APID,uint8_t sequenceFlag,uint16_t packetName ,uint16_t size,uint8_t packetPreamble,uint8_t* data
输出参数：注入包
说    明：
***********************************************************/
uint16_t *injectPacketGeneration(uint8_t verison, uint8_t packetType, uint8_t flag, uint16_t APID, uint8_t sequenceFlag,
                                 uint16_t packetName, uint16_t size, uint8_t packetPreamble, uint8_t *data) {

    uint16_t *array = (uint16_t *) malloc(sizeof(uint8_t) * size + 6);//6为包结构前面固定的6个字节格式
    array[0] = (verison << 13) + (packetType << 12) + (flag << 11) + APID;
    array[1] = (sequenceFlag << 14) + packetName;
    array[2] = size;
    memcpy((array + 3), data, size);
    return array;
}


/***********************************************************
程序名称：
功能描述： 指令包生成
输入参数：uint8_t verison,uint8_t packetType,uint8_t flag,uint16_t APID,uint8_t sequenceFlag,uint16_t packetName ,uint16_t size,uint8_t packetPreamble,uint8_t* data
输出参数：指令包
说    明：
***********************************************************/
uint16_t *
instructionPacketGeneration(uint8_t verison, uint8_t packetType, uint8_t flag, uint16_t APID, uint8_t sequenceFlag,
                            uint16_t packetName, uint16_t size, uint32_t timer, uint32_t packetPreamble,
                            uint8_t *data) {

    uint16_t *array = (uint16_t *) malloc(sizeof(uint8_t) * size + 6);//6为包结构前面固定的6个字节格式
    array[0] = (verison << 13) + (packetType << 12) + (flag << 11) + APID;
    array[1] = (sequenceFlag << 14) + packetName;
    array[2] = size;
    array[3] = timer >> 16;
    array[4] = timer;
    array[5] = packetPreamble;
    memcpy((array + 6), data, size);
    return array;
}

/***********************************************************
程序名称：
功能描述： 注入包检查
输入参数：
输出参数：指令包
说    明：对缓冲区的数据展开相关检查
 #define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*注入包缓冲区起始地址*/
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*注入包缓冲区结束地址*/
***********************************************************/
void injectionPacketInspection(){
//g_InjectionPacketBuffer
    uint32_t n_ui32VarT1MIEn = 0; /*4字节变量-遍历参数监控表for循环*/
    for(n_ui32VarT1MIEn = ((uint32_t)(MONI_ZRB_STR_ADRS)); n_ui32VarT1MIEn < ((uint32_t)(MONI_ZRB_END_ADRS)); ){
//n_ui16VarT1MIEn = inpDw(n_ui32VarT1MIEn) & 0x0000FFFF



        n_ui32VarT1MIEn = n_ui32VarT1MIEn +MAX_INJECT_PKG_LEN;
    }
}