//
// Created by kfzjw008 on 2023-05-16.
//

#ifndef SICSA_ONLINEMONITORING_H
#define SICSA_ONLINEMONITORING_H

/*************主要头文件*********************/
#include <stdint.h>
#include<string.h>
#include <stdio.h>
#include"../global.h"
#include <malloc.h>
#include <stdbool.h>

void fMonitorInit(void);
void fMonitor(void);
void fonlineMonitoringMain();
void fonlineMonitoringInit();
void fonlineMonitoringEnd(); // 清理在线监控模块资源
/*UINT8 y_ui8MonitorVar1;			遍历在线监控数据注入数组专用*/
/*UINT8 y_ui8Cnt_MoniCurrent; 	更新监控表中目前的监控项数*/
/*UINT16 y_ui16Addr_MoniActionOut;*/ 	/*动作ID输出偏移地址*/

#endif //SICSA_ONLINEMONITORING_H
