#include "list_operations.h"
#include <stdio.h>
#include<stdlib.h>
// 创建空链表
ExpenseNode* createList() {
    return NULL;
}

// 添加消费记录到链表
ExpenseNode* addExpense(ExpenseNode* head, ExpenseRecord record) {
    ExpenseNode* newNode = (ExpenseNode*)malloc(sizeof(ExpenseNode));
    if (!newNode) {
        printf("内存分配失败!\n");
        return head;
    }

    newNode->data = record;
    newNode->next = head;
    return newNode;
}

// 删除指定索引的记录
ExpenseNode* deleteExpense(ExpenseNode* head, int index) {
    if (index < 0) return head;

    if (index == 0) {
        ExpenseNode* temp = head;
        head = head->next;
        free(temp);
        return head;
    }

    ExpenseNode* current = head;
    for (int i = 0; current != NULL && i < index - 1; i++) {
        current = current->next;
    }

    if (current == NULL || current->next == NULL) {
        printf("无效的索引!\n");
        return head;
    }

    ExpenseNode* temp = current->next;
    current->next = temp->next;
    free(temp);
    return head;
}

// 更新指定索引的记录
ExpenseNode* updateExpense(ExpenseNode* head, int index, ExpenseRecord newRecord) {
    ExpenseNode* current = head;
    for (int i = 0; i < index && current != NULL; i++) {
        current = current->next;
    }

    if (current == NULL) {
        printf("无效的索引!\n");
        return head;
    }

    current->data = newRecord;
    return head;
}

// 释放链表内存
void freeList(ExpenseNode* head) {
    ExpenseNode* current = head;
    while (current != NULL) {
        ExpenseNode* temp = current;
        current = current->next;
        free(temp);
    }
}

// 获取链表长度
int getListLength(ExpenseNode* head) {
    int count = 0;
    ExpenseNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// 获取指定索引的记录
ExpenseRecord* getExpenseAt(ExpenseNode* head, int index) {
    ExpenseNode* current = head;
    for (int i = 0; i < index && current != NULL; i++) {
        current = current->next;
    }

    if (current == NULL) {
        return NULL;
    }

    return &(current->data);
}

// 打印链表内容
void printList(ExpenseNode* head) {
    if (head == NULL) {
        printf("没有消费记录。\n");
        return;
    }

    // 表头：各列固定宽度，详情列较宽并允许截断
    printf("\n%-10s %-8s %10s %-12s %-10s %-15s %-25s\n",
           "日期", "成员", "金额", "品类", "方式", "场所", "详情");
    for (int i = 0; i < 100; i++) putchar('-');
    putchar('\n');

    int count = 0;
    ExpenseNode* current = head;
    while (current != NULL) {
        Date d = current->data.date;
        char dateStr[11];
        sprintf(dateStr, "%04d-%02d-%02d", d.year, d.month, d.day);

        // 使用字段宽度与截断（如 %-8.8s 限制长度为8并左对齐）
        printf("%-10s %-8.8s %10.2f %-12.12s %-10.10s %-15.15s %-25.25s\n",
               dateStr,
               current->data.member,
               current->data.amount,
               getTypeName(current->data.type),
               current->data.method,
               current->data.site,
               current->data.detail);

        current = current->next;
        count++;
    }

    for (int i = 0; i < 100; i++) putchar('-');
    putchar('\n');
    printf("共 %d 条记录。\n", count);
}//
// Created by 33182 on 2025/12/16.
//