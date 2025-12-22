#ifndef FAMILYEXPENSESYSTEM_UI_FUNCTIONS_H
#define FAMILYEXPENSESYSTEM_UI_FUNCTIONS_H

#include "data_structures.h"

// 用户界面函数声明
void showMainMenu(ExpenseNode** head);
void showQueryMenu(ExpenseNode* head);
void showSortMenu(ExpenseNode** head);
void showStatisticMenu(ExpenseNode* head);
void showAddMenu(ExpenseNode** head);
void showUpdateMenu(ExpenseNode** head);
void showRecordForm(ExpenseRecord* record, bool isEditing);
void showDeleteMenu(ExpenseNode** head);

#endif //FAMILYEXPENSESYSTEM_UI_FUNCTIONS_H