#include<iostream>
#include <stdint.h>
#include<string.h>
#include"global.h"
#include"main.c"
#include<bitset>
using namespace std;

/***********************************************************
packetSplicing
功能描述：注入包拼接
输入参数：  uint8_t* data 注入包，格式为数组，一个元素大小为8位；
            uint16_t size 注入包长度，单位：字节
输出参数：拼接后包的长度
说明：调用此函数时，已接收到尾包，此函数将多个注入包拼接为一个包
***********************************************************/
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
/***********************************************************
程序名称：pac ketIdentification
功能描述：注入包识别
输入参数：  uint8_t* data 注入包，格式为数组，一个元素大小为8位；
            uint16_t size 注入包长度，单位：字节
输出参数：0:不是包  数字:从0字节到数字字节构成一个包
说    明：注入包识别，初步识别注入包是否正确，并判断是首包、尾包、独立包。
***********************************************************/
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

//注入包接收
/***********************************************************
程序名称： injectionPacketReception(uint8_t data[],uint8_t size)
功能描述：数据注入包接收
输入参数：uint8_t data[],uint8_t size
输出参数：无
说    明：
***********************************************************/

void injectionPacketReception(uint8_t data[],uint8_t size){ 

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

//判断是指令包开头
//指令包固定开始为：00011000 10110100
bool isInstructionPacket(uint8_t data1,uint8_t data2){
    if(data1 == 0b00011000 && data2 == 0b10110100){
        return 1;
    }
    else return 0;
}
//注入包解包
//输入是一个注入包
//返回值是   空 | 指令序列数组的指针？

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


    // #define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*注入包缓冲区起始地址
    // #define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*注入包缓冲区结束地址
    // #define  MAX_INJECT_BUFFER_NUM                      (10)        /*注入包缓冲区最大个数*/


//包正确性检测
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


/***********************************************************
程序名称：
功能描述： 包唯一性
输入参数：
输出参数：指令包
说    明：对缓冲区的数据展开相关检查
#define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*注入包缓冲区起始地址
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*注入包缓冲区结束地址
***********************************************************/
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

/***********************************************************
程序名称：
功能描述： 包合法性
输入参数：
输出参数：指令包
说    明：对缓冲区的数据展开相关检查
#define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*注入包缓冲区起始地址
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*注入包缓冲区结束地址
***********************************************************/
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

/***********************************************************
程序名称：
功能描述： 包相关性
输入参数：
输出参数：指令包
说    明：对缓冲区的数据展开相关检查
#define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*注入包缓冲区起始地址
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*注入包缓冲区结束地址
***********************************************************/

bool packageRelevance(uint32_t item){
    //包与此前已收到并通过检查的包之间的包序号和首末分量的逻辑关系，通过“包序控制”域来判别。
    uint32_t item;
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

// 注入包检查
bool injectionPacketInspection(int size){

    printf("%d",*g_InjectionPacketBuffer[g_InjectionPacketBufferNum]); //注入包缓冲区拿数据
    // for(int i = 0; i < size; i ++){
    //     printf("%d\n",g_InjectionPacketBuffer[g_InjectionPacketBufferNum][i]);
    // }
    bool flag;
    flag = packageValidity(g_InjectionPacketBuffer[g_InjectionPacketBufferNum]);
    if(flag == 1) return 0;
    packageUniqueness();
    if(flag == 1) return 0;

    // packageLegitimacy(data);
    // if(flag == 1) return 0;
    // packageRelevance(data);
    // if(flag == 1) return 0;
    return 1;
}


int main()
{
    int i;// 循环data数据的下标

    // uint8_t data[5] = {0b00011110,0b00001100,0b00000111,0b10101010,0b01101010};  // 设备访问传递的数据
    // for(i = 0;i < 5;i ++){
    //     cout << bitset<8>(data[i]) << "   ";
    //     // printf("data中第i个数据为：%B",data[i]);
    // }
    int size = 7;
    uint8_t data[size] = {0b00010011,0b00110100,0b11000000,0b00000001,0b00000000,0b00000111,0b11111111};// 设备访问传递的数据
    // packetIdentification(data,6);
// 00010011001101001100000000000001000000000000011111111111
    // 注入包接收

    injectionPacketReception(data,size);
    g_injectionPacketRxCount ++;

    //注入包检查
    if(injectionPacketInspection(size)){
        printf("注入包正确~");
    }else{
        printf("注入包不正确~");
    }

    //注入包拼接为注入帧


    char myvar[8];
    
    // while(1){ // 一直处于等待输入数据的状态
    //     cout << "input the data"  << endl;
        
    //     for(int i = 0;i < 8; i ++){
    //         cin >> myvar[i];
    //     }
    //     for(int i = 0; i < 8; i ++){
    //         if(myvar[i] == '0'){
    //             data = (data << 1); // Shift left
    //         } else if (myvar[i] == '1') {
    //             data = (data << 1) | 1; // Shift left and set the least significant bit
    //         }
    //     }

    //     // 自此，拿到了每次传递的一个uint8_t数据。

    //     cout << bitset<8>(data) << endl;
    // }

    puts("");
    mallocInit();
    printf("首地址为：%08X",MEMORY);
    printf("尾地址为：%08X",MEMORY + 0x000FFFFF);

    return 0;
}