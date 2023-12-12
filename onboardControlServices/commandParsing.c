//
// Created by kfzjw008 on 2023-05-16.
//

#include "commandParsing.h"

/********************************************************************
功能：
      解析接收到的指令请求，并执行相应的操作，如增加、删除、修改指令表中的指令。
参数：
      data[] - 接收到的指令数据数组。
返回值：
      1 - 如果指令执行成功。
      0 - 如果指令执行失败或未识别的指令。
主要思路：
      首先根据 data[1] 的值判断指令类型，然后执行相应的操作：
      - 128：增加一条指令到指令表。
      - 129：删除指定数量的指令。
      - 130：设置指定指令的应用过程识别符。
      - 131：设置指定指令的编码。
      - 132：设置指定指令的路由。
      对于每种操作，解析并应用指令数据，然后更新指令表。
调用方法：
      uint8_t result = requestInstructionParsing(data);
      // 其中 data 是接收到的指令数据数组
编写日期：
      2023-12-11
修改记录：
      无
注意：
      函数假定 data 数组中包含有效的指令数据，且指令格式符合预期。
      指令表操作需要确保数据完整性和一致性。
********************************************************************/
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


/********************************************************************
功能：
      指令解析。解析接收到的指令数据并执行相应的操作，包括处理请求指令和其他业务相关指令。
参数：
      data[] - 接收到的指令数据数组。
返回值：
      1 - 如果指令被正确解析和执行。
      0 - 如果指令解析失败或不被识别。
主要思路：
      首先解析指令ID（data数组的前两个字节）。
      如果指令ID的高位为21，则视为请求指令，调用 requestInstructionParsing 函数处理。
      否则，根据业务定义的ID进行分类，决定是转发还是释放该指令。
      遍历指令表，查找并执行匹配的指令操作。
调用方法：
      uint8_t result = instructionParsing(data);
      // 其中 data 是接收到的指令数据数组
编写日期：
      2023-12-11
修改记录：
      无
注意：
      函数依赖于全局变量 g_instructionTable 和 g_instructionTableNum 来检查和执行指令。
      需要确保 data 数组包含有效的指令数据。
      指令ID的正确解析对于执行正确的操作至关重要。
********************************************************************/
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


/********************************************************************
功能：
      解析指令包并根据执行方式类型编码执行相应的操作。
参数：
      data[] - 指向指令包数据的指针。
返回值：
      1 - 如果指令包被正确解析并执行。
      0 - 如果执行类型编码不被识别，或指令解析失败。
主要思路：
      首先解析指令包的数据长度和执行方式类型编码。
      根据执行方式类型编码判断指令类型（立即指令、延时指令、宏指令等）。
      对于立即指令，遍历指令包中的所有指令并立即执行。
      对于延时指令和宏指令，将指令存储在相应的缓冲区中，以便后续处理。
      对于任务级指令，暂不考虑，打印提示信息。
调用方法：
      uint8_t result = instructionPacketParsing(data);
      // 其中 data 是指向指令包数据的指针
编写日期：
      2023-12-11
修改记录：
      无
注意：
      函数依赖于全局变量 g_eventTableBuffer 和 g_macroInstructionBuffer 以存储延时指令和宏指令。
      指令包中指令的正确解析和分类对于执行正确的操作至关重要。
      需要确保 data 数组包含有效且完整的指令包数据。
********************************************************************/
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


/********************************************************************
功能：
      指令释放。将指令发送给相应的业务进行处理，但不负责指令的执行。
参数：
      command - 指向要释放的指令数据的指针。
主要思路：
      根据指令的应用过程识别符确定指令的接收业务，并将指令发送给该业务。
      不关注指令执行的结果。
调用方法：
      releaseCommand(command);
      // 其中 command 是指向要释放的指令数据的指针
编写日期：
      2023-12-11
修改记录：
      无
注意：
      确保指令正确分类并发送给相应的业务。
      处理读写冲突和时序冲突。
********************************************************************/
void releaseCommand(uint8_t *command) {
    // 解析指令的应用过程识别符
    uint8_t appProcessIdentifier = command[0]; // 示例，实际位置可能不同

    // 根据应用过程识别符发送指令给相应的业务
    if (appProcessIdentifier == STARBOARD_DATA_MANAGEMENT) {
        // 发送指令给星载数管业务
        // ... (省略发送代码)
    } else {
        // 发送指令给其他业务
        // ... (省略发送代码)
    }
}




/********************************************************************
功能：
      指令转发。将指令数据发送给系统中的非星载数管应用过程的设备。
参数：
      command - 指向要转发的指令数据的指针。
主要思路：
      组织要转发的指令数据，并通过查询指令表确定转发的途径。
      解决转发通道的冲突，并发送指令给设备访问业务。
      不关注指令转发的执行结果。
调用方法：
      forwardCommand(command);
      // 其中 command 是指向要转发的指令数据的指针
编写日期：
      2023-12-11
修改记录：
      无
注意：
      确保指令数据正确地发送给设备访问业务。
      处理转发通道的冲突问题。
********************************************************************/
void forwardCommand(uint8_t *command) {
    // 查询指令表确定转发途径
    uint8_t route = determineRoute(command); // 示例，实际实现可能不同

    // 检查并解决转发通道冲突
    // ... (省略冲突解决代码)

    // 转发指令数据
    // ... (省略转发代码)
}




/********************************************************************
功能：
      清理指令解析模块占用的资源。
主要思路：
      释放分配的资源，清理全局变量。
调用方法：
      fcommandParsingEnd();
编写日期：
      2023-12-11
修改记录：
      无
注意：
      确保所有资源被正确释放，避免内存泄漏。
********************************************************************/
void fcommandParsingEnd() {
    // 清理指令表资源
    // ... (如果有需要释放的资源，添加相应代码)

    // 重置全局变量
    g_instructionTableNum = 0;
    g_eventTableBufferCount = 0;
    g_macroInstructionBufferCount = 0;

    // 这里可以添加其他必要的清理代码
}


/********************************************************************
功能：
      执行指令解析的主要业务逻辑。
主要思路：
      处理接收到的指令，执行指令解析和分发过程。
调用方法：
      fcommandParsingMain();
编写日期：
      2023-12-11
修改记录：
      无
注意：
      函数需要处理各种类型的指令，确保指令正确分类和执行。
********************************************************************/
vvoid fcommandParsingMain() {
    // 假设 data 是接收到的指令数据数组，应包含完整的指令包格式
    uint8_t data[MAX_INSTRUCTION_LENGTH];
    // 假设这是接收数据的结果，其中 MAX_INSTRUCTION_LENGTH 是指令数据的最大长度
    // TODO: 实现接收指令数据的实际代码逻辑

    // 检查指令包的第一个字节，确定是请求指令还是其他业务相关的指令
    // 假设指令包的第一个字节为指令类型标识
    uint8_t instructionType = data[0];

    // 初始化结果变量
    uint8_t result = 0;

    // 根据指令类型分别处理
    switch (instructionType) {
        case REQUEST_INSTRUCTION:
            // 处理请求指令，调用请求指令解析函数
            result = requestInstructionParsing(data + 1); // +1 为了跳过类型字节
            break;
        case IMMEDIATE_INSTRUCTION:
        case DELAYED_INSTRUCTION:
        case MACRO_INSTRUCTION:
        case MISSION_LEVEL_INSTRUCTION:
            // 这些类型的指令包需要调用指令包解析函数
            result = instructionPacketParsing((uint16_t *)(data + 1)); // +1 为了跳过类型字节
            break;
        default:
            // 未识别的指令类型，记录错误或进行其他处理
            // TODO: 添加错误处理逻辑
            result = 0;
            break;
    }

    // 根据解析结果处理
    if (result == 1) {
        // 指令解析成功，根据具体业务逻辑进行操作
        // TODO: 根据具体业务逻辑进一步处理
    } else {
        // 指令解析失败，处理错误
        // TODO: 添加错误处理逻辑
    }
}



/********************************************************************
功能：
      初始化指令解析模块，准备指令表和相关数据结构。
主要思路：
      初始化全局变量、指令表和缓冲区，为指令解析和执行做准备。
调用方法：
      fcommandParsingInit();
编写日期：
      2023-12-11
修改记录：
      无
注意：
      确保所有初始化步骤按预期执行，避免未初始化变量引起的错误。
********************************************************************/
void fcommandParsingInit() {
    // 初始化指令表的数量
    g_instructionTableNum = 0;

    // 初始化事件表缓冲区计数
    g_eventTableBufferCount = 0;

    // 初始化宏指令缓冲区计数
    g_macroInstructionBufferCount = 0;

    // 这里可以添加其他必要的初始化代码
}


