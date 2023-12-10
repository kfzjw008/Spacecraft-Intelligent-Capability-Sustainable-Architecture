//
// Created by kfzjw008 on 2023-05-16.
//
#include <stdint.h>

#ifndef SICSA_GLOBAL_H
#define SICSA_GLOBAL_H


/*************Flag大全*********************/
uint8_t DataInjectionTag;
uint8_t CommandParsingTag;
uint8_t MacroInstructionTag;
uint8_t EventTableTag;
uint8_t OnlineMonitoringTag;


















/*************常用宏定义*********************/
#define  INJECTDATA_SIZE                            (256)        /*数据注入一次的长度（字节）*/
#define  MAX_INJECT_PKG_LEN                         (128)       /*注入包最长长度*/
#define  MAX_INJECT_BUFFER_NUM                      (10)        /*注入包缓冲区最大个数*/
#define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*注入包缓冲区起始地址*/
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*注入包缓冲区结束地址*/
#define    PLAN_OUT_STR_ADRS            (0x0000C000+MEMORY)    /*任务规划输出结果起始地址*/
#define    PLAN_OUT_END_ADRS            (0x0000CFFF+MEMORY)    /*任务规划输出结果结束地址*/
#define    PLAN_TAB_STR_ADRS            (0x0000D000+MEMORY)    /*任务规划表起始地址*/
#define    PLAN_TAB_END_ADRS            (0x0000DFFF+MEMORY)    /*任务规划表结束地址*/
#define    MONI_OUT_STR_ADRS            (0x0000E000+MEMORY)    /*在线监控输出结果起始地址*/
#define    MONI_OUT_END_ADRS            (0x0000EFFF+MEMORY)    /*在线监控输出结果结束地址*/
#define    MONI_TAB_STR_ADRS            (0x0000F000+MEMORY)    /*在线监控表起始地址*/
#define    MONI_TAB_END_ADRS            (0x0000FFFF+MEMORY)    /*在线监控表结束地址*/
#define        MAX_IMM_CMD_NUM                (10)    /*转发立即指令最大条数*/
#define        MAX_PLAN_CMD_LEN            (50)        /*转发任务规划单条最大长度字数，原转发1553BC立即指令*/
#define        MAX_MONI_CMD_LEN            (64)        /*转发在线监控立即指令单条最大长度字数*/

//宏指令相关
#define    MACR_TAB_STR_ADRS            (0x00010000+MEMORY)    /*宏指令库表起始地址*/
#define    MACR_TAB_END_ADRS            (0x00011FFF+MEMORY)    /*宏指令库表结束地址*/
#define    MACR_LSH_STR_ADRS            (0x00012000+MEMORY)    /*宏指令库表起始地址*/
#define    MACR_LSH_END_ADRS            (0x00013FFF+MEMORY)    /*宏指令库表结束地址*/
#define    MACR_HSJ_STR_ADRS            (0x00014000+MEMORY)    /*宏指令库表起始地址*/
#define    MACR_HSJ_END_ADRS            (0x00015FFF+MEMORY)    /*宏指令库表结束地址*/
#define    MACR_HCX_STR_ADRS            (0x00016000+MEMORY)    /*宏指令库表起始地址*/
#define    MACR_HCX_END_ADRS            (0x00017FFF+MEMORY)    /*宏指令库表结束地址*/
#define MAX_MACRO_LENGTH 100 // 每条宏指令的最大长度
#define MACRO_INSTRUCTION_COUNT 81 // 宏指令库可以存储的宏指令数量
uint16_t g_macroInstructionTable[MACRO_INSTRUCTION_COUNT][MAX_MACRO_LENGTH]; // 宏指令库（宏序列）
uint8_t g_temporaryMacroTable[MACRO_INSTRUCTION_COUNT][MAX_MACRO_LENGTH]; // 临时宏表
uint8_t g_macroDataTable[MACRO_INSTRUCTION_COUNT][MAX_MACRO_LENGTH];     // 宏数据表
uint8_t g_macroProgramTable[MACRO_INSTRUCTION_COUNT][MAX_MACRO_LENGTH];   // 宏程序表

//事件表相关 1
#define    EVENT_TAB_STR_ADRS            (0x00020000+MEMORY)    /*事件项表起始地址*/
#define    EVENT_TAB_END_ADRS            (0x00021FFF+MEMORY)    /*事件项表结束地址*/
#define    EVENT_SEA_STR_ADRS            (0x00022000+MEMORY)    /*事件检索表起始地址*/
#define    EVENT_SEA_END_ADRS            (0x00022FFF+MEMORY)    /*事件检索表结束地址*/

#define    EVENT_TO_INSEXE_ADRS            (0x00030000+MEMORY)    /*事件表业务将指令提交到指令动作业务的地址*/

uint8_t g_InjectionPacketBuffer[MAX_INJECT_BUFFER_NUM][MAX_INJECT_PKG_LEN];//	注入包缓冲区
uint8_t g_InjectionPacketBufferCheckFlag[MAX_INJECT_BUFFER_NUM];//注入包检查启用标志，0为未检查，1为合法，2为包格式不正确
uint16_t g_InjectionPacketBufferFlag[MAX_INJECT_BUFFER_NUM];/*注入包接收标志，AA为任务规划部件已处理，55为已接收待任务规划部件处理*/
uint16_t g_packetHeaderLength;//	包头长度
uint16_t g_packetFooterLength;//	包尾长度
uint16_t g_InjectionPacketBufferNum;//	注入包计数clion
uint16_t g_dataInjectionFlag;//	数据注入操作标识
uint16_t packageSequenceIdentifier;//	包序号标识
uint16_t g_injectionPacketRxCount;//	注入包接收计数
uint16_t g_injectionFrameRxCount;//	注入帧接收计数
uint16_t g_injectionSequenceOrChecksumArray;//	注入序号或校验和数组
uint16_t g_instructionPool;//	指令池
uint16_t g_instructionPoolNum;//
uint16_t g_instructionTable;//	指令表
uint16_t instructionEncodingLength;//	指令编码长度
uint16_t instructionRoutingLength;//	指令路由长度
uint16_t g_instructionTableNum;//	指令表计数
uint16_t g_instructionTableNumID;//	指令表ID计数
uint16_t g_instructionCount;//	指令个数
uint16_t g_instructionReleaseCount;//	指令释放计数
uint16_t g_instructionForwardCount;//	指令转发计数
uint16_t g_instructionParsingAndExecutionOperationFlag;//	指令解析和执行操作标识
uint16_t g_eventSearchTable;//	事件检索表
uint16_t g_eventItemTable;//	事件项表
uint16_t g_eventTableCapacity;//	事件表容量
uint16_t g_pendingEventCount;//	待执行事件计数
uint16_t g_appendedEventCount;//	追加事件计数
uint16_t g_currentEventItemID;//	当前事件项ID
uint16_t g_eventCount;//	事件表项数


uint16_t g_macroInstructionLookupTable_SE;//	宏指令检索表-序列
uint16_t g_MacroInstructionCount;//	宏指令个数
uint16_t g_NextAvailableMacroInstructionNumber;//	下一个可分配的宏指令序号
uint16_t g_TemporaryMacroInstructionGenerationCount;//	临时宏指令生成个数
uint16_t g_macroInstructionLookupTable_DA;//	宏指令检索表-数据
uint16_t g_macroInstructionLookupTable_PR;//	宏指令检索表-程序
uint16_t g_macroInstructionLookupTable_TE;//	宏指令检索表-临时
uint16_t g_parameterMonitoringTable;//	参数监视表
uint16_t g_monitoredParametersNum;//	监视参数个数
uint16_t g_monitoringStatus;//	监视状态
uint16_t g_actionStatus;//	动作状态
uint16_t g_actionCount;//	动作计数
uint16_t g_totalActionCount;//	动作总计数
uint16_t g_countdownExceedLimit;//	超限倒计数
uint16_t g_ingectSize;//注入数据长度
//到此为止，以上为所有文档内函数
/*uint16_t*	g_pui8MoniTablePoint;*/        /*在线监控表指针*/
uint16_t g_ui16MoniTableNum;            /*在线监控表条数*/
uint16_t g_ui16MoniParaCnt;/*TJSG增加，在线监控监视参数个数*/
uint16_t g_ui16MoniParaNum;/*TJSG增加，在线监控监视参数序号轮询*/
uint16_t g_ui8MoniStatusPara;/*TJSG增加，在线监控监视状态轮询参数*/
uint16_t g_ui8MoniActionStatus;/*TJSG增加，在线监控动作状态*/
uint16_t g_ui8MoniActionCnt;/*TJSG增加，在线监控动作计数*/
uint16_t g_ui8MoniBeyondRemainCnt;/*TJSG增加，在线监控超限1倒计数轮询参数*/
uint16_t g_ui8MoniBeyondRemainCnt2;/*TJSG增加，在线监控超限2倒计数轮询参数*/
uint16_t g_ui16MoniParaActionSum;/*TJSG增加，在线监控监视参数动作总计数*/
uint16_t g_ui16MoniParaCurrVal;/*TJSG增加，在线监控参数当前值*/
uint16_t g_ui8MoniItemOperateStatus;/*TJSG增加，在线监控监视项操作状态*/
uint16_t g_ui8MoniParaUnExeActionNum;/*TJSG增加，在线监控监视参数待执行动作计数*/
uint16_t g_ui32MoniTaskExeMaxT;    /*TJSG增加，在线监控任务最大执行时间*/
uint16_t g_ui32MoniOutAddrWrite;/*TJSG增加，在线监控输出结果写指针的当前地址*/
uint16_t g_ui32MoniOutAddrRead;/*TJSG增加，在线监控输出结果读指针的当前地址*/
uint16_t g_ui8EngParaPrdSndDrv;/*TJSG增加，电机轮询标志，0x00已打开，0xCC已关闭*/
uint16_t g_ui8EngParaPrdSndCtrl;/*TJSG增加，头部轮询标志，0x00已打开，0xCC已关闭*/
uint16_t g_ucSysTimeFlag;/*TJSG增加，时间码类型，0x01表示4字节秒计数+2字节25微秒计数，0x02表示6字节百微秒计数*/





uint16_t g_ui16PlanCmdBuff[MAX_IMM_CMD_NUM][MAX_PLAN_CMD_LEN];/*任务规划指令接收数据区*/
uint8_t g_ui8PlanCmdFlag[MAX_IMM_CMD_NUM];/*任务规划指令接收标志，AA为任务规划部件已处理，55为已接收待任务规划部件处理*/
uint8_t g_ui8PlanOutFlag;/*任务规划输出标志，AA为任务规划部件已输出，55为已执行待任务规划部件处理*/
uint8_t g_ui16MoniCmdBuff[MAX_IMM_CMD_NUM][MAX_MONI_CMD_LEN];/*在线监控指令接收数据区*/
uint8_t g_ui8MoniCmdFlag[MAX_IMM_CMD_NUM];/*在线监控指令接收标志，AA为在线监控部件已处理，55为已接收待在线监控部件处理*/
uint8_t g_ui8MoniOutFlag;/*在线监控输出标志，AA为在线监控部件已输出，55为已执行待在线监控部件处理*/
unsigned short g_usSlowEngBuf[64];

/*-----------------------windows function define----------------------*/
#define     MAX_MEMORY_SIZE               (0x000FFFFF)        /*转发在线监控立即指令单条最大长度字数*/
int MEMORYSize;//内存大小，单位是字节
uint8_t *MEMORY;//虚拟存储首地址
#define outpDw(addr, data)   (MEMORY[addr] = data)/*#define	outpDw(port, data)						(*( unsigned int *)(port) = (data) )*/
#define inpDw(addr)         ((unsigned int)(*(volatile unsigned int*)(addr+MEMORY)))



/*-----------------------function define----------------------*/
#define    aceRegRead(aceReg)                    ((unsigned short)(*((unsigned int *)(aceReg))))
#define    aceRegWrite(aceReg, data)                ( (*(unsigned int *)(aceReg)) = (unsigned int)(data))
#define    aceMemRead(aceMem)                    ((unsigned short) ((*(unsigned int *)((aceMem)*4+(Ace_Mem_Base)))))
#define    aceMemWrite(aceMem, data)                ((*(unsigned int *)((aceMem)*4+(Ace_Mem_Base))) = (unsigned int)(data))
#define    write_vector(vector_data)                ((*(unsigned int *)((0x120) *4+(Ace_Mem_Base))) = (unsigned int)(vector_data))
#define    read_vector()                            ( (unsigned short) (*(unsigned int *)(0x120*4+(Ace_Mem_Base))))
#define    reset_ace()                            ( (*(unsigned int *)(Start_Reset_REG)) = (unsigned int)(0x01) )
#define    set_svc_req()                            (aceRegWrite((Config_1_REG),(aceRegRead(Config_1_REG) & 0xfdff)))
#define    reset_svc_req()                            (aceRegWrite((Config_1_REG),(aceRegRead(Config_1_REG) | 0x200)))


//#define outpDw(addr, data)   (*(volatile unsigned int*)(addr) = ((unsigned int)(data)))/*#define	outpDw(port, data)						(*( unsigned int *)(port) = (data) )*/
//#define inpDw(addr)         ((unsigned int)(*(volatile unsigned int*)(addr)))/*#define	inpDw(port)								(*((unsigned int *)(port)) )*/


#define outpDwB(addr, data)   (*(volatile unsigned char*)(addr) = ((unsigned char)(data)))/*#define outpDwB(addr,data)   (*(unsigned char*)(addr) = ((unsigned char)(data)))*/
#define inpDwB(addr)         ((unsigned char)(*(volatile unsigned char*)(addr)))
#define inpMemW(addr)        ((unsigned short)(*(volatile unsigned int*)(addr)))/*TJSG����*/
#define outpMemW(addr, data)   (*(volatile unsigned int*)(addr) = ((unsigned short)(data)))/*TJSG����*/
#endif //SICSA_GLOBAL_H
