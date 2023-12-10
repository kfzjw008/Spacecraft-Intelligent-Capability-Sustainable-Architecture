#include <stdio.h>
#include <stdint.h>
#include "test/testConsole.h"
#include "global.h"
#include "windowsUtil.h"
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
        初始化哪吒架构的相关组件和变量。

调用方法：
        在主函数开始时被调用。

编写日期：2023-12-10
修改记录：
********************************************************************/
void nezhaInit() {
    mainInit();
    DataInjectionTag=0;
    CommandParsingTag=0;
    MacroInstructionTag=0;
    EventTableTag=0;
    OnlineMonitoringTag=0;
    fdataInjectionInit();
    fcommandParsingInit();
    fmacroInstructionInit();
    feventTableInit();
    fonlineMonitoringInit();



}

/********************************************************************
功能：
        哪吒架构的主要执行函数，用于定期执行架构的核心功能。

调用方法：
        在主循环中被重复调用。

编写日期：2023-12-10
修改记录：
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
功能：
        执行程序的初始设置和配置，例如内存分配和初步的系统检查。

调用方法：
        在 nezhaInit 函数中被调用。

编写日期：2023-12-10
修改记录：
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

#ifdef __linux__
    // Linux系统使用POSIX共享内存
    int shm_fd = shm_open("my_shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    void *ptr = mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
#elif defined(_WIN32)
    // Windows系统使用Windows API创建共享内存
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedData), "my_shared_memory");
    void *ptr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
#endif
}

/********************************************************************
功能：
        执行哪吒架构的清理操作，如释放资源和保存状态。

调用方法：
        在程序结束前被调用。

编写日期：2023-12-10
修改记录：
********************************************************************/
void nezhaEnd() {



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
    SharedData *sharedData = /* 获取共享内存的指针 */;

    if (sharedData->flag == 1) {
        // 将content写入数据缓冲区
        writeToDataBuffer(sharedData->content);
        // 修改DataInjectionTag
        DataInjectionTag = 1;
        // 将flag修改为0
        sharedData->flag = 0;
    }
}