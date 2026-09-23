#ifndef GLOBAL_H
#define GLOBAL_H

//宏定义
#define FALSE 0
#define TRUE 1
#define CARDPATH  "data\\card.txt"  //卡信息保存路径
#define TIMELENGTH 30   //时间字符串缓冲区大小
#define BILLINGPATH "data\\billing.ams"  //消费信息保存路径

// 上机结果返回值常量
#define LOGON_FAIL 0        // 上机失败
#define LOGON_SUCCESS 1     // 上机成功
#define LOGON_CARD_INVALID 2   // 该卡不能使用
#define LOGON_BALANCE_LOW 3     // 该卡的余额不足

// 计费相关常量
#define UNIT 15      // 最小收费单元（分钟）
#define CHARGE 0.5   // 每个收费单元的金额（元）

// 下机结果返回值常量
#define SETTLE_FAIL 0           // 下机失败
#define SETTLE_SUCCESS 1        // 下机成功
#define SETTLE_CARD_NOT_LOGON 2 // 该卡未上机
#define SETTLE_NOT_ENOUGH_MONEY 3 // 余额不足

// 充值退费相关常量（放在文件末尾）
#define MONEY_RECHARGE 0   // 充值
#define MONEY_REFUND 1     // 退费
#define MONEY_PATH "data\\money.ams"  // 充值退费信息保存路径

// 充值退费结果返回值常量
#define RECHARGE_SUCCESS 1     // 充值成功
#define RECHARGE_FAIL 0        // 充值失败
#define RECHARGE_CARD_INVALID 2 // 卡无效（已注销/过期等）
#define REFUND_SUCCESS 1       // 退费成功
#define REFUND_FAIL 0          // 退费失败
#define REFUND_NO_MONEY 2      // 余额不足


#endif // !GLOBAL_H