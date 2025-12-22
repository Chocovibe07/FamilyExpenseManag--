#ifndef FAMILYEXPENSESYSTEM_FILE_OPERATIONS_H
#define FAMILYEXPENSESYSTEM_FILE_OPERATIONS_H

#include "data_structures.h"

// 文件操作函数声明
bool loadFromFile(const char* filename, ExpenseNode** head);
bool saveToFile(const char* filename, ExpenseNode* head);
bool fileExists(const char* filename);
void createSampleData(const char* filename);

#endif //FAMILYEXPENSESYSTEM_FILE_OPERATIONS_H