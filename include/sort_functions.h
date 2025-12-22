#ifndef FAMILYEXPENSESYSTEM_SORT_FUNCTIONS_H
#define FAMILYEXPENSESYSTEM_SORT_FUNCTIONS_H

#include "data_structures.h"

// 排序功能函数声明
ExpenseNode* sortByDate(ExpenseNode* head, bool ascending);
ExpenseNode* sortByAmount(ExpenseNode* head, bool ascending);
ExpenseNode* sortList(ExpenseNode* head, int sortBy, bool ascending);

#endif //FAMILYEXPENSESYSTEM_SORT_FUNCTIONS_H