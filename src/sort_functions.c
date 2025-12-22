#include "sort_functions.h"
#include "query_functions.h"
#include<stddef.h>
#include <stdio.h>
// 比较两个日期
int compareDates(Date d1, Date d2) {
    if (d1.year != d2.year) return d1.year - d2.year;
    if (d1.month != d2.month) return d1.month - d2.month;
    return d1.day - d2.day;
}

// 按日期排序
ExpenseNode* sortByDate(ExpenseNode* head, bool ascending) {
    if (head == NULL || head->next == NULL) return head;
    
    bool swapped;
    ExpenseNode* lptr = NULL;
    
    do {
        swapped = false;
        ExpenseNode *ptr1 = head;
        
        while (ptr1->next != lptr) {
            int cmp = compareDates(ptr1->data.date, ptr1->next->data.date);
            if ((ascending && cmp > 0) || (!ascending && cmp < 0)) {
                // 交换数据
                ExpenseRecord temp = ptr1->data;
                ptr1->data = ptr1->next->data;
                ptr1->next->data = temp;
                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    return head;
}

// 按金额排序
ExpenseNode* sortByAmount(ExpenseNode* head, bool ascending) {
    if (head == NULL || head->next == NULL) return head;
    
    bool swapped;
    ExpenseNode* lptr = NULL;
    
    do {
        swapped = false;
        ExpenseNode *ptr1 = head;
        
        while (ptr1->next != lptr) {
            if ((ascending && ptr1->data.amount > ptr1->next->data.amount) || 
                (!ascending && ptr1->data.amount < ptr1->next->data.amount)) {
                // 交换数据
                ExpenseRecord temp = ptr1->data;
                ptr1->data = ptr1->next->data;
                ptr1->next->data = temp;
                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    return head;
}

// 通用排序函数
ExpenseNode* sortList(ExpenseNode* head, int sortBy, bool ascending) {
    switch (sortBy) {
        case 1: // 按日期排序
            return sortByDate(head, ascending);
        case 2: // 按金额排序
            return sortByAmount(head, ascending);
        default:
            printf("无效的排序方式!\n");
            return head;
    }
}