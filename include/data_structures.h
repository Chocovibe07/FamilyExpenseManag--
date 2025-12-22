#ifndef FAMILYEXPENSESYSTEM_DATA_STRUCTURES_H
#define FAMILYEXPENSESYSTEM_DATA_STRUCTURES_H

#include "common.h"

// 消费品类枚举
typedef enum {
    FOOD,        // 食品
    DAILY_USE,   // 日用品
    CLOTHING,    // 服装
    ELECTRIC,    // 电器
    UTILITIES,   // 水电煤气
    MORTGAGE,    // 房贷
    TUITION,     // 学费
    TRANSPORT,   // 交通费
    CONSUMPTION_TYPE_COUNT
} ConsumptionType;

// 消费记录结构体
typedef struct {
    int year;
    int month;
    int day;
} Date;

typedef struct {
    Date date;                // 消费日期
    char member[20];          // 成员身份
    double amount;             // 消费金额
    ConsumptionType type;     // 消费品类
    char method[20];          // 支出方式
    char site[50];            // 消费场所
    char detail[100];         // 商品详情
} ExpenseRecord;

// 链表节点结构体
typedef struct ExpenseNode {
    ExpenseRecord data;
    struct ExpenseNode* next;
} ExpenseNode;

// 函数声明
const char* getTypeName(ConsumptionType type);
ConsumptionType getTypeFromName(const char* name);
bool isValidDate(int year, int month, int day);
bool parseDate(const char* str, Date* date);

#endif //FAMILYEXPENSESYSTEM_DATA_STRUCTURES_H