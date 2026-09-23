#ifndef SERVICE_H
#define SERVICE_H

#include"model.h"
#include"global.h"

int addCardInfo(Card card);                           // 添加卡信息
Card* queryCardInfo(const char* pName);               // 精确查询卡信息
Card* queryCardsInfo(const char* pName, int* pIndex); // 模糊查询卡信息
int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo);  // 上机
void releaseList(void);                               // 释放链表内存
double getAmount(time_t tStart);                           // 计算消费金额
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo);  // 下机
int doAddMoney(const char* pName, const char* pPwd, float fAmount, MoneyInfo* pMoneyInfo);    // 充值
int doRefundMoney(const char* pName, const char* pPwd, float fAmount, MoneyInfo* pMoneyInfo);  // 退费
void doStatistics(void);  // 查询统计
int annulCard(const char* pName, const char* pPwd, float* pRefund);  // 注销卡


#endif // !SERVICE_H
