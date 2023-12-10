//
// Created by kfzjw008 on 2023-05-16.
//

#include "commandParsing.h"


//	指令表
uint16_t g_instructionTable[2 + instructionEncodingLength + instructionRoutingLength][g_instructionTableLength];
uint16_t g_instructionTableNum = 0;//	指令表计数
uint16_t g_instructionTableNumID;//	指令表ID计数  干什么的？

/*指令解析与事件表*/

#define g_instructionPackageMaxLength  (256) //一条指令包数据不超过256字节
#define g_eventTableBufferMaxLength  (1000) //指令解析与时间表业务池不超过1000数据


uint8_t g_eventTableBuffer[g_instructionPackageMaxLength][g_eventTableBufferMaxLength];//事件表缓冲区
//地址是

uint8_t g_eventTableBufferCount = 0;// 事件表计数


/*指令解析与宏指令*/

#define g_macroInstructionBufferMaxLength  (1000) //指令解析与宏指令业务池不超过1000数据


uint8_t g_macroInstructionBuffer[g_instructionPackageMaxLength][g_macroInstructionBufferMaxLength];//宏指令缓冲区
//地址是
uint8_t g_macroInstructionBufferCount = 0;// 宏指令缓冲池计数








/*
请求指令
函数名：requestInstructionParsing
功能：对指令表进行管理
输入：一条指令  data
    格式为：
    指令 ID：16bit，由业务 ID（8bit）+序号（bit）组成。序号在各业务规范中定义，并可随业务
    剪裁而增减。
    b) 指令应用过程识别符：16bit，使用其中的低 11bit。应用过程编码见《数据注入格式》附录 A。
    c) 指令编码：需要根据应用过程的需要编码，由具体项目任务确定。
    d) 指令路由：需要根据应用过程的需要设计，由具体项目任务确定。


输出：0不能接受请求指令  1可以接受请求指令

需要定义指令表：指令 ID ，指令应用过程识别符 ，指令编码 ，指令路由

*/



uint8_t requestInstructionParsing(uint8_t data[]) {
    //21是业务ID，后边的是序号

    if (data[1] == 128) {
        //指令表增加一条指令

        //将 除前32位后的一条指令 放到指令表中。
        memcpy(g_instructionTable[g_instructionTableNum++], &data[2],
               2 + instructionEncodingLength + instructionRoutingLength);

        return 1;
    } else if (data[1] == 129) {
        //删除n条指令：因为指令表无序，这里采取的策略是将指令表末尾的指令填充到当前位置。
        int n = data[2];
        for (int i = 1; i <= n; i++) {
            uint16_t deleteID = data[i * 2 + 1] << 8 && data[(i + 1) * 2]; //获取每一个要删除的指令ID

            for (int i = 0; i < g_instructionTableNum; i++) { //遍历指令表
                if (deleteID == g_instructionTable[i][0] << 8 && g_instructionTable[i][1]) {//判断ID是否相等
                    memcpy(g_instructionTable[i], g_instructionTable[g_instructionTableNum--],
                           2 + instructionEncodingLength + instructionRoutingLength);

                }
            }
        }
        return 1;

    } else if (data[1] == 130) {
        //设置n条指令的应用过程识别符
        int n = data[2];
        for (int i = 1; i <= 4 * n; i += 2) {  //每次走2步
            uint16_t alterID = data[i * 2 + 1] << 8 && data[i * 2 + 2]; //获取每一个要修改的指令ID

            for (int i = 0; i < g_instructionTableNum; i += 2) { //遍历指令表
                if (alterID == g_instructionTable[i][0] << 8 && g_instructionTable[i][1]) {//判断ID是否相等

                    g_instructionTable[i][2] = data[i * 2 + 3];
                    g_instructionTable[i][3] = data[i * 2 + 4];
                }
            }
        }
        return 1;

    } else if (data[1] == 131) {
        //设置n条指令的编码
        int n = data[2];
        for (int i = 1; i <= 2 * n; i += 2) {  //每次走2步
            uint16_t alterID = data[i * 2] << 8 && data[i * 2 + 1]; //获取每一个要修改的指令ID

            for (int i = 0; i < g_instructionTableNum; i += 2) { //遍历指令表
                if (alterID == g_instructionTable[i][0] << 8 && g_instructionTable[i][1]) {//判断ID是否相等

                    g_instructionTable[i][4] = data[i * 3];
                    g_instructionTable[i][5] = data[i * 3 + 1];
                }
            }
        }
        return 1;

    } else if (data[1] == 132) {
        //设置n条指令的路由
        int n = data[2];
        for (int i = 1; i <= 2 * n; i += 2) {  //每次走2步
            uint16_t alterID = data[i * 2] << 8 || data[i * 2 + 1]; //获取每一个要修改的指令ID

            for (int i = 0; i < g_instructionTableNum; i += 2) { //遍历指令表
                if (alterID == g_instructionTable[i][0] << 8 || g_instructionTable[i][1]) {//判断ID是否相等

                    g_instructionTable[i][6] = data[i * 3];
                    g_instructionTable[i][7] = data[i * 3 + 1];
                }
            }
        }
        return 1;
    } else return 0;

}


/*
指令解析
函数名：instructionParsing
输入：**一条**   即将执行的指令
功能：可以理解基本的指令请求，根据指令码表和路由表，确定指令发送对象，确定是转发还是释放。
输出：0或者1。


*/
uint8_t instructionParsing(uint8_t data[]) {
    //指令ID
    uint16_t instructionID = data[0] << 8 || data[1];
    if (instructionID >> 8 == 21) {//属于请求指令
        requestInstructionParsing(data);
        return 1;
    } else if () { //需要根据各业务定义的ID进行分类，决定转发还是释放
        for (int i = 0; i < g_instructionTableNum; i++) {
            if (instructionID == *g_instructionTable[0]) {
                printf("当前指令是：+");
            }
        }

    } else return 0; //出错
}


/*
指令包解析
输入：指令池中的一条完整的指令包
功能：  对接收到的数据注入分解出的指令包进行分析，识别不同的执行方式（立即指令、事件表指令、宏指令、上注程序、立即转发），
        将指令包内容分发给相应业务。直接触发相应业务的执行。（主要是识别指令包的开头）
        需要维护两个表：立即执行指令表  延时执行指令表
输出：0/1


*/
uint8_t instructionPacketParsing(uint16_t data[]) {
    //指令包数据长度
    uint16_t instructionPacketLength = data[4] << 8 || data[5];

    //执行方式类型编码  占1字节？
    uint8_t executionType = data[6];


    if (executionType == 0xF0) { //立即指令,调用指令解析函数立即执行
        for (int i = 0; i < instructionPacketLength; i++) {
            instructionParsing(data[7 + i]); //下标有问题，期望参数是一条指令
        }

    } else if (executionType == 0xF1 || executionType == 0xF2 || executionType == 0xF3 || executionType == 0xF9 ||
               executionType == 0xFA || executionType == 0xFB) { //延时指令

        memcpy(g_eventTableBuffer[g_eventTableBufferCount++], &data[7], instructionPacketLength - 7);
    } else if (executionType == 0xF4 || executionType == 0xF5 || executionType == 0xF6) { //宏指令
        memcpy(g_macroInstructionBuffer[g_macroInstructionBufferCount++], &data[7], instructionPacketLength - 7);
    } else if (executionType == 0xFC) { //任务级指令
        //暂不考虑
        printf("任务级指令");
    } else

        return 0;
}