#include"model.h"
#include"global.h"
#include<stdio.h>

// 保存充值退费信息到二进制文件
int saveMoney(const Money* pMoney, const char* pPath) {
    FILE* fp = NULL;

    // 以追加二进制方式打开文件
    errno_t err = fopen_s(&fp, pPath, "ab");
    if (err != 0 || fp == NULL) {
        // 追加失败，尝试创建新文件
        err = fopen_s(&fp, pPath, "wb");
        if (err != 0 || fp == NULL) {
            return FALSE;
        }
    }

    // 将数据写入二进制文件
    fwrite(pMoney, sizeof(Money), 1, fp);

    fclose(fp);
    return TRUE;
}