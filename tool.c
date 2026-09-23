#include <time.h>
#include<stdio.h>
#include "tool.h"

// 将 time_t 类型转换为字符串，字符串格式为“年-月-日 时:分”
void timeToString(time_t t, char* pBuf) {
    struct tm timeInfo;  // 定义结构体变量，不是指针

    // 将 time_t 类型转化为 tm 类型（安全版本）
    errno_t err = localtime_s(&timeInfo, &t);

    if (err == 0) {
        // 将 tm 类型转化为字符串
        strftime(pBuf, 20, "%Y-%m-%d %H:%M", &timeInfo);
    }
    else {
        // 转换失败，写入错误信息
        sprintf_s(pBuf,20, "时间错误");
    }
}

time_t stringToTime(char* pTime)
{
    struct tm tm1 = { 0 };  // 初始化为0
    time_t time1;
    int year, mon, day, hour, min;

    // 先解析到临时变量
    sscanf_s(pTime, "%d-%d-%d %d:%d", &year, &mon, &day, &hour, &min);

    // 转换后赋值给 tm1
    tm1.tm_year = year - 1900;
    tm1.tm_mon = mon - 1;
    tm1.tm_mday = day;
    tm1.tm_hour = hour;
    tm1.tm_min = min;
    tm1.tm_sec = 0;

    time1 = mktime(&tm1);//mktime()将struct tm转换为time_t时间戳  自动调整溢出(如tm_mon=13会变成下一年的一月)

    return time1;
}