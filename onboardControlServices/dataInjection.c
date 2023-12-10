//
// Created by kfzjw008 on 2023-05-16.
//

#include "dataInjection.h"

//双星计划 格非 萤火一号博士论文 aos高级在轨系统
/********************************************************************
功能：接收并处理注入数据包，将其放入注入包缓冲区
参数：
      data[] - 接收到的数据包数组
      size - 数据包数组的大小
返回值：无
主要思路：
      遍历数据包数组，使用 packetIdentification 函数检查数据包是否可以构成完整的包。
      如果可以，则将该包复制到注入包缓冲区。如果不可以，或数据包过小，不执行任何操作。
调用方法：
      injectionPacketReception(receivedData, dataSize);
      其中 receivedData 是接收到的数据包数组，dataSize 是数组的大小。
编写日期：2023-12-10
修改记录：无
注意：假定注入包长度至少为 6 个字节。
********************************************************************/
void injectionPacketReception(uint8_t data[], uint8_t size) {

    //data是一个数据包，一个数据包包含多个指令包
    if(size < 6){
        return ;
    }
    g_ingectSize = size;//注入数据长度

    uint8_t i; //循环变量
    uint8_t start,end;

    while (i < size) {
        // 调用 packetIdentification 函数来检查是否可以构成完整的包
        uint16_t packetIndex = packetIdentification(&data[i], size - i);

        if (packetIndex == 0) {
            // 不可以构成完整的包，退出循环
            break;
        }

        // 到此，识别到一个完整的包，从i开始，长度为packetIndex

        // 计算注入包的起始和结束索引
        start = i;
        end= i + packetIndex - 1;

        // 将注入包复制到 g_InjectionPacketBuffer
        memcpy(g_InjectionPacketBuffer[(g_InjectionPacketBufferNum ++) % MAX_INJECT_BUFFER_NUM], &data[start], packetIndex);
        printf("注入包已放到缓冲区~\n");
        printf("%d",g_InjectionPacketBufferNum);
        // 移动到下一个注入包的起始位置
        i = end + 1;
    }


    return;
}



/********************************************************************
功能：从临时注入包缓冲区中读取注入包并拼接成一个完整的注入包
参数：无
返回值：拼接后的注入包总大小（Psize）
主要思路：
      遍历临时注入包缓冲区 g_InjectionTempPacketBuffer 中的所有注入包。
      将每个注入包的内容拼接到结果数组 result 中，并更新拼接后的总大小 Psize。
      在拼接过程中，还会设置拼接包的头部信息。
调用方法：uint16_t totalSize = packetSplicing();
编写日期：2023-12-10
修改记录：无
注意：假设 result 数组足够大以容纳拼接后的所有数据。拼接完成后，临时注入包缓冲区将被清空。
********************************************************************/
uint16_t packetSplicing(){
    //从g_InjectionTempPacketBuffer里读出注入包拼接，然后清空g_InjectionTempPacketBuffer
    uint8_t Psize = 0;
    uint8_t size = 0;
    uint8_t result[50];
    uint8_t resultPointer = 6;
    for(int i = 0; i < g_InjectionTempPacketBufferPointer; i ++){
        uint8_t t = g_InjectionTempPacketBuffer[i]; //是一个数组
        if(!i){
            result[0] = t[0];
            result[1] = t[1];
            result[3] = t[2] | 0b11000000;
            result[4] = t[3];
        }
        size = ((t[4]) << 8) | t[5];
        Psize += size;
        for(uint8_t i = 0; i < size; i ++){
            result[resultPointer++] = t[i + 6];
        }
    }
    result[5-6] = size;

    //清空
    g_InjectionTempPacketBufferPointer = 0;

    return result,Psize;
}


/********************************************************************
功能：
        识别并验证接收到的数据是否构成一个完整的注入包。

参数：
        data - 指向接收到的数据的指针。
        size - 接收到的数据的长度。

返回值：
        如果成功识别出一个完整的注入包，则返回注入包的长度。
        如果数据不构成一个完整的包或验证失败，则返回 0。

主要思路：
        首先检查接收到的数据长度是否足够。
        然后逐步解析包的不同部分，如版本号、包类型、副导头标志、
        应用过程标识符、序列标志和包序列计数。
        根据序列标志判断包的类型（首包、中间包、尾包或独立包）。
        如果识别出首包或中间包，则存储到临时缓冲区并返回 0。
        如果是尾包，则调用 packetSplicing() 函数拼接包并返回完整包的长度。
        如果是独立包，则直接返回包的长度。

调用方法：
        uint16_t packetLength = packetIdentification(data, size);

编写日期：
        2023-12-10

修改记录：
        无

注意：
        函数假设注入包没有副导头，只处理有限的包类型。
        临时缓冲区 g_InjectionTempPacketBuffer 用于存储首包和中间包，
        直到识别出尾包或独立包。
********************************************************************/
uint16_t packetIdentification(uint8_t *data, uint16_t size) {
    // 检查数据长度是否足够
    if (size < 6) {
        return 0;  // 数据长度不足，无法构成完整的包
    }

    // 解析包版本号
    uint8_t version = (data[0] >> 5 & 0b0111);//第1字节前3位
    // 解析包类型
    uint8_t packetType = (data[0] >> 4) & 0b0001;
    // 解析副导头标志
    uint8_t secondaryHeaderFlag = (data[0] >> 3) & 0b0001;
    // 解析应用过程标识符
    uint16_t applicationProcessID = ((data[0] & 0b0111) << 8) | data[1]; //左移8位是给data[1]腾位置。
    //解析序列标志
    uint8_t sequenceFlag = (data[2] >> 6 & 0x03); //第3字节前2位
    // 解析包序列计数
    uint16_t sequenceCount = ((data[2] & 0x3F) << 8) | data[3];//第3字节后6位 + 第4字节前8位
    // 解析包数据长度
    uint16_t Psize = ((data[4]) << 8) | data[5];//第5字节 + 第6字节

    //注入包没有副导头，不需要解析
    // printf("%d\n",sequenceFlag );
    // printf("%d\n",sequenceFlag & 0x01);

    if (version == 0 && secondaryHeaderFlag == 0 && packetType == 1 && applicationProcessID == 0b01100110100 &&
        Psize <= INJECTDATA_SIZE){
        if(sequenceFlag  == 0x01) {
            // 为首包
            g_InjectionPacketFullFlag = 1;
            //存入临时缓冲区
            *g_InjectionTempPacketBuffer[g_InjectionTempPacketBufferPointer++] = *data;
            printf("该包为首包！");
            return 0;
        }else if(sequenceFlag  == 0x00){
            // 中间包
            printf("该包为中间包！");
            //存入临时缓冲区
            *g_InjectionTempPacketBuffer[g_InjectionTempPacketBufferPointer++] = *data;
            return 0;
        }
        else if(sequenceFlag  == 0x02){
            printf("该包为尾包！");
            //末尾包
            g_InjectionPacketFullFlag = 0;
            //存入临时缓冲区
            *g_InjectionTempPacketBuffer[g_InjectionTempPacketBufferPointer++] = *data;
            Psize = packetSplicing();
            return Psize;  // 出现尾包，构成一个完整的包，返回包长度
        }
        else if(sequenceFlag  == 0x03){
            printf("该包为独立包！");
            //独立包
            return Psize;  // 构成一个完整的包，返回包长度
        }else return 0;
    }

    else {
        // printf("%d",sequenceFlag);
        return 0;
    }
}


/********************************************************************
功能：
        判断给定的两个字节是否构成指令包的开头。
参数：
        data1 - 第一个字节的数据。
        data2 - 第二个字节的数据。
返回值：
        如果这两个字节匹配指令包的开头标识，则返回 1。
        否则返回 0。
主要思路：
        通过比较给定的两个字节与指令包固定开始的标识（00011000 10110100），
        确定这两个字节是否构成指令包的开头。
调用方法：
        uint8_t result = isInstructionPacket(data1, data2);
        // 其中 data1 和 data2 是需要判断的字节数据
编写日期：
        2023-12-10
修改记录：
        无
注意：
        该函数仅通过两个字节的比较来判断，不涉及更复杂的数据包结构分析。
********************************************************************/
uint8_t isInstructionPacket(uint8_t data1,uint8_t data2){
    if(data1 == 0b00011000 && data2 == 0b10110100){
        return 1;
    }
    else return 0;
}

/********************************************************************
功能：
        解包注入包，提取其中的指令序列。
参数：
        data - 指向注入包的指针。
返回值：
        无。函数通过操作全局变量来存储提取的指令序列。
主要思路：
        遍历注入包数据，使用 isInstructionPacket 函数检测指令包的开始。
        一旦检测到新指令的头部，将该指令复制到指令缓冲区 g_instructionPacketBuffer 中。
调用方法：
        injectionPacketUnpacking(data);
        // 其中 data 是指向注入包的指针
编写日期：
        2023-12-10
修改记录：
        无
注意：
        该函数假定注入包数据的结构是连续的指令序列，且每个指令包的开头都符合特定格式。
********************************************************************/
void injectionPacketUnpacking(uint8_t data[]){
    uint8_t Psize = (data[4] << 8 )|data[5]; //数据域（指令）字节数
    uint8_t  head = 0,tail = 0;
    for(uint8_t i = 0;i < Psize - 1; i ++){
        if(isInstructionPacket(data[i],data[i + 1])){//判定为新指令头，将把新指令存起来
            if(i - tail > 0){ //等于0时是第一个。

                tail = i -1;
                //存指令 下标为：head - tail;
                // 将注入包复制到 g_instructionPacket
                memcpy(g_instructionPacketBuffer[g_instructionPacketBufferNum ++], &data[head], tail - head + 1);
            }
            head = i;
        }
    }

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

/********************************************************************
功能：
        检查注入包的正确性，确保其符合预定的数据包规范。
参数：
        item - 指向注入包的指针。
返回值：
        0 - 如果注入包符合规范。
        1 - 如果注入包不符合规范。
主要思路：
        对注入包的头部数据进行解析，包括版本号、包类型、副导头标志、应用过程标识符和包序列控制。
        检查每个部分是否符合预定的格式和值，如果有任何部分不符合规范，则返回1表示错误。
调用方法：
        int result = packageValidity(item);
        // 其中 item 指向注入包的指针
编写日期：
        2023-12-10
修改记录：
        无
注意：
        函数假设 item 指针指向一个有效的注入包，并且包含足够的数据进行头部信息的解析。
********************************************************************/
int packageValidity(uint8_t *item){
//符合规范为0,不符合规范返回1
// 到 注入包缓冲区 拿注入包
    printf("开始包正确性检测~\n");
    printf("item:%d\n",*item);
    uint16_t packageSymbol = inpDw(*item) & 0xFFFF; //前两字节为包标识，一共16位
    printf("packageSymbol：%d\n",packageSymbol);
    uint16_t packageSequenceControl = inpDw(*item + 2)& 0xFFFF; // 中间两字节为包序列控制
    uint16_t packageDataLen = inpDw(*item + 4)& 0xFFFF; //数据域

    // 版本号错误：1
    uint8_t version = packageSymbol >> 13; // 右移13位则剩下三位为包版本
    if(version != 0b000){
        return 0;
    }
    // 包类型错误：2
    uint8_t packetType = (packageSymbol >> 12 ) & 0b0001;

    // 副导头标志错误：3
    uint8_t secondaryHeaderFlag = (packageSymbol >> 11 )& 0b00001;

    // 应用过程标识符错误：4  有效载荷系统注入包必须为 0b01100110100
    uint8_t applicationProcessID = packageSymbol  & 0b0000011111111111;

    //解析序列标志
    uint8_t sequenceFlag = packageSequenceControl >> 14;
    // 解析包序列计数
    uint16_t sequenceCount = packageSequenceControl & 0b0011111111111111;
    // 解析包数据长度
    uint16_t Psize = packageDataLen & 0b1111111111111111;

    if (version != 0b000) {
        return 1;
    }
    if (packetType != 0b1) {
        return 1;
    }
    if (secondaryHeaderFlag != 0b0) {
        return 1;
    }
    if (applicationProcessID != 0b0001001100110100) {
        return 1;
    }
    if (sequenceFlag >= 4) {
        return 1;
    }

    return 0;
}

/********************************************************************
功能：
        检查注入包序列的唯一性，确保每个注入包具有唯一的包序列计数。
参数：
        无
返回值：
        无
主要思路：
        遍历注入包缓冲区中的所有包，解析每个包的序列计数，并检查是否存在重复的序列计数。
        如果发现重复，标记相应的包为非唯一（通过g_InjectionPacketBufferCheckFlag数组）。
调用方法：
        packageUniqueness();
编写日期：
        2023-12-10
修改记录：
        无
注意：
        函数假设全局变量g_InjectionPacketBufferNum正确表示缓冲区中的包数量。
        g_InjectionPacketBufferCheckFlag数组用于记录检查结果。
********************************************************************/
int packageUniqueness() {
    uint16_t sequenceCounts[MAX_INJECT_BUFFER_NUM];
    int i = 0;
    // 遍历缓冲区所有的包序号
    uint32_t item;
    for (item = ((uint32_t) (MONI_ZRB_STR_ADRS)); item < ((uint32_t) (MONI_ZRB_END_ADRS));item += MAX_INJECT_PKG_LEN) {
        if (i == g_InjectionPacketBufferNum) {
            // 读到最后一个包则停止
            break;
        }
        uint16_t n_ui16VarT1MIEn1 = inpDw(item + 0) & 0xFFFF;
        uint16_t n_ui16VarT1MIEn2 = inpDw(item + 2) & 0xFFFF;
        uint16_t n_ui16VarT1MIEn3 = inpDw(item + 4) & 0xFFFF;
        // 解析包版本号
        uint8_t version = n_ui16VarT1MIEn1 >> 13;
        // 解析包类型
        uint8_t packetType = (n_ui16VarT1MIEn1 >> 12) & 0b0001;
        // 解析副导头标志
        uint8_t secondaryHeaderFlag = (n_ui16VarT1MIEn1 >> 11) & 0b00001;
        // 解析应用过程标识符
        uint16_t applicationProcessID = n_ui16VarT1MIEn1 & 0b0000011111111111;
        //解析序列标志
        uint8_t sequenceFlag = n_ui16VarT1MIEn2 >> 14;
        // 解析包序列计数
        uint16_t sequenceCount = n_ui16VarT1MIEn2 & 0b0011111111111111;
        // 解析包数据长度
        uint16_t Psize = n_ui16VarT1MIEn3 & 0b1111111111111111;


        for (int j = 0; j < g_InjectionPacketBufferNum; j++) {
            if (sequenceCounts[j] == sequenceCount && j != i && j < i) {
                g_InjectionPacketBufferCheckFlag[j] = 2;
            }
        }
        i = i + 1;
    }
}

/********************************************************************
功能：
        检查注入包的合法性，确保包内指令序列的总长度与注入包长度匹配。
参数：
        data - 指向注入包数据的指针。
返回值：
        1 - 如果注入包合法（指令序列总长度与注入包长度匹配）。
        0 - 如果注入包不合法。
主要思路：
        首先使用解包函数处理注入包，提取其中的指令序列。
        然后计算所有指令序列的总长度，并与注入包的总长度进行比较。
        如果两者相等，则注入包被视为合法；否则，视为不合法。
调用方法：
        int result = packageLegitimacy(data);
        // 其中 data 指向注入包数据
编写日期：
        2023-12-10
修改记录：
        无
注意：
        函数依赖于全局变量g_instructionPacketBuffer和g_instructionPacketBufferNum
        以存储和追踪解包后的指令序列。
********************************************************************/
int packageLegitimacy(uint8_t data[]) {
    //调用解包函数
    injectionPacketUnpacking(data);
    uint8_t Psize = (data[4] << 8) | data[5];
    uint8_t Isize = 0;
    for(uint8_t i = 0; i < g_instructionPacketBufferNum; i ++){
        Isize += (g_instructionPacketBuffer[i][4] << 8) | g_instructionPacketBuffer[i][5];
    }
    if(Psize == Isize) return 1;

    return 0;


}

/********************************************************************
功能：
        判断包相关性。当前包与已收到包之间的逻辑关联性，确保包序列的连续性和完整性。
参数：
        item - 当前包的内存地址。
返回值：
        true - 如果当前包与之前包的关联性合理。
        false - 如果当前包与之前包的关联性不合理。
主要思路：
        通过检查包序列控制字段中的序列标志（首包、中间分量、独立分量、末尾分量），
        来判断当前包是否与先前接收的包序列逻辑上连续和完整。
        使用全局标志 g_InjectionPacketFullFlag 来追踪包序列的完整性状态。
调用方法：
        bool relevance = packageRelevance(item);
        // 其中 item 为当前包的内存地址
编写日期：
        2023-12-10
修改记录：
        无
注意：
        函数依赖于全局变量 g_InjectionPacketFullFlag 来维护包序列的完整性状态。
********************************************************************/
uint8_t packageRelevance(uint32_t item){
    //包与此前已收到并通过检查的包之间的包序号和首末分量的逻辑关系，通过“包序控制”域来判别。
    //uint32_t item;
    int i;
    for (item = ((uint32_t) (MONI_ZRB_STR_ADRS)); item < ((uint32_t) (MONI_ZRB_END_ADRS));item += MAX_INJECT_PKG_LEN) {
        if (i == g_InjectionPacketBufferNum) {
            // 读到最后一个包则停止
            break;
        }
        uint16_t packageSequenceControl = inpDw(item + 2); // 中间两字节为包序列控制
        uint8_t seqSymbol = packageSequenceControl >> 14;
        if(seqSymbol == 1){//首包
            if(g_InjectionPacketFullFlag == 1) return 0;
            else g_InjectionPacketFullFlag = 1;
        }else if(seqSymbol== 0) {// 中间分量
            if(g_InjectionPacketFullFlag == 0) return 0;
        } else if(seqSymbol == 0x03){// 独立分量
            if(g_InjectionPacketFullFlag == 1) return 0;
        } else if(seqSymbol == 0x02) { //末尾分量
            if(g_InjectionPacketFullFlag == 0) return 0;
            else g_InjectionPacketFullFlag = 0;
        }
    }

}

/********************************************************************
功能：
        对注入包进行一系列检查，以确保其符合预定的包格式和逻辑关系。
参数：
        size - 注入包的大小。
返回值：
        1 - 如果包通过所有检查。
        0 - 如果包在任何检查中失败。
主要思路：
        依次调用不同的检查函数（如包的有效性、唯一性检查），判断注入包是否
        符合指定的格式和逻辑要求。如果任何一个检查未通过，立即返回0。
调用方法：
        uint8_t result = injectionPacketInspection(size); // 其中 size 为注入包的大小
编写日期：
        2023-12-10
修改记录：
        无
注意：
        此函数假定注入包已经被放入 g_InjectionPacketBuffer 缓冲区中。
********************************************************************/
uint8_t  injectionPacketInspection(int size) {

    printf("%d", *g_InjectionPacketBuffer[g_InjectionPacketBufferNum]); //注入包缓冲区拿数据
    // for(int i = 0; i < size; i ++){
    //     printf("%d\n",g_InjectionPacketBuffer[g_InjectionPacketBufferNum][i]);
    // }
    uint8_t flag;
    flag = packageValidity(g_InjectionPacketBuffer[g_InjectionPacketBufferNum]);
    if (flag == 1) return 0;
    packageUniqueness();
    if (flag == 1) return 0;

    // packageLegitimacy(data);
    // if(flag == 1) return 0;
    // packageRelevance(data);
    // if(flag == 1) return 0;
    return 1;
}

/********************************************************************
功能：
        初始化数据注入模块，设置所有相关数组和变量的初始值。
参数：无
返回值：无
主要思路：
        对所有涉及到的数组和全局变量进行初始化，以确保数据注入模块正常运行。
调用方法：
        fdataInjectionInit(); // 在数据注入模块启动时调用
编写日期：
        2023-12-10
修改记录：
        无
注意：
        确保所有全局变量都被正确初始化。
********************************************************************/
void fdataInjectionInit() {
    g_InjectionPacketBufferNum = 0;
    g_InjectionTempPacketBufferPointer = 0;
    g_instructionPacketBufferNum = 0;
    g_InjectionPacketFullFlag = 0;
    memset(g_InjectionPacketBuffer, 0, sizeof(g_InjectionPacketBuffer));
    memset(g_InjectionTempPacketBuffer, 0, sizeof(g_InjectionTempPacketBuffer));
    memset(g_instructionPacketBuffer, 0, sizeof(g_instructionPacketBuffer));
    memset(g_InjectionPacketBufferCheckFlag, 0, sizeof(g_InjectionPacketBufferCheckFlag));
}


/********************************************************************
功能：        主数据注入处理函数，负责接收和处理注入数据包。
参数：        无（假设 receivedData 和 dataSize 由系统或其他函数提供）
返回值：      无
主要思路：    该函数整合了注入包接收、注入包检查、注入帧完整性检查和注入包解包四个子业务。
              它首先接收注入包，并将其存储在缓冲区中。然后对每个注入包进行一系列检查，
              包括其唯一性、合法性和相关性。如果注入帧完整，则进行注入包解包，提取其中的指令序列。
调用方法：    fdataInjectionMain();
编写日期：    2023-12-10
修改记录：    无
********************************************************************/
void fdataInjectionMain() {
    // 假设 receivedData 和 dataSize 由系统或其他函数提供
    uint8_t receivedData[MAX_INJECT_PKG_LEN];
    uint16_t dataSize;

    // 1. 接收注入包
    injectionPacketReception(receivedData, dataSize);

    // 2. 注入包检查
    for (int i = 0; i < g_InjectionPacketBufferNum; i++) {
        if (injectionPacketInspection(i)) {
            // 如果注入包通过所有检查
            // 3. 注入帧完整性检查
            if (g_InjectionPacketFullFlag) {
                // 注入帧完整
                // 4. 注入包解包
                injectionPacketUnpacking(g_InjectionPacketBuffer[i]);
            }
        }
    }

    // 对解包后的指令进行后续处理
    // 这部分逻辑取决于系统的具体要求
}


/********************************************************************
功能：
        清理数据注入模块资源。
参数：无
返回值：无
主要思路：
        释放数据注入模块占用的所有资源，如动态分配的内存。
调用方法：
        fdataInjectionEnd(); // 在数据注入模块结束时调用
编写日期：
        2023-12-10
修改记录：
        无
注意：
        确保释放所有已分配的资源，避免内存泄露。
********************************************************************/
// Cleanup Function
void fdataInjectionEnd() {
    g_InjectionPacketBufferNum = 0;
    g_InjectionTempPacketBufferPointer = 0;
    g_instructionPacketBufferNum = 0;
    g_InjectionPacketFullFlag = 0;
    memset(g_InjectionPacketBuffer, 0, sizeof(g_InjectionPacketBuffer));
    memset(g_InjectionTempPacketBuffer, 0, sizeof(g_InjectionTempPacketBuffer));
    memset(g_instructionPacketBuffer, 0, sizeof(g_instructionPacketBuffer));
    memset(g_InjectionPacketBufferCheckFlag, 0, sizeof(g_InjectionPacketBufferCheckFlag));
}