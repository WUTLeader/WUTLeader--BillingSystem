#include"model.h"
#include<stdio.h>
#include"global.h"


//将计费信息保存到billing.ams文件中
int saveBilling(const Billing* pBilling, const char* pPath) {
    FILE* fp = NULL;

    //打开二进制文件
    errno_t err = fopen_s(&fp, pPath, "ab");
    if (err != 0 || fp == NULL) {
        return FALSE;
    }

    //将数据写进二进制文件
    fwrite(pBilling, sizeof(Billing), 1, fp);

    //关闭文件
    fclose(fp);

    return TRUE;
}

// 获取消费信息数量（二进制文件）
int getBillingCount(const char* pPath) {
    FILE* fp = NULL;
    int nCount = 0;
    Billing billing = { 0 };

    errno_t err = fopen_s(&fp, pPath, "rb");
    if (err != 0 || fp == NULL) {
        return 0;  // 文件不存在或打开失败，返回0
    }

    // 遍历二进制文件，统计记录数量
    while (fread(&billing, sizeof(Billing), 1, fp) == 1) {
        nCount++;
    }

    fclose(fp);
    return nCount;
}

// 读取消费信息到数组（二进制文件）
int readBilling(Billing* pBilling, const char* pPath) {
    FILE* fp = NULL;
    int i = 0;

    errno_t err = fopen_s(&fp, pPath, "rb");
    if (err != 0 || fp == NULL) {
        return FALSE;
    }

    // 读取所有消费信息到数组
    while (fread(&pBilling[i], sizeof(Billing), 1, fp) == 1) {
        i++;
    }

    fclose(fp);
    return TRUE;
}