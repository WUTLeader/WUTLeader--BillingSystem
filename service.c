#include"model.h"
#include"global.h"
#include"card_file.h"
#include"card_service.h"
#include"billing_file.h"
#include"billing_service.h"
#include<string.h>
#include<stdio.h>

// 添加卡信息
int addCardInfo(Card card) {
    return saveCard(&card, CARDPATH);
}

// 精确查询卡信息
Card* queryCardInfo(const char* pName) {
    return queryCard(pName);
}

// 模糊查询卡信息
Card* queryCardsInfo(const char* pName, int* pIndex) {
    return queryCards(pName, pIndex);
}

//上机功能
int doLogon(const char* pName, const char* pPwd, LogonInfo* pInfo) {
    lpCardNode pCardNode = NULL;
    int nIndex = 0;
    int nResult = 0;
    Billing billing = { 0 };

    // 1. 调用 checkCard 查找上机卡，获取卡节点和索引
    nResult = checkCard(pName, pPwd, &pCardNode, &nIndex);

    // 2. 根据查找结果处理
    if (nResult == LOGON_SUCCESS) {
        // 更新链表中的卡信息
        pCardNode->data.nStatus = 1;           // 状态改为上机中
        pCardNode->data.nUseCount++;           // 使用次数+1
        pCardNode->data.tLast = time(NULL);    // 最后使用时间更新

        // 更新文件中的卡信息
        if (TRUE == updateCard(&pCardNode->data, CARDPATH, nIndex)) {
            // 添加消费记录
            strcpy_s(billing.aCardName, sizeof(billing.aCardName), pName);
            billing.tStart = time(NULL);       // 上机时间
            billing.tEnd = 0;                  // 下机时间暂为0
            billing.fAmount = 0;               // 消费金额暂为0
            billing.nStatus = 0;               // 未结算
            billing.nDel = 0;                  // 未删除

            if (TRUE == saveBilling(&billing, BILLINGPATH)) {
                // 成功上机，返回上机信息
                if (pInfo != NULL) {
                    strcpy_s(pInfo->aCardName, sizeof(pInfo->aCardName), pCardNode->data.aName);
                    pInfo->fBalance = pCardNode->data.fBalance;
                    pInfo->tLogon = billing.tStart;
                }
                return LOGON_SUCCESS;
            }
        }
        return LOGON_FAIL;
    }

    // 返回卡状态错误或余额不足等
    return nResult;
}

// 添加：计算消费金额函数
// 功能：根据上机时间，计算从那时到现在的消费金额
// 参数：tStart - 上机时间戳
// 返回值：消费金额（元）
double getAmount(time_t tStart)
{
    time_t tEnd = time(NULL);  // 获取当前时间作为下机时间
    int nSec = 0;              // 消费秒数
    int nMinutes = 0;          // 消费分钟数
    int nCount = 0;            // 消费单元数
    double dbAmount = 0.0;     // 消费金额

    // 1、计算消费时长（秒数）
    nSec = (int)(tEnd - tStart);

    // 2、转换为分钟数
    nMinutes = nSec / 60;

    // 3、计算消费的时间单元数（每个单元15分钟，不足一个单元按一个单元计算）
    if (nMinutes % UNIT == 0)
    {
        nCount = nMinutes / UNIT;
    }
    else
    {
        nCount = nMinutes / UNIT + 1;  // 不足一个单元，加1
    }

    // 4、计算消费金额
    dbAmount = nCount * CHARGE;

    return dbAmount;
}

// 添加：下机功能函数
// 功能：根据卡号和密码进行下机操作，计算消费金额，更新卡余额
// 参数：pName - 卡号，pPwd - 密码，pInfo - 下机信息输出参数
// 返回值：下机结果状态码
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo)
{
    Card* pCard = NULL;
    Billing* pBilling = NULL;
    int nPosition = 0;
    double dbAmount = 0.0;
    float fBalance = 0.0;

    // 1、查询上机卡，判断该卡是否存在
    pCard = queryCard(pName);
    if (pCard == NULL) {
        return SETTLE_FAIL;  // 卡号不存在，下机失败
    }

    // 验证密码是否正确
    if (strcmp(pCard->aPwd, pPwd) != 0) {
        return SETTLE_FAIL;  // 密码错误，下机失败
    }

    // 2、判断查询到的下机卡的状态，只有状态为"正在上机"的卡，才能进行下机的操作
    if (pCard->nStatus != 1) {
        return SETTLE_CARD_NOT_LOGON;  // 未上机，无法下机
    }

    // 3、根据卡号，查询计费信息（获取上机时间）
    pBilling = queryBilling(pName, &nPosition);
    if (pBilling == NULL) {
        return SETTLE_FAIL;  // 没有找到消费记录，下机失败
    }

    // 4、计算消费金额
    dbAmount = getAmount(pBilling->tStart);

    // 5、计算下机卡的余额，如果余额小于消费金额，则不能进行下机
    fBalance = pCard->fBalance - (float)dbAmount;
    if (fBalance < 0) {
        return SETTLE_NOT_ENOUGH_MONEY;  // 余额不足，无法下机
    }

    // 6、更新卡信息（状态改为未上机，余额减去消费金额，更新最后使用时间）
    pCard->nStatus = 0;
    pCard->fBalance = fBalance;
    pCard->tLast = time(NULL);

    // 更新文件中的卡信息
    updateCard(pCard, CARDPATH, nPosition);

    // 7、更新消费信息（设置下机时间、消费金额、状态改为已结算）
    pBilling->tEnd = time(NULL);
    pBilling->fAmount = (float)dbAmount;
    pBilling->nStatus = 1;  // 已结算

    // 更新计费文件中的信息
    updateBilling(pBilling, BILLINGPATH, nPosition);

    // 8、组装下机信息，保存到结构体中，用于界面显示
    if (pInfo != NULL) {
        strcpy_s(pInfo->aCardName, sizeof(pInfo->aCardName), pCard->aName);
        pInfo->tStart = pBilling->tStart;   // 上机时间
        pInfo->tEnd = pBilling->tEnd;       // 下机时间
        pInfo->fAmount = (float)dbAmount;   // 消费金额
        pInfo->fBalance = pCard->fBalance;  // 剩余余额
    }

    return SETTLE_SUCCESS;  // 下机成功
}

// 添加：充值功能函数
// 功能：根据卡号、密码和充值金额进行充值操作
// 参数：pName - 卡号，pPwd - 密码，fAmount - 充值金额，pMoneyInfo - 充值信息输出参数
// 返回值：充值结果状态码
int doAddMoney(const char* pName, const char* pPwd, float fAmount, MoneyInfo* pMoneyInfo)
{
    Card* pCard = NULL;
    Money money = { 0 };
    int nIndex = 0;

    // 1、查询卡信息，判断该卡是否存在
    pCard = queryCard(pName);
    if (pCard == NULL) {
        return RECHARGE_FAIL;  // 卡号不存在
    }

    // 验证密码是否正确
    if (strcmp(pCard->aPwd, pPwd) != 0) {
        return RECHARGE_FAIL;  // 密码错误
    }

    // 2、判断该卡是否可以充值，只有未使用(0)和正在使用(1)的卡才能进行充值操作
    // 已注销(2)、失效(3)的卡不能充值
    if (pCard->nStatus == 2 || pCard->nStatus == 3) {
        return RECHARGE_CARD_INVALID;  // 卡无效
    }

    // 3、更新卡信息（余额增加，累计金额增加）
    pCard->fBalance += fAmount;
    pCard->fTotalUse += fAmount;
    pCard->tLast = time(NULL);

    // 更新文件中的卡信息（需要找到卡在文件中的索引）
    // 注意：这里需要根据卡号获取索引，简化处理，先遍历查找
    lpCardNode node = cardList->next;
    while (node != NULL) {
        if (strcmp(node->data.aName, pName) == 0) {
            break;
        }
        node = node->next;
        nIndex++;
    }
    updateCard(pCard, CARDPATH, nIndex);

    // 4、保存充值信息到文件
    strcpy_s(money.aCardName, sizeof(money.aCardName), pName);
    money.tTime = time(NULL);
    money.nStatus = MONEY_RECHARGE;  // 充值
    money.fMoney = fAmount;
    money.nDel = 0;
    saveMoney(&money, MONEY_PATH);

    // 5、返回充值信息
    if (pMoneyInfo != NULL) {
        strcpy_s(pMoneyInfo->aCardName, sizeof(pMoneyInfo->aCardName), pCard->aName);
        pMoneyInfo->fMoney = fAmount;
        pMoneyInfo->fBalance = pCard->fBalance;
    }

    return RECHARGE_SUCCESS;
}

// 添加：退费功能函数
// 功能：根据卡号、密码和退费金额进行退费操作
// 参数：pName - 卡号，pPwd - 密码，fAmount - 退费金额，pMoneyInfo - 退费信息输出参数
// 返回值：退费结果状态码
int doRefundMoney(const char* pName, const char* pPwd, float fAmount, MoneyInfo* pMoneyInfo)
{
    Card* pCard = NULL;
    Money money = { 0 };
    int nIndex = 0;

    // 1、查询卡信息，判断该卡是否存在
    pCard = queryCard(pName);
    if (pCard == NULL) {
        return REFUND_FAIL;  // 卡号不存在
    }

    // 验证密码是否正确
    if (strcmp(pCard->aPwd, pPwd) != 0) {
        return REFUND_FAIL;  // 密码错误
    }

    // 2、判断该卡是否可以退费，只有未使用的卡(0)才能进行退费操作
    if (pCard->nStatus != 0) {
        return REFUND_FAIL;  // 卡正在使用或已注销，不能退费
    }

    // 3、判断卡里的余额是否大于等于退费金额
    if (pCard->fBalance < fAmount) {
        return REFUND_NO_MONEY;  // 余额不足
    }

    // 4、更新卡信息（余额减少）
    pCard->fBalance -= fAmount;
    pCard->tLast = time(NULL);

    // 更新文件中的卡信息
    lpCardNode node = cardList->next;
    while (node != NULL) {
        if (strcmp(node->data.aName, pName) == 0) {
            break;
        }
        node = node->next;
        nIndex++;
    }
    updateCard(pCard, CARDPATH, nIndex);

    // 5、保存退费信息到文件
    strcpy_s(money.aCardName, sizeof(money.aCardName), pName);
    money.tTime = time(NULL);
    money.nStatus = MONEY_REFUND;  // 退费
    money.fMoney = fAmount;
    money.nDel = 0;
    saveMoney(&money, MONEY_PATH);

    // 6、返回退费信息
    if (pMoneyInfo != NULL) {
        strcpy_s(pMoneyInfo->aCardName, sizeof(pMoneyInfo->aCardName), pCard->aName);
        pMoneyInfo->fMoney = fAmount;
        pMoneyInfo->fBalance = pCard->fBalance;
    }

    return REFUND_SUCCESS;
}

void doStatistics(void) {
    int nTotalCards = 0;        // 总卡数
    int nLogonCards = 0;        // 正在上机卡数
    int nInvalidCards = 0;      // 已注销卡数
    float fTotalRecharge = 0.0; // 总充值金额（累计金额）
    float fTotalCost = 0.0;     // 总消费金额
    int nTotalUseCount = 0;     // 总使用次数

    lpCardNode pCard = NULL;
    lpBillingNode pBilling = NULL;

    // 1. 确保数据已加载
    if (cardList == NULL || cardList->next == NULL) {
        getCard();
    }
    if (billingList == NULL || billingList->next == NULL) {
        getBilling();
    }

    // 2. 统计卡信息
    pCard = cardList->next;
    while (pCard != NULL) {
        nTotalCards++;
        if (pCard->data.nStatus == 1) {
            nLogonCards++;
        }
        if (pCard->data.nStatus == 2) {
            nInvalidCards++;
        }
        fTotalRecharge += pCard->data.fTotalUse;
        nTotalUseCount += pCard->data.nUseCount;
        pCard = pCard->next;
    }

    // 3. 统计消费信息
    pBilling = billingList->next;
    while (pBilling != NULL) {
        fTotalCost += pBilling->data.fAmount;
        pBilling = pBilling->next;
    }

    // 4. 显示统计结果
    printf("========== 统计结果 ==========\n");
    printf("【卡信息统计】\n");
    printf("  总卡数：%d 张\n", nTotalCards);
    printf("  正在上机：%d 张\n", nLogonCards);
    printf("  已注销：%d 张\n", nInvalidCards);
    printf("  总充值金额：%.2f 元\n", fTotalRecharge);
    printf("\n");
    printf("【使用统计】\n");
    printf("  总使用次数：%d 次\n", nTotalUseCount);
    printf("  总消费金额：%.2f 元\n", fTotalCost);
    printf("\n");
    printf("【余额统计】\n");
    printf("  总剩余余额：%.2f 元\n", fTotalRecharge - fTotalCost);

    // ========== 5. 新增：显示充值退费记录 ==========
    printf("\n========== 充值退费记录 ==========\n");
    FILE* fp = NULL;
    Money money = { 0 };
    int nMoneyCount = 0;

    errno_t err = fopen_s(&fp, MONEY_PATH, "rb");
    if (err != 0 || fp == NULL) {
        printf("暂无充值退费记录\n");
    }
    else {
        printf("%-10s %-20s %-10s %-10s\n", "卡号", "时间", "类型", "金额");
        printf("%-10s %-20s %-10s %-10s\n", "--------", "--------------------", "--------", "--------");

        while (fread(&money, sizeof(Money), 1, fp) == 1) {
            char timeStr[30] = { 0 };
            timeToString(money.tTime, timeStr);

            printf("%-10s %-20s %-10s %-10.2f\n",
                money.aCardName,
                timeStr,
                money.nStatus == 0 ? "充值" : "退费",
                money.fMoney);
            nMoneyCount++;
        }
        printf("共 %d 条记录\n", nMoneyCount);
        fclose(fp);
    }

    printf("==============================\n");
}
// 添加：注销卡功能函数
// 功能：根据卡号和密码进行注销操作，退还卡内余额
// 参数：pName - 卡号，pPwd - 密码，pRefund - 退款金额输出参数
// 返回值：1-成功，0-失败，2-卡状态不符合
int annulCard(const char* pName, const char* pPwd, float* pRefund)
{
    lpCardNode pCurCard = NULL;
    int nIndex = 0;
    float fRefund = 0.0;

    // 使用 findCard 查找卡（不检查状态和余额）
    if (FALSE == findCard(pName, pPwd, &pCurCard, &nIndex)) {
        return 0;  // 卡号或密码错误
    }

    // 判断注销卡的状态，只有状态为"未上机"(0)的卡才能注销
    if (pCurCard->data.nStatus != 0) {
        return 2;  // 卡正在上机，无法注销
    }

    fRefund = pCurCard->data.fBalance;

    // 更新链表
    pCurCard->data.nStatus = 2;
    pCurCard->data.fBalance = 0;
    pCurCard->data.tLast = time(NULL);
    pCurCard->data.nDel = 1;

    // 更新文件
    if (FALSE == updateCard(&(pCurCard->data), CARDPATH, nIndex)) {
        return 0;
    }

    if (pRefund != NULL) {
        *pRefund = fRefund;
    }

    return 1;
}


// 释放链表内存
void releaseList(void) {
    releaseCardList();      // 释放卡信息链表
    releaseBillingList();   // 释放计费信息链表
    // 充值退费信息不需要常驻内存，每次直接读写文件，所以不需要释放链表
}

