#ifndef BILLING_SERVICE_H
#define BILLING_SERVICE_H
#include"model.h"

int addBilling(Billing billing);                    // 判断消费信息是否能保存成功
int initBillingList(void);                          // 初始化计费信息链表
void releaseBillingList(void);                      // 释放计费信息链表
int getBilling(void);                               // 从文件获取消费信息到链表
Billing* queryBilling(const char* pName, int* pIndex);  // 根据卡号查询消费信息

// 声明外部变量
extern lpBillingNode billingList;

#endif // !BILLING_SERVICE_H