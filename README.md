# 计费管理系统

一个基于 C 语言开发的网吧/机房计费管理系统，使用链表和文件存储实现卡信息管理、上机下机计费、充值退费、查询统计等功能。

## 功能特性

- **卡管理**：添加卡、精准查询、模糊查询、注销卡，支持卡号重复检查
- **上机下机**：验证卡号密码、检查卡状态和余额、自动记录上机时间
- **消费计费**：按15分钟为一个计费单元，每单元0.5元，不足15分钟按15分钟计算
- **充值与退费**：支持卡内余额充值、退费，记录每笔资金流水
- **查询统计**：统计总卡数、正在上机数、已注销数、总充值金额、总消费金额、剩余余额，并显示充值退费明细
- **数据持久化**：卡信息保存为文本文件，消费记录和充值退费记录保存为二进制文件

## 项目结构
计费管理系统/
├── main.c // 程序入口，菜单循环
├── menu.c / menu.h // 界面层：用户交互
├── service.c / service.h // 业务逻辑层：核心业务处理
├── card_service.c / card_service.h // 卡链表操作层
├── billing_service.c / billing_service.h // 消费链表操作层
├── card_file.c / card_file.h // 卡文件读写层
├── billing_file.c / billing_file.h // 消费文件读写层
├── money_file.c / money_file.h // 充值退费文件读写层
├── tool.c / tool.h // 工具层：时间转换
├── model.h // 数据结构定义
├── global.h // 全局常量定义
└── data/ // 数据存储目录
├── card.txt // 卡信息（文本）
├── billing.ams // 消费记录（二进制）
└── money.ams // 充值退费记录（二进制）

## 架构设计

项目采用五层架构，各层职责清晰：

| 层级 | 文件 | 职责 |
|------|------|------|
| 界面层 | main.c, menu.c | 显示菜单、获取输入、显示结果 |
| 业务逻辑层 | service.c | 上机、下机、充值、退费、统计、注销 |
| 链表操作层 | card_service.c, billing_service.c | 内存链表增删改查 |
| 文件操作层 | card_file.c, billing_file.c, money_file.c | 文件读写 |
| 工具层 | tool.c, model.h, global.h | 时间转换、数据结构、常量 |

## 核心数据结构

**卡信息结构体 Card**
```
typedef struct Card {
    char aName[18];      // 卡号
    char aPwd[8];        // 密码
    int nStatus;         // 状态：0-未上机，1-上机中，2-已注销，3-失效
    float fTotalUse;     // 累计金额
    int nUseCount;       // 使用次数
    float fBalance;      // 余额
    int nDel;            // 删除标识
    time_t tStart;       // 开卡时间
    time_t tEnd;         // 截止时间
    time_t tLast;        // 最后使用时间
} Card;
```

卡链表结点 CardNode
```
typedef struct CardNode {
    Card data;
    struct CardNode* next;
} CardNode, *lpCardNode;
```
消费记录结构体 Billing
```
typedef struct Billing {
    char aCardName[18];  // 卡号
    time_t tStart;       // 上机时间
    time_t tEnd;         // 下机时间
    float fAmount;       // 消费金额
    int nStatus;         // 0-未结算，1-已结算
    int nDel;            // 删除标识
} Billing;
```
消费链表结点 BillingNode
```
typedef struct BillingNode {
    Billing data;
    struct BillingNode* next;
} BillingNode, *lpBillingNode;
```

充值退费结构体 Money
```
typedef struct Money {
    char aCardName[18];  // 卡号
    time_t tTime;        // 充值退费的时间
    int nStatus;         // 状态：0-充值，1-退费
    float fMoney;        // 充值退费金额
    int nDel;            // 删除标识
} Money;
```
计费规则
最小收费单元：15分钟

每单元收费：0.5元

不足15分钟按15分钟计算

计费公式：金额 = ceil(分钟数 ÷ 15) × 0.5

编译与运行
环境要求：Visual Studio（Windows）

编译步骤：

使用 Visual Studio 打开项目

确保项目目录下存在 data 文件夹

按 F5 编译并运行

运行效果：

text
欢迎进入计费管理系统
----------菜单----------
1.添加卡
2.查询卡
3.上机
4.下机
5.充值
6.退费
7.查询统计
8.注销卡
0.退出
请选择菜单项编号(0-8):
使用说明
添加卡：输入卡号（1-18位）、密码（1-8位）、开卡金额，系统自动计算截止时间（开卡时间+1年）

查询卡：支持精准查询（卡号完全匹配）和模糊查询（输入部分卡号即可）

上机：输入卡号和密码，验证通过后卡状态变为上机中

下机：输入卡号和密码，系统自动计算消费金额并从余额扣除

充值/退费：输入卡号、密码和金额，系统更新余额并记录流水

查询统计：显示总卡数、正在上机数、已注销数、总充值金额、总消费金额、剩余余额及充值退费明细

注销卡：输入卡号和密码，退还余额，卡状态改为已注销

文件存储格式
card.txt（文本文件）

text
卡号##密码##状态##开卡时间##截止时间##累计金额##最后使用时间##使用次数##余额##删除标识
billing.ams（二进制文件）
每条记录按 sizeof(Billing) 字节存储

money.ams（二进制文件）
每条记录按 sizeof(Money) 字节存储

开发难点与解决
链表头指针丢失：遍历时使用临时指针，不移动头指针

文件路径错误：统一在 global.h 中定义路径宏

时间转换错误：stringToTime 中 tm_year 减1900，tm_mon 减1

数据同步：每次操作同时更新链表和文件

查询记录错误：queryBilling 取上机时间最新的未结算记录

内存泄漏：malloc 后必须有对应的 free，程序退出时释放链表

技术要点
带头结点的单向链表

动态内存分配（malloc、realloc、free）

文本文件与二进制文件读写

时间戳与字符串转换

模块化分层设计

Visual Studio 安全函数（strcpy_s、fopen_s、localtime_s）


