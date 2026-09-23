#include"model.h"
#include"global.h"
#include"billing_file.h"
#include<stdlib.h>   // 包含 malloc、free 函数
#include<string.h>   // 包含 strcmp 函数
#include<stdio.h>

// 定义全局计费链表头指针
lpBillingNode billingList = NULL;

// 原有函数：添加消费信息（上机时调用）
int addBilling(Billing billing) {
    return saveBilling(&billing, BILLINGPATH);
}

// 新增：初始化计费信息链表（创建头结点）
int initBillingList(void) {
    lpBillingNode head = (lpBillingNode)malloc(sizeof(BillingNode));
    if (head == NULL) {
        return FALSE;
    }
    head->next = NULL;
    billingList = head;
    return TRUE;
}

// 新增：释放计费信息链表（释放所有节点，包括头结点）
void releaseBillingList(void) {
    if (billingList == NULL) {
        return;
    }
    lpBillingNode p = billingList;
    lpBillingNode temp;

    while (p != NULL) {
        temp = p;
        p = p->next;
        free(temp);
    }
    billingList = NULL;
}

// 新增：从文件获取消费信息到链表
int getBilling(void) {
    int nCount = 0;
    Billing* pBilling = NULL;
    int i = 0;
    lpBillingNode node = NULL;
    lpBillingNode cur = NULL;

    // 如果链表已存在，先释放
    if (billingList != NULL) {
        releaseBillingList();
    }

    // 初始化链表
    if (initBillingList() == FALSE) {
        return FALSE;
    }

    // 获取消费信息数量
    nCount = getBillingCount(BILLINGPATH);
    if (nCount == 0) {
        return TRUE;
    }

    // 动态分配内存，用于临时存储读取的消费信息
    pBilling = (Billing*)malloc(sizeof(Billing) * nCount);
    if (pBilling == NULL) {
        return FALSE;
    }

    // 读取消费信息到数组
    if (FALSE == readBilling(pBilling, BILLINGPATH)) {
        free(pBilling);
        pBilling = NULL;
        return FALSE;
    }

    // 将消费信息保存到链表中
    node = billingList;
    if (node == NULL) {
        free(pBilling);
        return FALSE;
    }

    for (i = 0; i < nCount; i++) {
        // 创建新节点
        cur = (lpBillingNode)malloc(sizeof(BillingNode));
        if (cur == NULL) {
            free(pBilling);
            return FALSE;
        }

        // 初始化新节点
        memset(cur, 0, sizeof(BillingNode));
        cur->data = pBilling[i];
        cur->next = NULL;

        // 添加到链表尾部
        node->next = cur;
        node = cur;
    }

    free(pBilling);
    pBilling = NULL;
    return TRUE;
}

// 新增：根据卡号查询消费信息（返回消费信息指针和索引）
Billing* queryBilling(const char* pName, int* pIndex)
{
    lpBillingNode p = NULL;
    lpBillingNode pLatest = NULL;
    int nIndex = 0;
    int nLatestIndex = -1;

    if (billingList == NULL || billingList->next == NULL) {
        getBilling();
    }

    if (billingList == NULL) {
        return NULL;
    }

    p = billingList->next;

    while (p != NULL) {
        // 匹配卡号且状态为未结算(0)
        if (strcmp(p->data.aCardName, pName) == 0 && p->data.nStatus == 0) {
            // 找到更新的记录（上机时间更晚）
            if (pLatest == NULL || p->data.tStart > pLatest->data.tStart) {
                pLatest = p;
                nLatestIndex = nIndex;
            }
        }
        p = p->next;
        nIndex++;
    }

    if (pLatest != NULL) {
        if (pIndex != NULL) {
            *pIndex = nLatestIndex;
        }
        return &(pLatest->data);
    }

    return NULL;
}
// 新增：更新消费信息（下机时调用，覆盖写入）
int updateBilling(const Billing* pBilling, const char* pPath, int nIndex) {
    FILE* fp = NULL;
    long lPosition = 0;

    // 以读写方式打开二进制文件
    errno_t err = fopen_s(&fp, pPath, "r+b");
    if (err != 0 || fp == NULL) {
        return FALSE;
    }

    // 定位到要更新的记录位置（根据索引计算偏移量）
    lPosition = nIndex * sizeof(Billing);
    fseek(fp, lPosition, SEEK_SET);

    // 覆盖写入更新后的消费信息
    fwrite(pBilling, sizeof(Billing), 1, fp);

    fclose(fp);
    return TRUE;
}