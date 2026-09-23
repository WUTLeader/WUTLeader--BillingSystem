//将一些函数写在menu.c中,而不写在main.c中,是为了便于改错时,修正代码,且能让代码更清晰

#include<stdio.h>//包含输入输出函数的头文件
#include<time.h>//导入时间头文件,包含time(NULL),可获取当前时间
#include<stdlib.h>//包含countof函数
#include"model.h"//导入结构体数据定义头文件
#include"card_service.h"
#include"menu.h"
#include"tool.h"
#include"card_file.h"
#include<string.h>
#include"global.h"
#include"service.h"


void outputMenu(void) {
    printf("1.添加卡\n2.查询卡\n3.上机\n4.下机\n5.充值\n6.退费\n7.查询统计\n8.注销卡\n0.退出\n");
    printf("请选择菜单项编号(0-8):");
}

void add() {
    Card card;                  //卡信息
    char aName[32] = { '\0' };  //输入的卡号
    char aPwd[20] = { '\0' };   //输入的密码
    int nNameSize = 0;
    int nPwdSize = 0;
    printf("----------添加卡----------\n");
    printf("请输入卡号(长度为1-18位)：");
    scanf_s("%s", aName, (unsigned)_countof(aName));

    // 添加：检查卡号是否已存在
    Card* pExist = queryCardInfo(aName);
    if (pExist != NULL) {
        printf("卡号 %s 已存在，请勿重复添加！\n", aName);
        return;
    }

    printf("请输入密码(长度为1-8位)：");
    scanf_s("%s", aPwd, (unsigned)_countof(aPwd));

    //判断卡号和密码长度是否符合要求
    nNameSize = getSize(aName);
    nPwdSize = getSize(aPwd);
    if (nNameSize > 18 || nPwdSize > 8) {
        printf("卡号或密码超过规定长度\n");
        return;
    }
    strcpy_s(card.aName, sizeof(card.aName), aName);
    strcpy_s(card.aPwd, sizeof(card.aPwd), aPwd);

    printf("请输入开卡金额：");
    scanf_s("%f", &card.fBalance);
    card.fTotalUse = card.fBalance;
    card.nUseCount = 0;
    card.nStatus = 0;
    card.nDel = 0;
    card.tStart = card.tEnd = card.tLast = time(NULL);

    // 计算截止时间：开卡时间 + 1年（365天 * 24小时 * 3600秒）
    card.tEnd = card.tStart + (365 * 24 * 3600);

    // 调用 service.c 中的 addCardInfo 函数
    if (TRUE == addCardInfo(card)) {
        printf("添加卡信息成功!\n\n");
        printf("---添加的卡信息如下---\n");
        printf("卡号\t密码\t卡状态\t余额\n");
        printf("%s\t%s\t%d\t%.1f\n", card.aName, card.aPwd, card.nStatus, card.fBalance);
    }
    else {
        printf("添加卡信息失败！\n");
    }
}

int getSize(const char* pInfo) {//const用来保护数据,承诺不会修改传入的数据
    int nSize = 0;
    while (*(pInfo + nSize) != '\0') {
        nSize++;
    }
    return nSize;
}

// 查询主菜单（选择精准还是模糊）
void query() {
    int nChoice = 0;

    printf("----------查询卡----------\n");
    printf("请选择查询方式：\n");
    printf("1. 精准查询（卡号必须完全匹配）\n");
    printf("2. 模糊查询（输入部分卡号即可）\n");
    printf("请输入选择(1-2)：");
    scanf_s("%d", &nChoice);

    switch (nChoice) {
    case 1:
        preciseQuery();
        break;
    case 2:
        fuzzyQuery();
        break;
    default:
        printf("输入错误！请选择1或2！\n");
        break;
    }
}

void fuzzyQuery() {
    char aName[18] = { 0 };
    char aTime[20] = { 0 };
    int nIndex = 0;
    int i = 0;
    Card* pCard = NULL;

    printf("----------查询卡----------\n");
    printf("请输入您要查询的卡号:");
    scanf_s("%s", aName, (unsigned)_countof(aName));

    pCard = queryCardsInfo(aName, &nIndex);

    if (pCard != NULL && nIndex > 0) {
        printf("%-10s %-5s %-8s %-10s %-10s %-25s\n",
            "卡号", "状态", "余额", "累计金额", "使用次数", "上次使用时间");

        for (i = 0; i < nIndex; i++) {
            timeToString(pCard[i].tLast, aTime);
            printf("%-10s %-5d %-8.1f %-10.1f %-10d %-25s\n",
                pCard[i].aName,
                pCard[i].nStatus,
                pCard[i].fBalance,
                pCard[i].fTotalUse,
                pCard[i].nUseCount,
                aTime);
        }
        free(pCard);
    }
    else {
        printf("您查询的卡号不存在,请检查您的卡号输入是否正确\n");
    }
}

// 精准查询
void preciseQuery() {
    char aName[18] = { 0 };
    char aTime[20] = { 0 };
    Card* pCard = NULL;

    printf("----------精准查询----------\n");
    printf("请输入要查询的卡号（必须完全匹配）：");
    scanf_s("%s", aName, (unsigned)_countof(aName));

    pCard = queryCardInfo(aName);

    if (pCard != NULL) {
        timeToString(pCard->tLast, aTime);
        printf("%-10s %-5s %-8s %-10s %-10s %-25s\n",
            "卡号", "状态", "余额", "累计金额", "使用次数", "上次使用时间");
        printf("%-10s %-5d %-8.1f %-10.1f %-10d %-25s\n",
            pCard->aName,
            pCard->nStatus,
            pCard->fBalance,
            pCard->fTotalUse,
            pCard->nUseCount,
            aTime);
    }
    else {
        printf("卡号 %s 不存在！请检查输入是否正确\n", aName);
    }
}

void exitAPP() {
    releaseList();  // 调用 service.c 中的 releaseList 函数
}

void logon() {
    char aName[20] = { 0 };
    char aPwd[8] = { 0 };
    LogonInfo* pInfo = NULL;
    int nResult = 0;

    printf("----------上机----------\n");
    //接受用户输入的卡号和密码
    printf("请输入卡号：");
    scanf_s("%s", aName, (unsigned)_countof(aName));
    printf("请输入密码：");
    scanf_s("%s", aPwd, (unsigned)_countof(aPwd));

    // 分配上机信息结构体内存
    pInfo = (LogonInfo*)malloc(sizeof(LogonInfo));
    if (pInfo == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    // 进行上机
    nResult = doLogon(aName, aPwd, pInfo);

    // 根据上机结果，提示不同信息
    switch (nResult)
    {
    case LOGON_FAIL:
        printf("上机失败！请检查卡号或密码\n");
        break;
    case LOGON_SUCCESS:
        printf("上机成功！\n");
        printf("卡号：%s\n", pInfo->aCardName);

        char timeStr[30];
        ctime_s(timeStr, sizeof(timeStr), &pInfo->tLogon);
        printf("上机时间：%s", timeStr);

        printf("卡内余额：%.2f\n", pInfo->fBalance);
        break;
    case LOGON_CARD_INVALID:
        printf("该卡不能使用！请检查卡状态\n");
        break;
    case LOGON_BALANCE_LOW:
        printf("余额不足！请充值后再上机\n");
        break;
    default:
        printf("未知错误！\n");
        break;
    }

    // 释放上机信息结构体内存
    if (pInfo != NULL) {
        free(pInfo);
        pInfo = NULL;
    }
}

//下机函数
void settle() {
    char aName[18] = { 0 }; // 卡号
    char aPwd[8] = { 0 };   // 密码
    int nResult = 0;
    SettleInfo* pInfo = NULL;

    printf("----------下机----------\n");
    printf("请输入下机卡号（长度为1~18）：");
    scanf_s("%s", aName, (unsigned)_countof(aName));

    printf("请输入下机密码（长度为1~8）：");
    scanf_s("%s", aPwd, (unsigned)_countof(aPwd));

    // 分配下机信息结构体内存
    pInfo = (SettleInfo*)malloc(sizeof(SettleInfo));
    if (pInfo == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    // 调用下机函数
    nResult = doSettle(aName, aPwd, pInfo);

    // 根据下机结果，提示不同信息
    switch (nResult)
    {
    case SETTLE_FAIL:
        printf("下机失败！请检查卡号或密码\n");
        break;
    case SETTLE_SUCCESS:
        printf("下机成功！\n");
        printf("卡号：%s\n", pInfo->aCardName);

        // 使用安全版本的 ctime_s
        char timeStr1[30];
        char timeStr2[30];
        ctime_s(timeStr1, sizeof(timeStr1), &pInfo->tStart);
        ctime_s(timeStr2, sizeof(timeStr2), &pInfo->tEnd);
        printf("上机时间：%s", timeStr1);
        printf("下机时间：%s", timeStr2);
        printf("消费金额：%.2f\n", pInfo->fAmount);
        printf("卡内余额：%.2f\n", pInfo->fBalance);
        break;
    case SETTLE_CARD_NOT_LOGON:
        printf("该卡未上机，无法下机！\n");
        break;
    case SETTLE_NOT_ENOUGH_MONEY:
        printf("余额不足，无法下机！\n");
        break;
    default:
        printf("未知错误！\n");
        break;
    }

    // 释放下机信息结构体内存
    if (pInfo != NULL) {
        free(pInfo);
        pInfo = NULL;
    }
}

// 充值功能函数
void addMoney() {
    char aName[18] = { 0 };      // 存放用户输入的卡号
    char aPwd[8] = { 0 };        // 存放用户输入的密码
    float fAmount = 0.0;         // 存放用户输入的充值金额
    int nResult = 0;             // 存放充值结果状态码
    MoneyInfo* pInfo = NULL;     // 存放充值信息，用于界面显示
    char ch;                     // 用于读取缓冲区中的字符，检查非法输入
    int nScanfResult;            // 存放scanf_s的返回值，判断是否成功读取数字

    printf("---充值---\n");
    printf("请输入充值卡号（长度为1~18）：");
    scanf_s("%s", aName, (unsigned)_countof(aName));

    printf("请输入充值密码（长度为1~8）：");
    scanf_s("%s", aPwd, (unsigned)_countof(aPwd));

    printf("请输入充值金额：");
    nScanfResult = scanf_s("%f", &fAmount);  // 尝试读取一个浮点数

    // 检查输入是否为数字（scanf_s返回1表示成功读取到1个浮点数）
    if (nScanfResult != 1) {
        printf("充值失败！金额只能输入数字！\n");
        // 清空输入缓冲区，清除残留的非法字符（如字母、符号等）
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;  // 提前返回，不执行充值操作
    }

    // 检查输入数字后面是否还有非法字符（比如输入"123abc"，scanf_s只读取了123，abc还留在缓冲区）
    ch = getchar();  // 读取数字后面的下一个字符
    if (ch != '\n' && ch != EOF) {
        printf("充值失败！金额只能输入数字，不能包含字母或其他字符！\n");
        // 清空剩余的缓冲区内容
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;  // 提前返回，不执行充值操作
    }

    // 检查金额是否为正数（不能为0或负数）
    if (fAmount <= 0) {
        printf("充值失败！充值金额必须大于0！\n");
        return;  // 提前返回，不执行充值操作
    }

    // 分配充值信息结构体内存，用于接收业务层返回的充值结果
    pInfo = (MoneyInfo*)malloc(sizeof(MoneyInfo));
    if (pInfo == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    // 调用业务层函数执行充值操作
    nResult = doAddMoney(aName, aPwd, fAmount, pInfo);

    // 根据充值结果，显示不同的提示信息
    switch (nResult) {
    case RECHARGE_SUCCESS:  // 充值成功
        printf("充值成功！\n");
        printf("卡号：%s\n", pInfo->aCardName);
        printf("充值金额：%.2f元\n", pInfo->fMoney);
        printf("当前余额：%.2f元\n", pInfo->fBalance);
        break;
    case RECHARGE_FAIL:  // 充值失败（卡号或密码错误）
        printf("充值失败！请检查卡号或密码\n");
        break;
    case RECHARGE_CARD_INVALID:  // 卡无效（已注销或已过期）
        printf("充值失败！该卡无效（已注销/过期）\n");
        break;
    default:  // 其他未知错误
        printf("未知错误！\n");
        break;
    }

    // 释放充值信息结构体内存，防止内存泄漏
    if (pInfo != NULL) {
        free(pInfo);
        pInfo = NULL;
    }
}
// 退费功能函数
void refundMoney() {
    char aName[18] = { 0 };      // 存放用户输入的卡号
    char aPwd[8] = { 0 };        // 存放用户输入的密码
    float fAmount = 0.0;         // 存放用户输入的退费金额
    int nResult = 0;             // 存放退费结果状态码
    MoneyInfo* pInfo = NULL;     // 存放退费信息，用于界面显示
    char ch;                     // 用于读取缓冲区中的字符，检查非法输入
    int nScanfResult;            // 存放scanf_s的返回值，判断是否成功读取数字

    printf("---退费---\n");
    printf("请输入退费卡号（长度为1~18）：");
    scanf_s("%s", aName, (unsigned)_countof(aName));

    printf("请输入退费密码（长度为1~8）：");
    scanf_s("%s", aPwd, (unsigned)_countof(aPwd));

    printf("请输入退费金额：");
    nScanfResult = scanf_s("%f", &fAmount);  // 尝试读取一个浮点数

    // 检查输入是否为数字（scanf_s返回1表示成功读取到1个浮点数）
    if (nScanfResult != 1) {
        printf("退费失败！金额只能输入数字！\n");
        // 清空输入缓冲区，清除残留的非法字符（如字母、符号等）
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;  // 提前返回，不执行退费操作
    }

    // 检查输入数字后面是否还有非法字符（比如输入"123abc"，scanf_s只读取了123，abc还留在缓冲区）
    ch = getchar();  // 读取数字后面的下一个字符
    if (ch != '\n' && ch != EOF) {
        printf("退费失败！金额只能输入数字，不能包含字母或其他字符！\n");
        // 清空剩余的缓冲区内容
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;  // 提前返回，不执行退费操作
    }

    // 检查金额是否为正数（不能为0或负数）
    if (fAmount <= 0) {
        printf("退费失败！退费金额必须大于0！\n");
        return;  // 提前返回，不执行退费操作
    }

    // 分配退费信息结构体内存，用于接收业务层返回的退费结果
    pInfo = (MoneyInfo*)malloc(sizeof(MoneyInfo));
    if (pInfo == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    // 调用业务层函数执行退费操作
    nResult = doRefundMoney(aName, aPwd, fAmount, pInfo);

    // 根据退费结果，显示不同的提示信息
    switch (nResult) {
    case REFUND_SUCCESS:  // 退费成功
        printf("退费成功！\n");
        printf("卡号：%s\n", pInfo->aCardName);
        printf("退费金额：%.2f元\n", pInfo->fMoney);
        printf("当前余额：%.2f元\n", pInfo->fBalance);
        break;
    case REFUND_FAIL:  // 退费失败（卡号或密码错误）
        printf("退费失败！请检查卡号或密码\n");
        break;
    case REFUND_NO_MONEY:  // 余额不足
        printf("退费失败！余额不足\n");
        break;
    default:  // 其他未知错误
        printf("未知错误！\n");
        break;
    }

    // 释放退费信息结构体内存，防止内存泄漏
    if (pInfo != NULL) {
        free(pInfo);
        pInfo = NULL;
    }
}

void statistics() {
    printf("---查询统计---\n");

    // 调用统计函数获取数据
    doStatistics();

    printf("\n");
}

// 添加：注销卡功能函数
void annul() {
    char aName[18] = { 0 };   // 卡号
    char aPwd[8] = { 0 };     // 密码
    int nResult = 0;
    float fRefund = 0.0;    // 退款金额

    printf("---注销卡---\n");
    printf("请输入要注销的卡号：");
    scanf_s("%s", aName, (unsigned)_countof(aName));

    printf("请输入密码：");
    scanf_s("%s", aPwd, (unsigned)_countof(aPwd));

    // 调用注销业务函数
    nResult = annulCard(aName, aPwd, &fRefund);

    // 根据注销结果，提示不同信息
    switch (nResult)
    {
    case 1:  // 注销成功
        printf("注销成功！\n");
        printf("卡号\t退款金额\n");
        printf("%s\t%.2f\n", aName, fRefund);
        break;
    case 0:  // 注销失败
        printf("注销失败！请检查卡号或密码\n");
        break;
    case 2:  // 卡状态不符合
        printf("注销失败！该卡正在上机，无法注销\n");
        break;
    default:
        printf("未知错误！\n");
        break;
    }
}

