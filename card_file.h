#ifndef CARD_FILE_H
#define CARD_FILE_H
#include"model.h"


int saveCard(const Card* pCard, const char* pPath);  //将卡信息存入文件
int getCardCount(const char* pPath);                 //获取文件卡信息的数量
int readCard(Card* pCard, const char* pPath);        //把卡信息读出文件
Card praseCard(char* pBuf);                          //将卡信息间的分隔符去掉
int updateCard(const Card* pCard, const char* pPath, int nIndex);//更新文件中的卡信息

#endif // !CARD_FILE_H

