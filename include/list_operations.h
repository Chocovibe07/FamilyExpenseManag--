#ifndef FAMILYEXPENSESYSTEM_LIST_OPERATIONS_H
#define FAMILYEXPENSESYSTEM_LIST_OPERATIONS_H

#include "data_structures.h"

// 链表操作函数声明
ExpenseNode* createList();
ExpenseNode* addExpense(ExpenseNode* head, ExpenseRecord record);
ExpenseNode* deleteExpense(ExpenseNode* head, int index);
ExpenseNode* updateExpense(ExpenseNode* head, int index, ExpenseRecord newRecord);
void freeList(ExpenseNode* head);
int getListLength(ExpenseNode* head);
ExpenseRecord* getExpenseAt(ExpenseNode* head, int index);
void printList(ExpenseNode* head);

#endif //FAMILYEXPENSESYSTEM_LIST_OPERATIONS_H