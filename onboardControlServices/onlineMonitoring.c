//
// Created by kfzjw008 on 2023-05-16.
//

#include "onlineMonitoring.h"
/********************************************************************
     工程：天基载荷-哪吒架构
     作者：FZW
     修改者：FZW
     描述：在线监控
     主要函数：…………
     版本：说明文件的版本，完成日期
     修改：说明对文件的修改内容、修改原因以及修改日期
     参考文献：…………
********************************************************************/

#include "stdio.h"

/*#include "logLib.h"*/



#include "../global.h"

#include "onlineMonitoring.h"

/********************************************************************
功能：		在线监控初始化
参数：		......
（入口）	......
（入口）	......
（出口）	......
返回值：	......
主要思路：......
调用方法：上电初始化
编写日期：......
修改记录：......
********************************************************************/
void fMonitorInit(void)
{
    g_ui8MoniItemOperateStatus = 0;			/*在线监控监视项操作状态*/
    g_ui16MoniTableNum = 0;			/*在线监控表条数*/
    g_ui16MoniParaCnt = 0;/*TJSG增加，在线监控监视参数个数*/
    g_ui16MoniParaNum = 0;/*TJSG增加，在线监控监视参数序号轮询*/
    g_ui8MoniStatusPara = 0;/*TJSG增加，在线监控动作计数*/
    g_ui8MoniActionStatus = 0;/*TJSG增加，在线监控动作状态*/
    g_ui8MoniActionCnt = 0;/*TJSG增加，在线监控动作计数*/
    g_ui8MoniBeyondRemainCnt = 0;/*TJSG增加，在线监控超限1倒计数轮询参数*/
    g_ui8MoniBeyondRemainCnt2 = 0;/*TJSG增加，在线监控超限2倒计数轮询参数*/
    g_ui16MoniParaActionSum = 0;/*TJSG增加，在线监控监视参数动作总计数*/
    g_ui16MoniParaCurrVal = 0;/*TJSG增加，在线监控参数当前值*/
    g_ui8MoniParaUnExeActionNum = 0;/*TJSG增加，在线监控监视参数待执行动作计数*/
    g_ui32MoniOutAddrWrite = (MONI_OUT_STR_ADRS);/*TJSG增加，在线监控输出结果写指针的当前地址*/
    g_ui32MoniOutAddrRead = (MONI_OUT_STR_ADRS);/*TJSG增加，在线监控输出结果读指针的当前地址*/

    g_ui32MoniTaskExeMaxT = 0;	/*TJSG增加，在线监控任务最大执行时间*/
}


/*******************************************************************
功能：监视项使能
输入：监视项ID，xc_ui16Var1
输出：无
主要思路：根据监视项ID去存储区寻找并使能该监视项
********************************************************************/
void fMonitorParaMItermEn(uint16_t xc_ui16Var1)
{
    uint32_t n_ui32VarT1MIEn = 0; /*4字节变量-遍历参数监控表for循环*/
    uint32_t n_ui32VarT2MIEn = 0; /*4字节变量-*/

    uint16_t n_ui16VarT1MIEn = 0;	/*2字节变量-*/

    for(n_ui32VarT1MIEn = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1MIEn < ((uint32_t)(MONI_TAB_END_ADRS)); )
    {
        n_ui16VarT1MIEn = inpDw(n_ui32VarT1MIEn) & 0x0000FFFF;	/*当前监视项ID*/
        if(n_ui16VarT1MIEn == xc_ui16Var1)
        {
            n_ui32VarT2MIEn = inpDw(n_ui32VarT1MIEn + 4) | 0x00000100;
            outpDw((n_ui32VarT1MIEn + 4), n_ui32VarT2MIEn);
            break;
        }
        n_ui32VarT1MIEn = n_ui32VarT1MIEn +MAX_MONI_CMD_LEN;
        //fWatchDog();
    }
}


/**参数监控表监控项控制指令执行函数
**输入参数，XcTemp1在线监控指令序号
**输出参数，无
**说明:
**/
void fMonitorParaMItermCtrl(uint8_t XcTemp1)
{
    uint16_t n_ui16VarT1 = 0;	/*2字节局部变量-数据注入指令要控制的监控项ID*/
    uint16_t n_ui16VarT2 = 0;	/*2字节局部变量-当前监控项ID*/
    uint16_t n_ui16VarT3 = 0;	/*2字节局部变量-控制指令类型*/
    uint16_t n_ui16VarT4 = 0;	/*2字节局部变量-控制指令码字*/
    uint16_t n_ui16VarT5 = 0;	/*2字节局部变量-*/
    uint16_t n_ui16VarT6 = 0;	/*2字节局部变量-*/

    uint8_t n_ui8VarF1 = 0x00;	/*1字节局部变量-标记是否找到监控项*/

    uint32_t n_ui32VarT1 = 0;		/*4字节局部变量-遍历参数监控表*/
    uint32_t n_ui32VarT2 = 0;		/*4字节局部变量-*/

    n_ui16VarT1 = g_ui16MoniCmdBuff[XcTemp1][2];		/*数据注入指令要控制的监控项ID*/

    /*遍历监控表中所有监控项，查找对应监控项ID*/
    for(n_ui32VarT1 = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1 < ((uint32_t)(MONI_TAB_END_ADRS)); )
    {
        n_ui16VarT2 = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);	/*已有监控项ID*/
        if(n_ui16VarT2 == n_ui16VarT1)			/*找到要控制的监控项ID*/
        {
            n_ui8VarF1 = 0xAA; /*已找到要控制的监控项ID*/
            break;
        }
        else
        {
            n_ui8VarF1 = 0x55; /*未找到要控制的监控项ID*/
            n_ui32VarT1 = n_ui32VarT1 + MAX_MONI_CMD_LEN;	/*for循环变量递增*/
        }
    }

    if(n_ui8VarF1 == 0xAA)	/*已找到要控制的监控项ID*/
    {
        n_ui16VarT3 = g_ui16MoniCmdBuff[XcTemp1][1];		/*获取数据注入指令控制类型*/
        switch(n_ui16VarT3)
        {
            case 0x1111:	/*监控项监控使能禁止*/
                n_ui16VarT4 = (g_ui16MoniCmdBuff[XcTemp1][3])&0xFF00;	/*获取控制命令字*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 4);					/*获取当前状态*/
                if(n_ui16VarT4 == 0xAA00)	/*使能*/
                {
                    outpDw((n_ui32VarT1 + 4), (n_ui32VarT2 | 0x00000100));
                }
                else						/*禁止*/
                {
                    outpDw((n_ui32VarT1 + 4), (n_ui32VarT2 & 0xFFFFFEFF));
                }
                g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                break;
            case 0x2222:	/*监控项动作使能禁止*/
                n_ui16VarT4 = (g_ui16MoniCmdBuff[XcTemp1][3])&0xFF00;	/*获取控制命令字*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 4);					/*获取当前状态*/
                if(n_ui16VarT4 == 0xAA00)	/*使能*/
                {
                    outpDw((n_ui32VarT1 + 4), (n_ui32VarT2|0x00000200));
                }
                else						/*禁止*/
                {
                    outpDw((n_ui32VarT1 + 4), (n_ui32VarT2&0xFFFFFDFF));
                }
                g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                break;
            case 0x3333:	/*监控项优先级别设置*/
                n_ui16VarT4 = (g_ui16MoniCmdBuff[XcTemp1][3])&0xFF00;	/*获取控制命令字*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 4);					/*获取当前状态*/
                switch(n_ui16VarT4)
                {
                    case 0x0000:	/*允许删除*/
                        outpDw((n_ui32VarT1 + 4), (n_ui32VarT2&0xFFFF3FFF));
                        g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                        break;
                    case 0x1100:	/*一般*/
                        outpDw((n_ui32VarT1 + 4), (n_ui32VarT2&0xFFFF7FFF)|0x00004000);
                        g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                        break;
                    case 0x2200:	/*重要*/
                        outpDw((n_ui32VarT1 + 4), (n_ui32VarT2&0xFFFFBFFF)|0x00008000);
                        g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                        break;
                    case 0x3300:	/*紧急*/
                        outpDw((n_ui32VarT1 + 4), (n_ui32VarT2|0x0000C000));
                        g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                        break;
                    default:	/*控制命令字错误*/
                        g_ui8MoniItemOperateStatus = 0xA0;	/*监视项操作状态-监控项ID已找到，控制指令命令字错误*/
                        break;
                }
                break;
            case 0x4444:	/*监视周期设置*/
                n_ui16VarT4 = g_ui16MoniCmdBuff[XcTemp1][3];	/*获取监视周期设置值*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 36);	/*转换公式+监视周期*/
                if(n_ui16VarT4 >= 0x0001)	/*设置值合法*/
                {
                    n_ui32VarT2 = (n_ui32VarT2 & 0xFFFF0000) | n_ui16VarT4;
                    outpDw((n_ui32VarT1 + 36), n_ui32VarT2);
                    g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                }
                else
                {
                    g_ui8MoniItemOperateStatus = 0xA1;	/*监视项操作状态-监控项ID已找到，监视周期设置值错误*/
                }
                break;
            case 0x5555:	/*判据周期设置*/
                n_ui16VarT5 = g_ui16MoniCmdBuff[XcTemp1][3];	/*获取判据周期1设置值*/
                n_ui16VarT6 = g_ui16MoniCmdBuff[XcTemp1][4];	/*获取判据周期2设置值*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 40);	/*预留+判据周期1*/
                if(n_ui16VarT5 >= 0x0001)	/*判据周期1设置值有效*/
                {
                    n_ui32VarT2 = (n_ui32VarT2 & 0xFFFF0000) | n_ui16VarT5;
                    outpDw((n_ui32VarT1 + 40), n_ui32VarT2);
                    g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                }
                else
                {
                    g_ui8MoniItemOperateStatus = 0xA2;	/*监视项操作状态-监控项ID已找到，判据周期1设置值错误*/
                }
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 44);	/*判据周期2+动作1ID*/
                if(n_ui16VarT6 >= 0x0001)	/*判据周期2设置值有效*/
                {
                    n_ui32VarT2 = (n_ui32VarT2 & 0x0000FFFF) | (((uint32_t)n_ui16VarT6) << 16);
                    outpDw((n_ui32VarT1 + 44), n_ui32VarT2);
                    g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                }
                else
                {
                    g_ui8MoniItemOperateStatus = 0xA3;	/*监视项操作状态-监控项ID已找到，判据周期2设置值错误*/
                }
                break;
            case 0x6666:	/*动作ID设置*/
                n_ui16VarT5 = g_ui16MoniCmdBuff[XcTemp1][3];	/*获取动作1ID设置值*/
                n_ui16VarT6 = g_ui16MoniCmdBuff[XcTemp1][4];	/*获取动作2ID设置值*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 44);	/*判据周期2+动作1ID*/
                n_ui32VarT2 = (n_ui32VarT2 & 0xFFFF0000) | n_ui16VarT5;
                outpDw((n_ui32VarT1 + 44), n_ui32VarT2);
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 48);	/*动作2ID+动作后状态1ID*/
                n_ui32VarT2 = (n_ui32VarT2 & 0x0000FFFF) | (((uint32_t)n_ui16VarT6) << 16);
                outpDw((n_ui32VarT1 + 48), n_ui32VarT2);
                g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                break;
            case 0x7777:	/*动作后状态ID设置*/
                n_ui16VarT5 = g_ui16MoniCmdBuff[XcTemp1][3];	/*获取动作后状态1ID设置值*/
                n_ui16VarT6 = g_ui16MoniCmdBuff[XcTemp1][4];	/*获取动作后状态2ID设置值*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 48);	/*动作2ID+动作后状态1ID*/
                n_ui32VarT2 = (n_ui32VarT2 & 0xFFFF0000) | n_ui16VarT5;
                outpDw((n_ui32VarT1 + 48), n_ui32VarT2);
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 52);	/*动作后状态2ID+动作计数*/
                n_ui32VarT2 = (n_ui32VarT2 & 0x0000FFFF) | (((uint32_t)n_ui16VarT6) << 16);
                outpDw((n_ui32VarT1 + 52), n_ui32VarT2);
                g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                break;
            case 0x8888:	/*监视条件设置*/
                n_ui16VarT5 = g_ui16MoniCmdBuff[XcTemp1][6];	/*获取监视条件1设置值-目前支持2字节*/
                n_ui16VarT6 = g_ui16MoniCmdBuff[XcTemp1][10];	/*获取监视条件2设置值*/
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 24);		/*监视条件1*/
                n_ui32VarT2 = (n_ui32VarT2 & 0xFFFF0000) | n_ui16VarT5;
                if(n_ui16VarT5 != 0xFFFF)
                {
                    outpDw((n_ui32VarT1 + 24), n_ui32VarT2);
                }
                n_ui32VarT2 = inpDw(n_ui32VarT1 + 32);		/*监视条件2*/
                n_ui32VarT2 = (n_ui32VarT2 & 0xFFFF0000) | n_ui16VarT6;
                if(n_ui16VarT6 != 0xFFFF)
                {
                    outpDw((n_ui32VarT1 + 32), n_ui32VarT2);
                }
                g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                break;
            case 0x9999:	/*预留*/

                g_ui8MoniItemOperateStatus = 0xAA;	/*监视项操作状态-监控项ID已找到，控制指令执行成功*/
                break;
            default:
                g_ui8MoniItemOperateStatus = 0x99;	/*监视项操作状态-监控项ID已找到，控制指令类型未知，执行失败*/
                break;
        }
    }
    else
    {
        g_ui8MoniItemOperateStatus = 0x88;	/*监视项操作状态-监控项ID未找到，控制指令执行失败*/
    }
}


/*参数监视表监控项增加函数，从监控参数数据注入缓存区提取出来，写入监控参数表缓存区
**输入参数XcTemp1，在线监控指令序号
**输出参数，无
**说明:
*/
void fMonitorParaMItermAdd(uint8_t XcTemp1)
{

    uint32_t n_ui32VarT1Add = 0;		/*4字节局部变量-遍历参数监控表*/
    uint32_t n_ui32VarT2 = 0;		/*4字节局部变量-可新增监视项的偏移地址*/

    uint16_t n_ui16VarTemp = 0;	/*2字节局部变量*/
    uint16_t n_ui16VarTemp2 = 0;	/*2字节局部变量*/
    uint16_t n_ui16VarT1 = 0;		/*2字节局部变量-已有监视项ID*/
    uint16_t n_ui16VarT2 = 0;		/*2字节局部变量-新增监视项ID*/

    uint8_t n_ui8VarTemp = 0;		/*1字节局部变量*/
    uint8_t n_ui8VarT1 = 0;		/*1字节局部变量-参数监控表索引序号*/

    uint8_t n_ui8VarF1 = 0x00;		/*1字节局部变量-监视项存在状态*/

    n_ui8VarT1 = XcTemp1;
    /*增加前，先判断参数监视表中目前有无该监控项，根据监视项ID索引*/
    for(n_ui32VarT1Add = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1Add < ((uint32_t)(MONI_TAB_END_ADRS)); )
    {
        n_ui16VarT1 = (uint16_t)(inpDw(n_ui32VarT1Add) & 0x0000FFFF);	/*读取监视项ID*/
        n_ui16VarT2 = g_ui16MoniCmdBuff[n_ui8VarT1][1];				/*需要新增的监视项ID*/

        if(n_ui16VarT1 == n_ui16VarT2)	/*参数监视表中已有该监视项*/
        {
            /*g_usSlowEngBuf[63] = (uint16_t)(n_ui32VarT1Add & 0x0000FFFF);	测试用*/
            /*遥测参数报错，已有该监视项，不能新增*/
            g_ui8MoniItemOperateStatus = 0x11;	/*监视项操作状态-已有该监视项ID，新增失败*/
            n_ui8VarF1 = 0xAA;					/*表明该监视项已有*/
            break; 								/*跳出循环*/
        }
        else
        {
            n_ui8VarF1 = 0x55;	/*表明该监视项没有*/
        }
        n_ui32VarT1Add = n_ui32VarT1Add + MAX_MONI_CMD_LEN;	/*for循环变量递增*/
    }


    if(n_ui8VarF1 == 0x55)	/*表明该监视项目前不存在，可以新增*/
    {
        /*遍历参数监控表，查找可以新增监控项的位置*/
        for(n_ui32VarT1Add = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1Add < ((uint32_t)(MONI_TAB_END_ADRS)); )
        {
            n_ui16VarT1 = (uint16_t)((inpDw(n_ui32VarT1Add) & 0xFFFF0000)>>16);	/*读取监控项状态*/
            if(n_ui16VarT1 != 0xAA00)	/*该位置可以写入新的监控项*/
            {
                n_ui32VarT2 = n_ui32VarT1Add;
                break;
            }
            else						/*该位置已有监控项，不可以写入新的监控项*/
            {
                n_ui32VarT2 = 0;
                n_ui32VarT1Add = n_ui32VarT1Add + MAX_MONI_CMD_LEN;	/*for循环变量递增*/
            }
        }

        if(n_ui32VarT2 == 0)	/*参数监控表已满，不能新增*/
        {
            /*遥测参数报错，参数监控表已满，不能新增*/
            g_ui8MoniItemOperateStatus = 0x22;	/*监视项操作状态-参数监控表已满，新增失败*/
        }
        else					/*可以新增，已确定新增位置n_ui32VarT2*/
        {
            n_ui16VarTemp = g_ui16MoniCmdBuff[n_ui8VarT1][1];	/*监控项ID*/
            outpDw(n_ui32VarT2, 0xAA000000 | n_ui16VarTemp); 	/*写第1，2个字*/
            /*写第2~32个字*/
            for(n_ui8VarTemp = 2; n_ui8VarTemp < MAX_MONI_CMD_LEN;) /*写入参数监控表*/
            {
                n_ui16VarTemp = g_ui16MoniCmdBuff[n_ui8VarT1][n_ui8VarTemp];	/*第一个字*/
                n_ui16VarTemp2 = g_ui16MoniCmdBuff[n_ui8VarT1][n_ui8VarTemp+1];	/*第二个字*/
                outpDw((n_ui32VarT2 + n_ui8VarTemp*2), (((uint32_t)n_ui16VarTemp)<<16) | n_ui16VarTemp2); /*注意偏移地址倍数*/
                n_ui8VarTemp = n_ui8VarTemp + 2;
            }
            /*y_ui8Cnt_MoniCurrent++; 						更新监控表中目前的监控项数*/
            g_ui16MoniTableNum = g_ui16MoniTableNum + 1;	/*在线监控表条数*/
            g_ui16MoniParaCnt = g_ui16MoniParaCnt + 1;		/*TJSG增加，在线监控监视参数个数*/
        }
    }
}

/**参数监控表监控项根据监控项ID删除函数
**输入参数，监控项ID
**输出参数，无
**说明:
**/
void fMonitorParaMItermDel(uint16_t n_ui16VarTempXc)
{
    uint16_t n_ui16VarTemp = 0;	/*2字节局部变量-已有监控项ID*/
    uint8_t n_ui8VarF1 = 0x00;	/*1字节局部变量*/
    uint8_t n_ui8VarT1 = 0;		/*1字节局部变量*/

    uint32_t n_ui32VarT1 = 0;		/*4字节局部变量-遍历参数监控表*/

    if(n_ui16VarTempXc == 0xFFFF)	/*监控项存储区全清0*/
    {
        for(n_ui32VarT1 = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1 < ((uint32_t)(MONI_TAB_END_ADRS)); )
        {
            outpDw(n_ui32VarT1, 0);
            n_ui32VarT1 = n_ui32VarT1 + MAX_MONI_CMD_LEN;	/*只清前2字*/
            //fWatchDog();
        }
        g_ui8MoniItemOperateStatus = 0x77;	/*监视项操作状态-监控项存储区全部清0成功*/
        g_ui16MoniTableNum = 0;				/*在线监控表条数*/
        g_ui16MoniParaCnt = 0;		/*TJSG增加，在线监控监视参数个数*/
    }
    else
    {
        /*遍历监控表中所有监控项，查找对应监控项ID*/
        for(n_ui32VarT1 = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1 < ((uint32_t)(MONI_TAB_END_ADRS)); )
        {
            n_ui16VarTemp = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);	/*已有监控项ID*/
            if(n_ui16VarTemp == n_ui16VarTempXc)
            {
                /*将该条监控项清0*/
                for(n_ui8VarT1=0; n_ui8VarT1<(MAX_MONI_CMD_LEN/2); n_ui8VarT1++)
                {
                    outpDw((n_ui32VarT1 + n_ui8VarT1*4), 0);
                }
                n_ui8VarF1 = 0xAA; /*已找到要删除的监控项ID*/
                break;
            }
            else
            {
                n_ui8VarF1 = 0x55; /*未找到要删除的监控项ID*/
                n_ui32VarT1 = n_ui32VarT1 + MAX_MONI_CMD_LEN;	/*for循环变量递增*/
            }
            //fWatchDog();
        }

        if(n_ui8VarF1 == 0xAA)	/*已找到要删除的监控项ID*/
        {
            g_ui8MoniItemOperateStatus = 0x33;	/*监视项操作状态-监控项ID已找到，删除成功*/
            /*y_ui8Cnt_MoniCurrent--; 更新监控表中目前的监控项数*/
            if(g_ui16MoniTableNum > 0)
            {
                g_ui16MoniTableNum = g_ui16MoniTableNum - 1;	/*在线监控表条数*/
            }
            if(g_ui16MoniParaCnt > 0)
            {
                g_ui16MoniParaCnt = g_ui16MoniParaCnt - 1;		/*TJSG增加，在线监控监视参数个数*/
            }
        }
        else
        {
            g_ui8MoniItemOperateStatus = 0x44;	/*监视项操作状态-监控项ID未找到，删除失败*/
        }
    }
}


/*参数监控表监视项修改函数
***输入参数n_ui16VarT1Xc:要修改的监控项ID，XcTemp1:要修改的监控项内容所在二维数组行号
***输出参数，无
****/
void fMonitorParaMItermUpdate(uint16_t n_ui16VarT1Xc, uint8_t XcTemp1)
{
    uint16_t n_ui16VarTemp = 0;	/*2字节局部变量-已有监控项ID*/
    uint16_t n_ui16VarT1 = 0;		/*2字节局部变量-*/
    uint16_t n_ui16VarT2 = 0;		/*2字节局部变量-*/
    uint8_t n_ui8VarF1 = 0x00;	/*1字节局部变量-状态标记*/
    uint8_t n_ui8VarT1 = 0;		/*1字节局部变量-循环变量*/

    uint32_t n_ui32VarT1 = 0;		/*4字节局部变量-遍历参数监控表*/

    /*遍历监控表中所有监控项，查找对应监控项ID*/
    for(n_ui32VarT1 = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1 < ((uint32_t)(MONI_TAB_END_ADRS)); )
    {
        n_ui16VarTemp = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);	/*已有监控项ID*/
        if(n_ui16VarTemp == n_ui16VarT1Xc)
        {
            n_ui16VarT1 = g_ui16MoniCmdBuff[XcTemp1][1];
            /*将该条监控项更新*/
            outpDw(n_ui32VarT1, (0xAA000000 | n_ui16VarT1)); 	/*再写一遍监控项状态*/
            /*写第2~32个字*/
            for(n_ui8VarT1 = 2; n_ui8VarT1 < MAX_MONI_CMD_LEN;) /*写入参数监控表*/
            {
                n_ui16VarT1 = g_ui16MoniCmdBuff[XcTemp1][n_ui8VarT1];
                n_ui16VarT2 = g_ui16MoniCmdBuff[XcTemp1][n_ui8VarT1+1];
                outpDw((n_ui32VarT1 + n_ui8VarT1*2), ((((uint32_t)n_ui16VarT1)<<16) | n_ui16VarT2));
                n_ui8VarT1 = n_ui8VarT1 + 2;
            }
            n_ui8VarF1 = 0xAA; /*已找到要更新的监控项ID*/
            break;
        }
        else
        {
            n_ui8VarF1 = 0x55; /*未找到要更新的监控项ID*/
            n_ui32VarT1 = n_ui32VarT1 + MAX_MONI_CMD_LEN;	/*for循环变量递增*/
        }
    }

    if(n_ui8VarF1 == 0xAA)	/**/
    {
        g_ui8MoniItemOperateStatus = 0x55;	/*监视项操作状态-监控项ID已找到，更新成功*/
    }
    else
    {
        g_ui8MoniItemOperateStatus = 0x66;	/*监视项操作状态-监控项ID未找到，更新失败*/
    }
}

/*******************************************************************
功能：在线监控主函数
主要思路：1s周期查询执行
********************************************************************/
void fMonitorAI(void)
{
    uint16_t n_ui16VarT1 = 0;	/*2字节变量-监控项第1个字*/
    uint16_t n_ui16VarT2 = 0;	/*2字节变量-监控项第4个字*/
    uint16_t n_ui16VarT3 = 0;	/*2字节变量-转换公式*/
    /*uint16_t n_ui16VarT4 = 0; 2字节变量-双字第2个字*/
    uint16_t n_ui16VarT5 = 0;	/*监视周期*/
    uint16_t n_ui16VarT6 = 0;	/*监视周期计数器*/
    uint16_t n_ui16VarT7 = 0;	/*2字节监视参数当前值*/
    uint16_t n_ui16VarT8 = 0;	/*2字节监视条件1当前值*/
    uint16_t n_ui16VarT9 = 0; /*2字节监视条件2当前值*/
    uint16_t n_ui16VarT10 = 0;	/*2字节判据周期1当前值*/
    uint16_t n_ui16VarT11 = 0; 	/*2字节判据周期2当前值*/
    uint16_t n_ui16VarT12 = 0;	/*2字节判据周期1-超限倒计数当前值*/
    uint16_t n_ui16VarT13 = 0; 	/*2字节判据周期2-超限倒计数当前值*/
    uint16_t n_ui16VarT14 = 0;	/*2字节动作1ID当前值*/
    uint16_t n_ui16VarT15 = 0; 	/*2字节动作2ID当前值*/
    uint16_t n_ui16VarT16 = 0; 	/*2字节-下一个监视项ID*/

    uint8_t n_ui8VarT1 = 0;	/*监视参数所在遥测数组索引值*/

    uint32_t n_ui32VarT1 = 0; /*4字节变量-遍历参数监控表for循环*/
    uint32_t n_ui32VarT2 = 0; /*4字节变量-参数位置4字节*/
    uint32_t n_ui32VarT3 = 0; /*4字节变量-*/

    /*遍历参数监控表中所有监控项，提取需要进行监控的项进行监控操作*/
    for(n_ui32VarT1 = ((uint32_t)(MONI_TAB_STR_ADRS)); n_ui32VarT1 < ((uint32_t)(MONI_TAB_END_ADRS)); )
    {
        //fWatchDog();
        n_ui16VarT1 = (uint16_t)((inpDw(n_ui32VarT1) & 0xFFFF0000) >> 16);/*监控项第1个字，参数监控项状态+预留00H*/
        n_ui16VarT2 = (uint16_t)(inpDw(n_ui32VarT1 + 4) & 0x0000FFFF);	/*监控项第4个字,高字节为"状态设置"，低字节为"参数长度-字节数"*/

        if(((n_ui16VarT1 & 0xFF00) == 0xAA00) && ((n_ui16VarT2 & 0x0100) == 0x0100))/*有监控项且已使能监视*/
        {
            n_ui16VarT5 = (uint16_t)(inpDw(n_ui32VarT1 + 36) & 0x0000FFFF);	/*监视周期*/
            n_ui16VarT6 = (uint16_t)(inpDw(n_ui32VarT1 + 60) & 0x0000FFFF);	/*监视周期计数器*/

            if(n_ui16VarT5 <= n_ui16VarT6)	/*监视周期到*/
            {
                //fWatchDog();
                n_ui32VarT3 = inpDw(n_ui32VarT1 + 60);		/*报告ID+监视周期计数器*/
                n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000);
                outpDw((n_ui32VarT1 + 60), n_ui32VarT3);	/*监视周期计数器清0*/

                n_ui32VarT2 = inpDw(n_ui32VarT1 + 8);	/*参数位置-目前是遥测参数偏移量-从0开始计*/
                if((n_ui16VarT2 & 0x00FF) == 2)			/*参数长度2字节*/
                {
                    /*获取监视参数当前值n_ui16VarT7*/
                    if((n_ui32VarT2 % 2) == 0)	/*2字节参数位置起始字节为偶数*/
                    {
                        n_ui8VarT1 = (uint8_t)(n_ui32VarT2 / 2);	/*监视参数所在遥测数组索引值*/
                        n_ui16VarT7 = g_usSlowEngBuf[n_ui8VarT1];
                    }
                    else
                    {
                        n_ui8VarT1 = (uint8_t)((n_ui32VarT2 - 1) / 2);	/*监视参数所在遥测数组索引值*/
                        n_ui16VarT7 = ((g_usSlowEngBuf[n_ui8VarT1] & 0x00FF) << 8) | ((g_usSlowEngBuf[n_ui8VarT1+1] & 0xFF00) >> 8);  /*2字节参数值*/
                    }
                }
                else if((n_ui16VarT2 & 0x00FF) == 1) /*参数长度1字节*/
                {
                    /*获取监视参数当前值n_ui16VarT7*/
                    if((n_ui32VarT2 % 2) == 0)	/*2字节参数位置起始字节为偶数*/
                    {
                        n_ui8VarT1 = (uint8_t)(n_ui32VarT2 / 2);	/*监视参数所在遥测数组索引值*/
                        n_ui16VarT7 = (g_usSlowEngBuf[n_ui8VarT1] & 0xFF00) >>8 ; /*单字节参数XXH表示成双字节00XXH*/
                    }
                    else
                    {
                        n_ui8VarT1 = (uint8_t)((n_ui32VarT2 - 1) / 2);	/*监视参数所在遥测数组索引值*/
                        n_ui16VarT7 = g_usSlowEngBuf[n_ui8VarT1] & 0x00FF;  /*单字节参数XXH表示成双字节00XXH*/
                    }
                }
                else	/*其它情况-待实现*/
                {


                }

                /*监视参数采集值按照转换公式，转换为判据使用值*/
                n_ui16VarT3 = (uint16_t)((inpDw(n_ui32VarT1 + 36) & 0xFFFF0000) >> 16);
                switch(n_ui16VarT3)
                {
                    case 0x1111:

                        break;
                    case 0x2222:	/*转台角度专用公式*/
                        n_ui16VarT7 = (n_ui16VarT7 / 100);
                        if((n_ui16VarT7 > 0) && (n_ui16VarT7 < 360))	/*确保采集到的判据值为有效值*/
                        {
                            if((n_ui16VarT7 > 180))
                            {
                                n_ui16VarT7 = 360 - n_ui16VarT7;
                            }
                        }
                        else
                        {
                            n_ui16VarT7 = 0;
                        }
                        break;
                    case 0x3333:

                        break;

                }

                //fWatchDog();
                /*获取监视条件*/
                n_ui16VarT8 = (uint16_t)(inpDw(n_ui32VarT1 + 24) & 0x0000FFFF); /*监视条件1-下限值*/
                n_ui16VarT9 = (uint16_t)(inpDw(n_ui32VarT1 + 32) & 0x0000FFFF); /*监视条件2-上限值*/
                /*获取判据周期*/
                n_ui16VarT10 = (uint16_t)(inpDw(n_ui32VarT1 + 40) & 0x0000FFFF); 			/*判据周期1*/
                n_ui16VarT11 = (uint16_t)((inpDw(n_ui32VarT1 + 44) & 0xFFFF0000) >> 16); 	/*判据周期2*/
                /*获取判据周期1-超限倒计数*/
                n_ui16VarT12 = (uint16_t)((inpDw(n_ui32VarT1 + 56) & 0xFFFF0000) >> 16); 	/*判据周期1-超限倒计数*/
                /*获取判据周期2-超限倒计数*/
                n_ui16VarT13 = (uint16_t)(inpDw(n_ui32VarT1 + 56) & 0x0000FFFF); 			/*判据周期2-超限倒计数*/

                /*判断参数是否超限*/
                if((n_ui16VarT2 & 0x0C00) == 0x0000)	/*监视条件类型-范围*/
                {
                    g_ui16MoniParaCurrVal = n_ui16VarT7;	/*测试语句-打印监控参数当前值*/

                    if((n_ui16VarT8 != 0) && (n_ui16VarT7 < n_ui16VarT8))	/*有下限值且监视参数小于下限值*/
                    {
                        if(n_ui16VarT12 == 0)	/*判据周期1-超限倒计数为0-触发监控动作*/
                        {
                            //fWatchDog();
                            n_ui16VarT14 = (uint16_t)(inpDw(n_ui32VarT1 + 44) & 0x0000FFFF);	/*动作1ID*/
                            if((n_ui16VarT14 & 0xC000) == 0x0000)	/*高2bit为00b-直接输出执行的动作*/
                            {
                                /*输出动作ID*/
                                if(g_ui32MoniOutAddrWrite%4 == 0)	/*地址是4的倍数*/
                                {
                                    n_ui32VarT3 = inpDw(g_ui32MoniOutAddrWrite);
                                    n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)n_ui16VarT14) << 16);
                                    outpDw(g_ui32MoniOutAddrWrite, n_ui32VarT3);
                                }
                                else
                                {
                                    n_ui32VarT3 = inpDw(g_ui32MoniOutAddrWrite - 2);
                                    n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | n_ui16VarT14;
                                    outpDw((g_ui32MoniOutAddrWrite - 2), n_ui32VarT3);
                                }

                                g_ui32MoniOutAddrWrite = g_ui32MoniOutAddrWrite + 2;
                                if(g_ui32MoniOutAddrWrite == MONI_OUT_END_ADRS)
                                {
                                    g_ui32MoniOutAddrWrite = MONI_OUT_STR_ADRS;	/*动作ID输出，循环缓存*/
                                }
                            }
                            else if((n_ui16VarT14 & 0xC000) == 0xC000)	/*高2bit为11b-使能下一个监视项的动作*/
                            {
                                n_ui16VarT16 = n_ui16VarT14 & 0x3FFF;	/*下一个监视项ID*/
                                fMonitorParaMItermEn(n_ui16VarT16);	/*使能监视项-根据监视项ID*/
                            }
                            else /*待扩展*/
                            {}

                            /*禁止该监控项*/
                            n_ui32VarT3 = (inpDw(n_ui32VarT1 + 4) & 0xFFFFFEFF);
                            outpDw((n_ui32VarT1 + 4), n_ui32VarT3);	/*禁止该监控项*/

                            g_ui16MoniParaActionSum = g_ui16MoniParaActionSum + 1;	/*TJSG增加，在线监控监视参数动作总计数*/
                            g_ui8MoniParaUnExeActionNum = g_ui8MoniParaUnExeActionNum + 1;	/*在线监控监视参数待执行动作计数*/
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 52);
                            n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | (((inpDw(n_ui32VarT1 + 52) & 0x0000FFFF) + 1) & 0x0000FFFF);
                            outpDw((n_ui32VarT1 + 52), n_ui32VarT3);	/*更新监视项动作计数*/

                            /*判据周期1-超限倒计数 恢复为判据周期1*/
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                            n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)n_ui16VarT10) << 16);
                            outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期1-超限倒计数 恢复为 判据周期1*/

                            /*写遥测参数*/
                            g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);	/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                            g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                            g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);			/*TJSG增加，在线监控动作计数*/
                            g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);/*TJSG增加，在线监控超限1倒计数轮询参数*/
                            g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);	/*TJSG增加，在线监控超限2倒计数轮询参数*/

                        }
                        else	/*判据周期1-超限倒计数 - 1*/
                        {
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                            n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)(n_ui16VarT12 - 1)) << 16);
                            outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期1-超限倒计数 - 1*/
                        }
                    }
                    else if((n_ui16VarT8 != 0) && (n_ui16VarT7 >= n_ui16VarT8)) /*有下限值且监视参数不小于下限值*/
                    {
                        /*判据周期1-超限倒计数 恢复为判据周期1*/
                        n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                        n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)n_ui16VarT10) << 16);
                        outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期1-超限倒计数 恢复为 判据周期1*/

                        /*写遥测参数*/
                        g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);				/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                        g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                        g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);			/*TJSG增加，在线监控动作计数*/
                        g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);/*TJSG增加，在线监控超限1倒计数轮询参数*/
                        g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);	/*TJSG增加，在线监控超限2倒计数轮询参数*/

                    }
                    else	/*待扩展*/
                    {

                    }


                    if((n_ui16VarT9 != 0xFFFF) && (n_ui16VarT7 > n_ui16VarT9))	/*有上限值且监视参数大于上限值*/
                    {

                        if(n_ui16VarT13 == 0)	/*触发监控动作*/
                        {
                            //fWatchDog();
                            n_ui16VarT15 = (uint16_t)((inpDw(n_ui32VarT1 + 48) & 0xFFFF0000) >> 16);	/*动作2ID*/
                            if((n_ui16VarT15 & 0xC000) == 0x0000)	/*高2bit为00b-直接输出执行的动作*/
                            {
                                /*输出动作ID*/
                                if(g_ui32MoniOutAddrWrite%4 == 0)	/*地址是4的倍数*/
                                {
                                    n_ui32VarT3 = inpDw(g_ui32MoniOutAddrWrite);
                                    n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)n_ui16VarT15) << 16);
                                    outpDw(g_ui32MoniOutAddrWrite, n_ui32VarT3);
                                }
                                else
                                {
                                    n_ui32VarT3 = inpDw(g_ui32MoniOutAddrWrite - 2);
                                    n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | n_ui16VarT15;
                                    outpDw((g_ui32MoniOutAddrWrite - 2), n_ui32VarT3);
                                }

                                g_ui32MoniOutAddrWrite = g_ui32MoniOutAddrWrite + 2;
                                if(g_ui32MoniOutAddrWrite == MONI_OUT_END_ADRS)
                                {
                                    g_ui32MoniOutAddrWrite = MONI_OUT_STR_ADRS;	/*动作ID输出，循环缓存*/
                                }
                            }
                            else if((n_ui16VarT15 & 0xC000) == 0xC000)	/*高2bit为11b-使能下一个监视项的动作*/
                            {
                                n_ui16VarT16 = n_ui16VarT15 & 0x3FFF;	/*下一个监视项ID*/
                                fMonitorParaMItermEn(n_ui16VarT16);	/*使能监视项-根据监视项ID*/
                            }
                            else /*待扩展*/
                            {}

                            /*禁止该监控项*/
                            n_ui32VarT3 = (inpDw(n_ui32VarT1 + 4) & 0xFFFFFEFF);
                            outpDw((n_ui32VarT1 + 4), n_ui32VarT3);	/*禁止该监控项*/

                            g_ui16MoniParaActionSum = g_ui16MoniParaActionSum + 1;	/*TJSG增加，在线监控监视参数动作总计数*/
                            g_ui8MoniParaUnExeActionNum = g_ui8MoniParaUnExeActionNum + 1;	/*在线监控监视参数待执行动作计数*/
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 52);
                            n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | (((inpDw(n_ui32VarT1 + 52) & 0x0000FFFF) + 1) & 0x0000FFFF);
                            outpDw((n_ui32VarT1 + 52), n_ui32VarT3);	/*更新监视项动作计数*/

                            /*判据周期2-超限倒计数 恢复为 判据周期2*/
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                            n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | ((inpDw(n_ui32VarT1 + 44) & 0xFFFF0000) >> 16);
                            outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期2-超限倒计数 恢复为 判据周期2*/

                            /*写遥测参数*/
                            g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);				/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                            g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                            g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);					/*TJSG增加，在线监控动作计数*/
                            g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);	/*TJSG增加，在线监控超限1倒计数轮询参数*/
                            g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);			/*TJSG增加，在线监控超限2倒计数轮询参数*/

                        }
                        else
                        {
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                            n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | (n_ui16VarT13 - 1);
                            outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期2-超限倒计数 - 1*/

                            /*写遥测参数*/
                            g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);				/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                            g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                            g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);					/*TJSG增加，在线监控动作计数*/
                            g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);	/*TJSG增加，在线监控超限1倒计数轮询参数*/
                            g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);			/*TJSG增加，在线监控超限2倒计数轮询参数*/

                        }
                    }
                    else if((n_ui16VarT9 != 0xFFFF) && (n_ui16VarT7 <= n_ui16VarT9))	/*有上限值且监视参数不大于上限值*/
                    {

                        /*判据周期2-超限倒计数 恢复为 判据周期2*/
                        n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                        n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | ((inpDw(n_ui32VarT1 + 44) & 0xFFFF0000) >> 16);
                        outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期2-超限倒计数 恢复为 判据周期2*/

                        /*写遥测参数*/
                        g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);				/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                        g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                        g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);			/*TJSG增加，在线监控动作计数*/
                        g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);/*TJSG增加，在线监控超限1倒计数轮询参数*/
                        g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);	/*TJSG增加，在线监控超限2倒计数轮询参数*/

                    }
                    else	/*待扩展*/
                    {

                    }
                }
                else if((n_ui16VarT2 & 0x0C00) == 0x0400)	/*监视条件类型-期望值*/
                {
                    if(n_ui16VarT7 == n_ui16VarT8)	/*监视参数等于期望值*/
                    {
                        if(n_ui16VarT12 == 0)	/*触发监控动作*/
                        {
                            //fWatchDog();
                            n_ui16VarT14 = (uint16_t)(inpDw(n_ui32VarT1 + 44) & 0x0000FFFF);	/*动作1ID*/
                            if((n_ui16VarT14 & 0xC000) == 0x0000)	/*高2bit为00b-直接输出执行的动作*/
                            {
                                /*输出动作ID*/
                                if(g_ui32MoniOutAddrWrite%4 == 0)	/*地址是4的倍数*/
                                {
                                    n_ui32VarT3 = inpDw(g_ui32MoniOutAddrWrite);
                                    n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)n_ui16VarT14) << 16);
                                    outpDw(g_ui32MoniOutAddrWrite, n_ui32VarT3);
                                }
                                else
                                {
                                    n_ui32VarT3 = inpDw(g_ui32MoniOutAddrWrite - 2);
                                    n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | n_ui16VarT14;
                                    outpDw((g_ui32MoniOutAddrWrite - 2), n_ui32VarT3);
                                }

                                g_ui32MoniOutAddrWrite = g_ui32MoniOutAddrWrite + 2;
                                if(g_ui32MoniOutAddrWrite == MONI_OUT_END_ADRS)
                                {
                                    g_ui32MoniOutAddrWrite = MONI_OUT_STR_ADRS;	/*动作ID输出，循环缓存*/
                                }
                            }
                            else if((n_ui16VarT14 & 0xC000) == 0xC000)	/*高2bit为11b-使能下一个监视项的动作*/
                            {
                                n_ui16VarT16 = n_ui16VarT14 & 0x3FFF;	/*下一个监视项ID*/
                                fMonitorParaMItermEn(n_ui16VarT16);	/*使能监视项-根据监视项ID*/
                            }
                            else /*待扩展*/
                            {}

                            /*禁止该监控项*/
                            n_ui32VarT3 = (inpDw(n_ui32VarT1 + 4) & 0xFFFFFEFF);
                            outpDw((n_ui32VarT1 + 4), n_ui32VarT3);	/*禁止该监控项*/

                            g_ui16MoniParaActionSum = g_ui16MoniParaActionSum + 1;	/*TJSG增加，在线监控监视参数动作总计数*/
                            g_ui8MoniParaUnExeActionNum = g_ui8MoniParaUnExeActionNum + 1;	/*在线监控监视参数待执行动作计数*/
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 52);
                            n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | (((inpDw(n_ui32VarT1 + 52) & 0x0000FFFF) + 1) & 0x0000FFFF);
                            outpDw((n_ui32VarT1 + 52), n_ui32VarT3);	/*更新监视项动作计数*/

                            /*判据周期1-超限倒计数 恢复为判据周期1*/
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                            n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | ((inpDw(n_ui32VarT1 + 40) & 0x0000FFFF) << 16);
                            outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期1-恢复为判据周期1*/

                            /*写遥测参数*/
                            g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);				/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                            g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                            g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);					/*TJSG增加，在线监控动作计数*/
                            g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);	/*TJSG增加，在线监控超限1倒计数轮询参数*/
                            g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);			/*TJSG增加，在线监控超限2倒计数轮询参数*/
                        }
                        else
                        {
                            n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                            n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)(n_ui16VarT12 - 1)) << 16);
                            outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期1-超限倒计数 - 1*/

                            /*写遥测参数*/
                            g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);				/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                            g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                            g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);					/*TJSG增加，在线监控动作计数*/
                            g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);	/*TJSG增加，在线监控超限1倒计数轮询参数*/
                            g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);			/*TJSG增加，在线监控超限2倒计数轮询参数*/
                        }
                    }
                    else	/*监视参数不等于期望值*/
                    {

                        /*判据周期1-超限倒计数 恢复为判据周期1*/
                        n_ui32VarT3 = inpDw(n_ui32VarT1 + 56);
                        n_ui32VarT3 = (n_ui32VarT3 & 0x0000FFFF) | (((uint32_t)n_ui16VarT10) << 16);
                        outpDw((n_ui32VarT1 + 56), n_ui32VarT3);  /*判据周期1-超限倒计数 恢复为 判据周期1*/

                        /*写遥测参数*/
                        g_ui16MoniParaNum = (uint16_t)(inpDw(n_ui32VarT1) & 0x0000FFFF);				/*TJSG增加，在线监控监视参数序号轮询-监视项ID*/
                        g_ui8MoniStatusPara = (uint8_t)((inpDw(n_ui32VarT1 + 4) & 0x0000FF00) >> 8);	/*TJSG增加，在线监控监视状态*/
                        g_ui8MoniActionCnt = (uint8_t)(inpDw(n_ui32VarT1 + 52) & 0x000000FF);			/*TJSG增加，在线监控动作计数*/
                        g_ui8MoniBeyondRemainCnt = (uint8_t)((inpDw(n_ui32VarT1 + 56) & 0x00FF0000) >> 16);	/*TJSG增加，在线监控超限1倒计数轮询参数*/
                        g_ui8MoniBeyondRemainCnt2 = (uint8_t)(inpDw(n_ui32VarT1 + 56) & 0x000000FF);	/*TJSG增加，在线监控超限2倒计数轮询参数*/
                    }
                }
                else	/*监视条件类型-变化量-待扩展*/
                {

                }

            }
            else							/*监视周期未到*/
            {
                n_ui32VarT3 = inpDw(n_ui32VarT1 + 60);		/*报告ID+监视周期计数器*/
                n_ui32VarT3 = (n_ui32VarT3 & 0xFFFF0000) | (n_ui16VarT6 + 1);
                outpDw((n_ui32VarT1 + 60), n_ui32VarT3);	/*监视周期计数器+1*/
            }
        }

        n_ui32VarT1 = n_ui32VarT1 + MAX_MONI_CMD_LEN;
    }
}


/********************************************************************
功能：		在线监控入口
参数：		......
（入口）	......
（入口）	......
（出口）	......
返回值：	......
主要思路：1s周期执行一次
调用方法：......
编写日期：......
修改记录：......
********************************************************************/
void fMonitor(void)
{
    uint8_t n_ui8Var1;	/*1字节变量-数据注入操作类型*/
    uint8_t n_ui8Var2;	/*1字节变量-遍历在线监控指令缓存标记数组*/
    uint8_t n_ui8Var3;	/**/

    uint16_t n_ui16Var1;	/*2字节变量-监控项ID*/
    uint32_t n_ui32VarfMT1;	/**/
    uint32_t n_ui32VarfMT2;	/**/
    uint32_t n_ui32VarfMT3 = 0;	/**/


    unsigned int   p_uiUt[3];	/*测试用*/

    n_ui8Var3 = g_ucSysTimeFlag;
    g_ucSysTimeFlag = 0x01;
    //fGetUserTime(p_uiUt);/* 获取当前时间码 */
    n_ui32VarfMT1 = p_uiUt[1];	/*亚秒（25us）计数*/

    //fWatchDog();

    /*在线监控主函数*/
    fMonitorAI();

    //fWatchDog();
    /*遍历在线监控指令缓存标记数组*/
    for(n_ui8Var2 = 0; n_ui8Var2 < MAX_IMM_CMD_NUM; n_ui8Var2++)
    {
        if(g_ui8MoniCmdFlag[n_ui8Var2] == 0x55) /*有未处理的数据注入指令*/
        {
            //fWatchDog();
            n_ui8Var1 = (uint8_t)(g_ui16MoniCmdBuff[n_ui8Var2][0] & 0x00FF);	/*获取数据注入操作类型*/
            g_ui8MoniItemOperateStatus = n_ui8Var1; /*数据注入类型*/
            switch(n_ui8Var1)
            {
                case 0x01:	/*监控项整条增加*/
                    fMonitorParaMItermAdd(n_ui8Var2);
                    g_ui8MoniCmdFlag[n_ui8Var2] = 0xAA;  /*该数据注入指令已执行，置指令缓存数组标记*/
                    break;
                case 0x02:	/*根据监控项ID整条删除*/
                    n_ui16Var1 = g_ui16MoniCmdBuff[n_ui8Var2][1];	/*获取监控项ID*/
                    fMonitorParaMItermDel(n_ui16Var1);
                    g_ui8MoniCmdFlag[n_ui8Var2] = 0xAA;  /*该数据注入指令已执行，置指令缓存数组标记*/
                    break;
                case 0x03:	/*根据监控项ID按条修改*/
                    n_ui16Var1 = g_ui16MoniCmdBuff[n_ui8Var2][1];	/*获取监控项ID*/
                    fMonitorParaMItermUpdate(n_ui16Var1, n_ui8Var2);
                    g_ui8MoniCmdFlag[n_ui8Var2] = 0xAA;  /*该数据注入指令已执行，置指令缓存数组标记*/
                    break;
                case 0x04:	/*监控项控制指令*/
                    fMonitorParaMItermCtrl(n_ui8Var2);	/*监控项控制指令执行函数*/
                    g_ui8MoniCmdFlag[n_ui8Var2] = 0xAA;  /*该数据注入指令已执行，置指令缓存数组标记*/
                    break;
                default:
                    g_ui8MoniItemOperateStatus = 0xFF; /*未知数据注入类型*/
                    g_ui8MoniCmdFlag[n_ui8Var2] = 0xAA;  /*该数据注入指令已执行，置指令缓存数组标记*/
                    break;
            }

            break;	/*跳出for循环，每秒最多执行一次数据注入指令*/
        }
    }

    /*更新在线监控待执行动作计数*/
    if(g_ui32MoniOutAddrWrite >= g_ui32MoniOutAddrRead)
    {
        g_ui8MoniParaUnExeActionNum = ((g_ui32MoniOutAddrWrite - g_ui32MoniOutAddrRead) / 2);
    }
    else
    {
        g_ui8MoniParaUnExeActionNum = (((g_ui32MoniOutAddrWrite + 0x1000) - g_ui32MoniOutAddrRead) / 2);
    }
    //fWatchDog();

    //fGetUserTime(p_uiUt);		/* 获取当前时间码 */
    n_ui32VarfMT2 = p_uiUt[1];	/*亚秒（25us）计数*/

    if(n_ui32VarfMT2 >= n_ui32VarfMT1)	/*秒计数没翻转*/
    {
        n_ui32VarfMT3 = n_ui32VarfMT2 - n_ui32VarfMT1;
    }
    else
    {
        n_ui32VarfMT3 = n_ui32VarfMT2 + 0x9C40 - n_ui32VarfMT1;
    }

    if(n_ui32VarfMT3 > g_ui32MoniTaskExeMaxT)
    {
        g_ui32MoniTaskExeMaxT = n_ui32VarfMT3;
    }

    g_usSlowEngBuf[63] = (uint16_t)(g_ui32MoniTaskExeMaxT & 0x0000FFFF); 	/*测试用*/

    g_ucSysTimeFlag = n_ui8Var3;
}





