
#include "main.h"

/********************************************************************
功能：
        程序的主入口点。负责初始化哪吒架构、执行主循环直至触发终止条件，
        然后执行结束处理。

调用方法：
        通过运行程序来自动调用。

编写日期：2023-12-10
修改记录：
********************************************************************/
int main() {


    nezhaInit();//哪吒架构初始化

    while (1) {
        nezha();//哪吒架构运行
        if (g_EndlessFLag == 1)break;
    }
    nezhaEnd();

    return 0;
}

/********************************************************************
功能：
        初始化哪吒架构的相关组件和变量。设置初始标签状态，并调用各个初始化函数。
参数：
        无
返回值：
        无
主要思路：
        调用每个功能模块的初始化函数以设置初始状态和准备运行环境。
调用方法：
        在 main 函数的开始被调用。
编写日期：
        2023-12-10
修改记录：
        无
********************************************************************/
void nezhaInit() {
    mainInit();
    DataInjectionTag = 0;
    CommandParsingTag = 0;
    MacroInstructionTag = 0;
    EventTableTag = 0;
    OnlineMonitoringTag = 0;
    fdataInjectionInit();
    fcommandParsingInit();
    fmacroInstructionInit();
    feventTableInit();
    fonlineMonitoringInit();
    g_EndlessFLag=0;

}

/********************************************************************
功能：
        哪吒架构的主要执行函数，用于定期执行架构的核心功能。
参数：
        无
返回值：
        无
主要思路：
        根据不同功能模块设置的标签，依次检查并执行相应的业务逻辑。
调用方法：
        在主循环中被重复调用。
编写日期：
        2023-12-10
修改记录：
        无
********************************************************************/
void nezha() {
    collectDataCommunication();
    // 检查数据注入业务标签并执行相应业务
    if (checkDataInjectionTag()) {
        fdataInjectionMain();
    }
    // 检查指令解析和执行业务标签并执行相应业务
    if (checkCommandParsingTag()) {
        fcommandParsingMain();
    }
    // 检查宏指令标签并执行相应业务
    if (checkMacroInstructionTag()) {
        fmacroInstructionMain();
    }
    // 检查事件表标签并执行相应业务
    if (checkEventTableTag()) {
        feventTableMain();
    }
    // 检查在线监控标签并执行相应业务
    if (checkOnlineMonitoringTag()) {
        fonlineMonitoringMain();
    }


}
/********************************************************************
功能：    初始化内存分配和系统设置
参数：    无
返回值：  无
主要思路：执行必要的初始化，如内存分配和变量设置
调用方法：在 nezhaInit() 中调用
编写日期：2023-12-10
修改记录：无
********************************************************************/
void mainInit() {
    // FF为1字节，uint16_t代表着占用2字节，uint8_t占用1字节。
    printf("Hello,CISCA!!!!!!!\n", sizeof(uint8_t));
    mallocInit(); // windows专用
    printf("First address of the virtual space= 0x%08X \n", MEMORY);
    printf("Online monitoring first address= 0x%08X \n", MONI_TAB_STR_ADRS);
    outpDw(2, 0x2E);
    printf("inpDw Test = %d \n", inpDw(2));
    // 哪吒架构第一行代码
    g_EndlessFLag = 0;//停止标识修改为0
    // 根据操作系统进行共享内存的初始化
#ifdef __linux__
    // Linux系统使用POSIX共享内存
    int shm_fd = shm_open("my_shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    *ptr = mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
#elif defined(_WIN64)
    // Windows系统使用Windows API创建共享内存
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedData), "my_shared_memory");
    void *ptr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
#endif
}

/********************************************************************
功能：
        执行哪吒架构的清理操作，如释放资源和保存状态。
参数：
        无
返回值：
        无
主要思路：
        调用每个功能模块的结束函数以释放分配的资源和执行清理操作。
调用方法：
        在程序结束前被调用。
编写日期：
        2023-12-10
修改记录：
        无
********************************************************************/
void nezhaEnd() {
    fdataInjectionEnd(); // 清理数据注入模块资源
    fcommandParsingEnd(); // 清理指令解析模块资源
    fmacroInstructionEnd(); // 清理宏指令模块资源
    feventTableEnd(); // 清理事件表模块资源
    fonlineMonitoringEnd(); // 清理在线监控模块资源
    // 进一步清理可能存在的全局或静态资源
    // 如果有动态分配的全局内存，请在这里释放
    free(MEMORY); // 假设 MEMORY 是全局动态分配的内存
    MEMORY = NULL;
    // 如果有其他共享资源或全局状态需要清理，请在这里进行
}


// 结构体定义共享内存中的数据格式
typedef struct {
    int flag;
    char content[CONTENT_SIZE]; // CONTENT_SIZE 需要定义为内容的大小
} SharedData;

// 写入数据缓冲区的函数示例
void writeToDataBuffer(char *content) {
    // 实现数据写入数据缓冲区的逻辑
}

void collectDataCommunication() {
    // 假设SharedData结构定义了flag和content
    SharedData *sharedData ;
    sharedData = (SharedData*)ptr;

    if (sharedData->flag == 1) {
        // 将content写入数据缓冲区
        writeToDataBuffer(sharedData->content);
        // 修改DataInjectionTag
        DataInjectionTag = 1;
        // 将flag修改为0
        sharedData->flag = 0;
    }
}

/********************************************************************
功能：检查数据注入业务标签
参数：无
返回值：如果 DataInjectionTag 为 1，则返回 1；如果为 0，则返回 0
主要思路：通过检查全局变量 DataInjectionTag 的值来确定是否需要执行数据注入业务
调用方法：uint8_t result = checkDataInjectionTag();
编写日期：2023-12-10
修改记录：无
********************************************************************/
uint8_t checkDataInjectionTag() {
    return DataInjectionTag;
}

/********************************************************************
功能：检查指令解析业务标签
参数：无
返回值：如果 CommandParsingTag 为 1，则返回 1；如果为 0，则返回 0
主要思路：通过检查全局变量 CommandParsingTag 的值来确定是否需要执行指令解析业务
调用方法：uint8_t result = checkCommandParsingTag();
编写日期：2023-12-10
修改记录：无
********************************************************************/
uint8_t checkCommandParsingTag() {
    return CommandParsingTag;
}

/********************************************************************
功能：检查宏指令业务标签
参数：无
返回值：如果 MacroInstructionTag 为 1，则返回 1；如果为 0，则返回 0
主要思路：通过检查全局变量 MacroInstructionTag 的值来确定是否需要执行宏指令业务
调用方法：uint8_t result = checkMacroInstructionTag();
编写日期：2023-12-10
修改记录：无
********************************************************************/
uint8_t checkMacroInstructionTag() {
    return MacroInstructionTag;
}

/********************************************************************
功能：检查事件表业务标签
参数：无
返回值：如果 EventTableTag 为 1，则返回 1；如果为 0，则返回 0
主要思路：通过检查全局变量 EventTableTag 的值来确定是否需要执行事件表业务
调用方法：uint8_t result = checkEventTableTag();
编写日期：2023-12-10
修改记录：无
********************************************************************/
uint8_t checkEventTableTag() {
    return EventTableTag;
}

/********************************************************************
功能：检查在线监控业务标签
参数：无
返回值：如果 OnlineMonitoringTag 为 1，则返回 1；如果为 0，则返回 0
主要思路：通过检查全局变量 OnlineMonitoringTag 的值来确定是否需要执行在线监控业务
调用方法：uint8_t result = checkOnlineMonitoringTag();
编写日期：2023-12-10
修改记录：无
********************************************************************/
uint8_t checkOnlineMonitoringTag() {
    return OnlineMonitoringTag;
}
