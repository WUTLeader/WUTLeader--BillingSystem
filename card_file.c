#include<stdio.h>  //包含文件结构体头文件
#include"model.h"  //包含数据类型定义头文件
#include"global.h" //包含全局定义头文件
#include<string.h> //包含字符处理头文件
#include<stdlib.h> 
#include"tool.h"   //包含工具头文件
#include"card_file.h"
#include"card_service.h"
extern lpCardNode cardList; //声明外部变量
#define CARDCHARNUM 256

Card praseCard(char* pBuf);//函数声明

int saveCard(const Card* pCard, const char* pPath) {
	FILE* fp = NULL; //定义一个指向文件的指针变量fp,并初始化为空指针NULL
    
    char startTime[TIMELENGTH] = { 0 };  //开卡时间
    char endTime[TIMELENGTH] = { 0 };    //截止时间
    char lastTime[TIMELENGTH] = { 0 };   //最后使用时间
	
	//打开文件
    //使用安全版本的fopen函数fopen_s
    errno_t err = fopen_s(&fp, pPath, "a");
    if (err != 0 || fp == NULL)
    {
        // 追加模式失败，尝试写入模式（创建新文件）
        err = fopen_s(&fp, pPath, "w");
        if (err != 0 || fp == NULL)
        {
            return FALSE;
        }
    }

    // 时间转换为字符串
    timeToString(pCard->tStart, startTime);
    timeToString(pCard->tEnd, endTime);
    timeToString(pCard->tLast, lastTime);
	//将数据写入文件
    // 格式：卡号##密码##状态##开卡时间##截止时间##累计金额##最后使用时间##使用次数##当前余额##删除标记
    fprintf(fp, "%s##%s##%d##%s##%s##%.2f##%s##%d##%.2f##%d\n",
        pCard->aName,      // 卡号
        pCard->aPwd,       // 密码
        pCard->nStatus,    // 卡状态
        startTime,    // 开卡时间
        endTime,      //截止时间
        pCard->fTotalUse,  // 累计金额
        lastTime,   // 最后使用时间
        pCard->nUseCount,  // 使用次数
        pCard->fBalance,   // 当前余额
        pCard->nDel);      // 删除标记
   

	//关闭文件
    fclose(fp);

	return TRUE;
}

int readCard(Card* pCard, const char* pPath) {
    FILE* fp = NULL;  //定义一个指向文件的指针变量fp,并初始化为空指针NULL
    char aBuf[CARDCHARNUM] = { 0 };  //定义一个卡信息字符串,便于读取文件
    int i = 0;

    //打开文件
    //使用安全版本的fopen函数fopen_s
    errno_t err = fopen_s(&fp, pPath, "r");
    if (err != 0 || fp == NULL)
    {
        return FALSE;
    }

    //读取文件
    while (!feof(fp)) {
        memset(aBuf, 0, CARDCHARNUM); //清空数组
        if (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
            if (strlen(aBuf) > 0) {
                pCard[i] = praseCard(aBuf);
                i++;
            }
        }
    }
    

    //关闭文件
    fclose(fp);


    return TRUE;
}

//解析函数
Card praseCard(char* pBuf) {
    Card card;
    const char* delims = "##"; // 字符串中的分隔符
    char* context = NULL;      // strtok_s 的上下文指针
    char* str = NULL;
    char flag[10][20] = { 0 }; // 保存分割后的字符串
    int index = 0;

    // 第一次调用 strtok_s(安全版本)
    str = strtok_s(pBuf, delims, &context);

    while (str != NULL && index < 10)
    {
        strcpy_s(flag[index], sizeof(flag[index]), str);
        index++;
        // 后续调用，第一个参数传 NULL
        str = strtok_s(NULL, delims, &context);
    }

    strcpy_s(card.aName, sizeof(card.aName), flag[0]);  //安全版本strcpy_s,检查目标缓冲区大小
    strcpy_s(card.aPwd, sizeof(card.aPwd), flag[1]);

    card.nStatus = atoi(flag[2]);                      // 状态
    card.tStart = stringToTime(flag[3]);               // 开卡时间
    card.tEnd = stringToTime(flag[4]);                 // 截止时间
    card.fTotalUse = (float)atof(flag[5]);             // 累计金额
    card.tLast = stringToTime(flag[6]);                // 最后使用时间
    card.nUseCount = atoi(flag[7]);                    // 使用次数
    card.fBalance = (float)atof(flag[8]);              // 余额
    card.nDel = atoi(flag[9]);                         // 删除标识

    return card;
}

int getCardCount(const char* pPath) {
    int nCount = 0;
    FILE* fp = NULL;  //定义一个指向文件的指针变量fp,并初始化为空指针NULL
    char aBuf[CARDCHARNUM] = { 0 };  //定义一个卡信息字符串,便于读取文件
    int i = 0;

    //打开文件
    //使用安全版本的fopen函数fopen_s
    errno_t err = fopen_s(&fp, pPath, "r");
    if (err != 0 || fp == NULL)
    {
        return FALSE;
    }

    //读取文件
    while (!feof(fp)) {
        memset(aBuf, 0, CARDCHARNUM); //清空数组
        if (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
            if (strlen(aBuf) > 0) {
                nCount++;
            }
        }
    }


    //关闭文件
    fclose(fp);


    return nCount;
}



int updateCard(const Card* pCard, const char* pPath, int nIndex) {
    FILE* fp = NULL;
    int nLine = 0;
    char aBuf[CARDCHARNUM] = { 0 };
    long lPosition = 0;

    char startTime[TIMELENGTH] = { 0 };  //开卡时间
    char endTime[TIMELENGTH] = { 0 };    //截止时间
    char lastTime[TIMELENGTH] = { 0 };   //最后使用时间

    //以读写方式打开文件（改为 "r+" 才能写入）
    //"r"只能读取
    errno_t err = fopen_s(&fp, pPath, "r+");
    if (err != 0 || fp == NULL) {
        return FALSE;
    }

    //遍历文件，找到该条记录，进行更新
    while (!feof(fp) && nLine < nIndex) {
        if (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
            lPosition = ftell(fp);
            nLine++;
        }
    }

    // 检查是否找到目标位置
    if (nLine < nIndex) {
        fclose(fp);
        return FALSE;  // 没有找到该索引的记录
    }

    fseek(fp, lPosition, SEEK_SET);  // 定位到要更新的记录起始位置

    // 时间转换为字符串
    timeToString(pCard->tStart, startTime);
    timeToString(pCard->tEnd, endTime);
    timeToString(pCard->tLast, lastTime);

    //将数据写进文件
    // 格式：卡号##密码##状态##开卡时间##截止时间##累计金额##最后使用时间##使用次数##当前余额##删除标记
    fprintf(fp, "%s##%s##%d##%s##%s##%.2f##%s##%d##%.2f##%d\n",
        pCard->aName,      // 卡号
        pCard->aPwd,       // 密码
        pCard->nStatus,    // 卡状态
        startTime,         // 开卡时间
        endTime,           // 截止时间
        pCard->fTotalUse,  // 累计金额
        lastTime,          // 最后使用时间
        pCard->nUseCount,  // 使用次数
        pCard->fBalance,   // 当前余额
        pCard->nDel);      // 删除标记

    //关闭文件
    fclose(fp);

    return TRUE;
}