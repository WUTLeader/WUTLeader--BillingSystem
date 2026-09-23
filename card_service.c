#include"model.h"
#include<string.h>
#include"global.h"
#include<stdlib.h>
#include"card_file.h"
#include"card_service.h"
#include<time.h>
#include"billing_file.h"
#include"billing_service.h"

lpCardNode cardList = NULL;//定义全局头指针(会一直记住它),可以通过它来找到头节点,一边访问整个链表

//编写初始化链表函数
int initInitCardList()
{
    lpCardNode head = NULL;
    head = (lpCardNode)malloc(sizeof(CardNode));
    if (head == NULL) {
        return FALSE;
    }
    // 删除多余的 if (head != NULL) 判断
    head->next = NULL;
    cardList = head;
    return TRUE;  // 确保所有路径都有返回值
}

//释放卡信息链表
void releaseCardList(){
    if (cardList == NULL) {
        return;//链表未初始化直接返回
    }
    lpCardNode p = cardList;  // p 指向头结点
    lpCardNode temp;

    // 遍历释放所有节点（包括头结点）
    while (p != NULL)
    {
        temp = p;           // temp保存当前节点地址
        p = p->next;        // p直接指向下一个节点
        free(temp);         // 释放当前节点
    }
    cardList = NULL;        // 释放后将头指针置空

}

/*
int addCard(Card card) {
    // 1. 申请新节点接受卡信息
    lpCardNode cur = (lpCardNode)malloc(sizeof(CardNode));
    if (cur == NULL) {  
        return FALSE;
    }

    // 2. 填充数据
    cur->data = card;
    cur->next = NULL;

    // 3. 找到链表尾部（带头结点，从 cardList 开始遍历）
    lpCardNode p = cardList;
    while (p->next != NULL) {
        p = p->next;
    }

    // 4. 把新节点挂到尾部
    p->next = cur;

    return TRUE;
}*/

int addCard(Card card) {
    // 1. 创建新节点
    lpCardNode cur = (lpCardNode)malloc(sizeof(CardNode));
    if (cur == NULL) {
        return FALSE;
    }

    // 2. 填充数据
    cur->data = card;
    cur->next = NULL;

    // 3. 找到链表尾部（假设 cardList 已经在 main 中初始化好了）
    lpCardNode p = cardList;
    while (p->next != NULL) {
        p = p->next;
    }

    // 4. 添加到链表尾部
    p->next = cur;

    // 5. 保存到文件
    saveCard(&card, CARDPATH);

    // 6. 调试：打印当前链表所有卡号
    printf("========== 当前链表内容 ==========\n");
    lpCardNode temp = cardList->next;
    int count = 0;
    while (temp != NULL) {
        count++;
        printf("第%d张：%s\n", count, temp->data.aName);
        temp = temp->next;
    }
    printf("共%d张卡\n", count);
    printf("================================\n");

    return TRUE;
}
/*
指针的查询卡函数
//精确查询
Card* queryCard(const char* pName) {
    if (cardList == NULL) {
        return NULL;
    }
    // 跳过头结点，从第一个数据节点开始
    lpCardNode p = cardList->next;

    while (p != NULL) {
        // 比较卡号是否匹配
        if (strcmp(p->data.aName, pName) == 0) {
            return &(p->data);  // 返回卡信息的指针
        }
        p = p->next;  // 移动到下一个节点
    }

    return NULL;  // 未找到，返回NULL
}
*/

//文件的精准查询
Card* queryCard(const char* pName) {
    // 如果链表为空，先尝试从文件加载数据
    if (cardList == NULL || cardList->next == NULL) {
        getCard();  // 加载卡数据到链表
    }

    // 再次检查是否为空
    if (cardList == NULL) {
        return NULL;
    }

    // 跳过头结点，从第一个数据节点开始
    lpCardNode p = cardList->next;

    while (p != NULL) {
        // 比较卡号是否匹配
        if (strcmp(p->data.aName, pName) == 0) {
            return &(p->data);  // 返回卡信息的指针
        }
        p = p->next;  // 移动到下一个节点
    }

    return NULL;  // 未找到，返回NULL
}


/*
//模糊查询
Card* queryCards(const char* pName, int* pIndex) {
    // 1. 参数检查
    if (pIndex == NULL) {
        return NULL;
    }
    *pIndex = 0;

    // 2. 检查链表是否初始化
    if (cardList == NULL || cardList->next == NULL) {
        return NULL;
    }

    // 3. 初始分配内存
    Card* pCard = (Card*)malloc(sizeof(Card));
    if (pCard == NULL) {
        return NULL;
    }

    // 4. 跳过头结点开始遍历
    lpCardNode p = cardList->next;

    while (p != NULL) {
        if (strstr(p->data.aName, pName) != NULL) {
            // 存储匹配的卡信息
            pCard[*pIndex] = p->data;
            (*pIndex)++;

            // 重新分配内存（预留下一个位置）
            Card* newCard = (Card*)realloc(pCard, ((*pIndex) + 1) * sizeof(Card));
            if (newCard == NULL) {
                // realloc 失败，原 pCard 仍然有效，返回已找到的结果
                return pCard;
            }
            pCard = newCard;
        }
        p = p->next;
    }

    // 如果没有找到任何卡，释放内存并返回 NULL
    if (*pIndex == 0) {
        free(pCard);
        return NULL;
    }

    return pCard;
}
*/

//文件的模糊查询
//文件的模糊查询
Card* queryCards(const char* pName, int* pIndex) {
    // 1. 参数检查
    if (pIndex == NULL) {
        return NULL;
    }
    *pIndex = 0;

    // 2. 检查链表是否初始化
    if (cardList == NULL || cardList->next == NULL) {
        // 新增调试
        printf("链表为空！请先调用 getCard() 加载数据\n");
        return NULL;
    }

    // 3. 初始分配内存
    Card* pCard = (Card*)malloc(sizeof(Card));
    if (pCard == NULL) {
        return NULL;
    }

    // 新增调试
    printf("正在查询卡号包含 [%s] 的卡...\n", pName);

    // 4. 跳过头结点开始遍历
    lpCardNode p = cardList->next;

    while (p != NULL) {
        // 新增调试
        printf("链表中的卡号: [%s]\n", p->data.aName);

        if (strstr(p->data.aName, pName) != NULL) {
            // 新增调试
            printf("匹配成功: %s\n", p->data.aName);

            // 存储匹配的卡信息
            pCard[*pIndex] = p->data;
            (*pIndex)++;

            // 重新分配内存（预留下一个位置）
            Card* newCard = (Card*)realloc(pCard, ((*pIndex) + 1) * sizeof(Card));
            if (newCard == NULL) {
                // realloc 失败，原 pCard 仍然有效，返回已找到的结果
                return pCard;
            }
            pCard = newCard;
        }
        p = p->next;
    }

    // 新增调试
    printf("共找到 %d 张卡\n", *pIndex);

    // 如果没有找到任何卡，释放内存并返回 NULL
    if (*pIndex == 0) {
        free(pCard);
        return NULL;
    }

    return pCard;
}

//将文件中的卡信息保存到链表中
int getCard() {
    int nCount = 0;
    Card* pCard = NULL;
    int i = 0;
    lpCardNode node = NULL;
    lpCardNode cur = NULL;

    if (cardList != NULL) {
        releaseCardList();
    }

    // 初始化链表并检查是否成功
    if (initInitCardList() == FALSE) {
        return FALSE;
    }

    // 获取卡信息数量
    nCount = getCardCount(CARDPATH);   //CARDPATH为卡路径

    // 如果没有卡数据，直接返回成功
    if (nCount == 0) {
        return TRUE;
    }

    // 动态分配内存
    pCard = (Card*)malloc(sizeof(Card) * nCount);
    if (pCard == NULL)
    {
        return FALSE;
    }

    //获取卡信息
    if (FALSE == readCard(pCard, CARDPATH)) {
        free(pCard);
        pCard = NULL;
        return FALSE;
    }

    //将获取到的卡信息保存到链表中去

    node = cardList;  // 确保 node 有值

    // 添加判断，确保 node 不为 NULL
    if (node == NULL) {
        free(pCard);
        pCard = NULL;
        return FALSE;
    }

    for (i = 0; i < nCount; i++)  // 循环创建节点
    {
        // 为结点分配内存
        cur = (lpCardNode)malloc(sizeof(CardNode));

        // 如果分配内存失败，则返回
        if (cur == NULL)
        {
            free(pCard);  //释放分配给pCard的内存,避免内存泄漏
            return FALSE;
        }

        // 初始化新的空间，全部赋值为0
        memset(cur, 0, sizeof(CardNode));  //防止残留垃圾数据

        // 将卡信息保存到结点中
        cur->data = pCard[i];
        cur->next = NULL;

        // 将结点添加到链表尾部
        node->next = cur;
        node = cur;
    }

    free(pCard);
    pCard = NULL;

    return TRUE;
}

//查询上机卡函数，只负责在链表中查找符合条件的卡
int checkCard(const char* pName, const char* pPwd, lpCardNode* ppCard, int* pIndex)
{
    lpCardNode cardNode = NULL;
    int nIndex = 0;

    //获取文件中的卡信息
    if (FALSE == getCard()) {
        return LOGON_FAIL;
    }

    cardNode = cardList->next;

    //遍历链表，判断能否进行上机
    while (cardNode != NULL) {
        if (strcmp(cardNode->data.aName, pName) == 0 &&
            strcmp(cardNode->data.aPwd, pPwd) == 0) {

            //只有状态为未上机(0)的卡才能进行上机
            if (cardNode->data.nStatus != 0) {
                return LOGON_CARD_INVALID;   // 该卡不能使用
            }
            //上机卡的余额必须大于0
            if (cardNode->data.fBalance <= 0) {
                return LOGON_BALANCE_LOW;    // 余额不足
            }

            // 返回找到的节点和索引
            if (ppCard != NULL) {
                *ppCard = cardNode;
            }
            if (pIndex != NULL) {
                *pIndex = nIndex;
            }
            return LOGON_SUCCESS;  // 找到可上机的卡
        }
        cardNode = cardNode->next;
        nIndex++;
    }
    return LOGON_FAIL;  // 未找到卡号密码匹配的卡
}

// 只查找卡，不检查状态和余额
int findCard(const char* pName, const char* pPwd, lpCardNode* ppCard, int* pIndex)
{
    lpCardNode cardNode = NULL;
    int nIndex = 0;

    if (FALSE == getCard()) {
        return FALSE;
    }

    cardNode = cardList->next;

    while (cardNode != NULL) {
        if (strcmp(cardNode->data.aName, pName) == 0 &&
            strcmp(cardNode->data.aPwd, pPwd) == 0) {
            if (ppCard != NULL) {
                *ppCard = cardNode;
            }
            if (pIndex != NULL) {
                *pIndex = nIndex;
            }
            return TRUE;  // 找到卡
        }
        cardNode = cardNode->next;
        nIndex++;
    }
    return FALSE;  // 未找到
}