#include "statistic_functions.h"
#include "query_functions.h"
#include<stddef.h>
#include <string.h>
#include <stdio.h>
// 计算总金额
double calculateTotalAmount(ExpenseNode* head) {
    double total = 0.0;
    ExpenseNode* current = head;
    
    while (current != NULL) {
        total += current->data.amount;
        current = current->next;
    }
    
    return total;
}

// 计算成员总消费
double calculateMemberTotal(ExpenseNode *head, const char *member, Date startDate, Date endDate) {
    double total = 0.0;
    ExpenseNode* current = head;
    
    while (current != NULL) {
        Date d = current->data.date;
        bool withinRange = 
            (d.year > startDate.year || 
            (d.year == startDate.year && d.month > startDate.month) ||
            (d.year == startDate.year && d.month == startDate.month && d.day >= startDate.day))
            &&
            (d.year < endDate.year || 
            (d.year == endDate.year && d.month < endDate.month) ||
            (d.year == endDate.year && d.month == endDate.month && d.day <= endDate.day));
        
        if (withinRange && strcmp(current->data.member, member) == 0) {
            total += current->data.amount;
        }
        current = current->next;
    }
    
    return total;
}

// 计算品类总消费
double calculateTypeTotal(ExpenseNode* head, ConsumptionType type, Date startDate, Date endDate) {
    double total = 0.0;
    ExpenseNode* current = head;
    
    while (current != NULL) {
        Date d = current->data.date;
        bool withinRange = 
            (d.year > startDate.year || 
            (d.year == startDate.year && d.month > startDate.month) ||
            (d.year == startDate.year && d.month == startDate.month && d.day >= startDate.day))
            &&
            (d.year < endDate.year || 
            (d.year == endDate.year && d.month < endDate.month) ||
            (d.year == endDate.year && d.month == endDate.month && d.day <= endDate.day));
        
        if (withinRange && current->data.type == type) {
            total += current->data.amount;
        }
        current = current->next;
    }
    
    return total;
}

// 打印统计结果
void printStatistics(ExpenseNode* head, Date startDate, Date endDate) {
    printf("\n");
    printf("**************************************************\n");
    printf("* 统计结果 *\n");
    printf("**************************************************\n");
    
    char startDateStr[11], endDateStr[11];
    sprintf(startDateStr, "%04d%02d%02d", startDate.year, startDate.month, startDate.day);
    sprintf(endDateStr, "%04d%02d%02d", endDate.year, endDate.month, endDate.day);
    
    printf("统计时间范围: %s 至 %s\n", startDateStr, endDateStr);
    
    // 家庭总消费
    double total = calculateTotalAmount(head);
    printf("\n家庭总消费: %.2f\n", total);
    
    // 按成员统计
    printf("\n按成员统计:\n");
    for (int i = 0; i < 4; i++) {
        const char* members[] = {"father", "mother", "son", "daughter"};
        double memberTotal = calculateMemberTotal(head, members[i], startDate, endDate);
        if (memberTotal > 0) {
            printf("%-10s: %.2f\n", members[i], memberTotal);
        }
    }
    
    // 按品类统计
    printf("\n按品类统计:\n");
    for (int i = 0; i < CONSUMPTION_TYPE_COUNT; i++) {
        double typeTotal = calculateTypeTotal(head, (ConsumptionType)i, startDate, endDate);
        if (typeTotal > 0) {
            printf("%-10s: %.2f\n", getTypeName((ConsumptionType)i), typeTotal);
        }
    }
}