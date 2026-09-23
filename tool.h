#ifndef TOOL_H
#define TOOL_H

void timeToString(time_t t, char* pBuf);  //将时间戳转换为时间字符串
time_t stringToTime(char* pTime);         //将时间字符串转化为时间戳

#endif // !TOOL_H

