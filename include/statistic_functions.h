#ifndef FAMILYEXPENSESYSTEM_STATISTIC_FUNCTIONS_H
#define FAMILYEXPENSESYSTEM_STATISTIC_FUNCTIONS_H

#include "data_structures.h"

// 统计功能函数声明
double calculateTotalAmount(ExpenseNode* head);
double calculateMemberTotal(ExpenseNode* head, const char* member, Date startDate, Date endDate);
double calculateTypeTotal(ExpenseNode* head, ConsumptionType type, Date startDate, Date endDate);
void printStatistics(ExpenseNode* head, Date startDate, Date endDate);

#endif //FAMILYEXPENSESYSTEM_STATISTIC_FUNCTIONS_H