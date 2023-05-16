//
// Created by kfzjw008 on 2023-05-16.
//

#include "util.h"


void packetHeaderSearch() {}//	包头寻找
void packetFooterSearch() {}//	包尾寻找
void packetHeaderVerification() {}//	包头判断
void packetFooterVerification() {}//	包尾判断
void fixedLengthString() {}//	定长字符串
void addInstruction() {}//	增加指令
void removeInstruction() {}//	删除指令
void setInstructionApplicationProcessIdentifier() {}//	设置指令应用过程识别符
void setInstructionEncoding() {}//	设置指令编码
void setInstructionRouting() {}//	设置指令路由
void uploadEventTableF1() {}//	上注事件表F1
void uploadEventTableF2() {}//	上注事件表F2
void uploadEventTableF3() {}//	上注事件表F3
void appendEventTableF9() {}//	追加事件表F9
void appendEventTableFA() {}//	追加事件表FA
void appendEventTableFB() {}//	追加事件表FB
void deleteEventItem() {}//	删除事件项
void timeShiftEventItem() {}//	时间平移事件项
void enableEventItemExecution() {}//	允许事件项执行
void disableEventItemExecution() {}//	禁止事件项执行
void enableEventItemDeletion() {}//	允许事件项删除
void disableEventItemDeletion() {}//	禁止事件项删除
void enableEventItemTimeShift() {}//	允许事件项时间平移
void disableEventItemTimeShift() {}//	禁止事件项时间平移
void performEventTableMaintenance() {}//	进行事件表维护
void eventExecuted() {}//	事件已执行
void eventInvalidated() {}//	事件已作废
void eventSourceSetting() {}//	事件来源设置
void eventItemVerification() {}//	事件项校验
void eventtableSort() {}//	事件表排序
void macroAllowRelease() {}//	宏允许释放
void macroDisallowRelease() {}//	宏禁止释放
void macroAllowDeletion() {}//	宏允许删除
void macroDisallowDeletion() {}//	宏禁止删除
void deleteMacroInstruction() {}//	删除宏指令
void setMacroInstructionType() {}//	设置宏指令类型
void maintainMacroInstructionTable() {}//	宏指令表维护
void addMonitoringItemDefinition() {}//	增加监视项定义
void deleteMonitoringItemDefinition() {}//	删除监视项定义
void enableParameterMonitoringStatus() {}//	使能参数监视状态
void disableParameterMonitoringStatus() {}//	禁用参数监视状态
void changeParameterID() {}//	改变参数ID
void changeParameterPosition() {}//	改变参数位置
void changeParameterFormat() {}//	改变参数格式
void changeMonitoringCondition1() {}//	改变监视条件1
void changeMonitoringCondition2() {}//	改变监视条件2
void changeConversionFormulaID() {}//	改变转换公式ID
void changeMonitoringPeriod() {}//	改变监视周期
void changeCriterionPeriod1() {}//	改变判据周期1
void changeCriterionPeriod2() {}//	改变判据周期2
void changeAction1ID() {}//	改变动作1ID
void changeAction2ID() {}//	改变动作2ID
void changeStatus1IDafterAction() {}//	改变动作后状态1ID
void changeStatus2IDafterAction() {}//	改变动作后状态2ID
void setMonitoringConditionType() {}//	设置监控条件类型
void setMonitoringPriorityInstruction() {}//	设置监视优先级指令
void setParameterType() {}//	设置参数类型
void enableMonitoringAction() {}//	使能监视动作
void disableMonitoringAction() {}//	禁用监视动作
//到此为止，以上为文档内函数