#include "query_functions.h"
#include<stddef.h>
#include <list_operations.h>
#include <string.h>
#include <stdio.h>
// 按日期范围查询
ExpenseNode* queryByDateRange(ExpenseNode* head, Date startDate, Date endDate) {
    ExpenseNode* result = NULL;
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
        
        if (withinRange) {
            result = addExpense(result, current->data);
        }
        current = current->next;
    }
    
    return result;
}

// 按金额查询
ExpenseNode* queryByAmount(ExpenseNode* head, float minAmount, float maxAmount) {
    ExpenseNode* result = NULL;
    ExpenseNode* current = head;
    
    while (current != NULL) {
        if (current->data.amount >= minAmount && current->data.amount <= maxAmount) {
            result = addExpense(result, current->data);
        }
        current = current->next;
    }
    
    return result;
}

// 按成员和日期范围查询
ExpenseNode* queryByMemberAndDate(ExpenseNode* head, const char* member, Date startDate, Date endDate) {
    ExpenseNode* result = NULL;
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
            result = addExpense(result, current->data);
        }
        current = current->next;
    }
    
    return result;
}

// 按品类和日期范围查询
ExpenseNode* queryByTypeAndDate(ExpenseNode* head, ConsumptionType type, Date startDate, Date endDate) {
    ExpenseNode* result = NULL;
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
            result = addExpense(result, current->data);
        }
        current = current->next;
    }
    
    return result;
}

// 打印查询结果
void printQueryResults(ExpenseNode* results) {
    if (results == NULL) {
        printf("没有找到符合条件的记录。\n");
        return;
    }
    
    printf("\n查询结果:\n");
    printList(results);
    freeList(results); // 释放查询结果链表
}