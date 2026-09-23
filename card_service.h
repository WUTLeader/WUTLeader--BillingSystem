#ifndef CARD_SERVICE_H
#define CARD_SERVICE_H
#include"model.h"

int addCard(Card card);//将添加卡的卡信息放到该结构体数组中或链表或文件中,便于查询卡
int initInitCardList();////编写初始化链表函数
void releaseCardList();//该函数用来释放卡信息链表
Card* queryCard(const char* pName);//精准查询
Card* queryCards(const char* pName, int* pIndex);//模糊查询
extern lpCardNode cardList; //声明外部变量
int getCard();
int checkCard(const char* pName, const char* pPwd, lpCardNode* ppCard, int* pIndex);   //查询上机卡，返回状态码
int findCard(const char* pName, const char* pPwd, lpCardNode* ppCard, int* pIndex);  // 只查找卡，不检查状态和余额
#endif // !CARD_SERVICE_H

