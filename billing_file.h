#ifndef BILLING_FILE_H
#define BILLING_FILE_H
#include"model.h"

int saveBilling(const Billing* pBilling, const char* pPath); //将计费信息保存到billing.ams文件中
int getBillingCount(const char* pPath);                    // 获取消费信息数量
int readBilling(Billing* pBilling, const char* pPath);     // 读取消费信息到数组

#endif // !BILLING_FILE_H

