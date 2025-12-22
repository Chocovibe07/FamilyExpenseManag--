#ifndef FAMILYEXPENSESYSTEM_QUERY_FUNCTIONS_H
#define FAMILYEXPENSESYSTEM_QUERY_FUNCTIONS_H

#include "data_structures.h"

// 查询功能函数声明
ExpenseNode* queryByDateRange(ExpenseNode* head, Date startDate, Date endDate);
ExpenseNode* queryByAmount(ExpenseNode* head, float minAmount, float maxAmount);
ExpenseNode* queryByMemberAndDate(ExpenseNode* head, const char* member, Date startDate, Date endDate);
ExpenseNode* queryByTypeAndDate(ExpenseNode* head, ConsumptionType type, Date startDate, Date endDate);
void printQueryResults(ExpenseNode* results);

#endif //FAMILYEXPENSESYSTEM_QUERY_FUNCTIONS_H