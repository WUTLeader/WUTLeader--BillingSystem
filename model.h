//加上头文件保护,这样在menu.c中定义card时可以直接用"Card card",而不需要用到"struct"
//如果没有,该头文件会在多文件包含时导致typedef定义被覆盖
#ifndef MODEL_H
#define MODEL_H

#include<time.h> //提供了时间类型time_t的数据类型
//定义卡信息结构体
typedef struct Card {
	char aName[18];  //卡号
	char aPwd[8];    //密码

	int nStatus;     // 卡状态(0-未上机；1-正在上机；2-已注销；3-失效)
	float fTotalUse; // 累计金额
	int nUseCount;   // 使用次数
	float fBalance;  // 余额
	int nDel;        // 删除标识(0-未删除,1-删除)

	time_t tStart;   //开卡时间
	time_t tEnd;     //截止时间
	time_t tLast; //最后使用时间
}Card;

//typedef struct Card Card;使用typedef关键字将"struct Card"重命名为"Card",便于后续使用简洁方便
//也可以按照上面写法直接重命名,更加简洁


//声明卡信息链表结点类型
typedef struct CardNode {
	Card data;
	struct CardNode* next;
}CardNode,*lpCardNode;//"*lpCardNode是指向结构体的指针


typedef struct Billing
{
	char aCardName[18];   // 卡号
	time_t tStart;    // 上机时间
	time_t tEnd;    // 下机时间
	float fAmount;    // 消费金额
	int nStatus;    // 消费状态，0-未结算，1-已结算
	int nDel;    // 删除标识，0-未删除，1-已删除
} Billing;

// 计费信息链表节点
typedef struct BillingNode {
	Billing data;
	struct BillingNode* next;
} BillingNode, * lpBillingNode;

// 上机信息结构体
typedef struct LogonInfo {
	char aCardName[18]; // 上机卡号
	time_t tLogon; // 上机时间
	float fBalance; // 上机时的卡余额
} LogonInfo;

// 下机信息结构体（添加在 LogonInfo 后面）
// 添加：下机信息结构体
typedef struct SettleInfo
{
	char aCardName[18];   // 卡号
	time_t tStart;        // 上机时间
	time_t tEnd;          // 下机时间
	float fAmount;        // 消费金额
	float fBalance;       // 余额
} SettleInfo;

// 充值退费结构体
typedef struct Money
{
	char aCardName[18]; // 卡号
	time_t tTime;       // 充值退费的时间
	int nStatus;        // 状态：0-表示充值；1-表示退费
	float fMoney;       // 充值退费金额
	int nDel;           // 删除标识，0-未删除；1-删除
} Money;

// 充值退费信息结构体
typedef struct MoneyInfo
{
	char aCardName[18]; // 卡号
	float fMoney;       // 充值退费金额
	float fBalance;     // 操作后的余额
} MoneyInfo;

#endif // !MODEL_H

