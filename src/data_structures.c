#include "data_structures.h"

#include <ctype.h>
#include <string.h>

// 获取消费品类名称
const char* getTypeName(ConsumptionType type) {
    switch (type) {
        case FOOD: return "食品";
        case DAILY_USE: return "日用品";
        case CLOTHING: return "服装";
        case ELECTRIC: return "电器";
        case UTILITIES: return "水电煤气";
        case MORTGAGE: return "房贷";
        case TUITION: return "学费";
        case TRANSPORT: return "交通费";
        default: return "未知";
    }
}

// 从名称获取消费品类
ConsumptionType getTypeFromName(const char* name) {
    if (strcmp(name, "食品") == 0) return FOOD;
    if (strcmp(name, "日用品") == 0) return DAILY_USE;
    if (strcmp(name, "服装") == 0) return CLOTHING;
    if (strcmp(name, "电器") == 0) return ELECTRIC;
    if (strcmp(name, "水电煤气") == 0) return UTILITIES;
    if (strcmp(name, "房贷") == 0) return MORTGAGE;
    if (strcmp(name, "学费") == 0) return TUITION;
    if (strcmp(name, "交通费") == 0) return TRANSPORT;
    return FOOD; // 默认值
}

// 验证日期是否有效
bool isValidDate(int year, int month, int day) {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;

    // 每月天数
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // 闰年处理
    if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
        daysInMonth[2] = 29;
    }

    return day >= 1 && day <= daysInMonth[month];
}

// 解析日期字符串
bool parseDate(const char* str, Date* date) {
    if (strlen(str) != 8) return false;

    for (int i = 0; i < 8; i++) {
        if (!isdigit(str[i])) return false;
    }

    date->year = (str[0] - '0') * 1000 + (str[1] - '0') * 100 +
                 (str[2] - '0') * 10 + (str[3] - '0');
    date->month = (str[4] - '0') * 10 + (str[5] - '0');
    date->day = (str[6] - '0') * 10 + (str[7] - '0');

    return isValidDate(date->year, date->month, date->day);
}
