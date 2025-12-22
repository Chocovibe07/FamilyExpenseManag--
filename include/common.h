#ifndef FAMILYEXPENSESYSTEM_COMMON_H
#define FAMILYEXPENSESYSTEM_COMMON_H


// 定义布尔类型
typedef enum { false, true } bool;

// 通用函数声明
void clearInputBuffer();
void pressAnyKeyToContinue();
void printHeader(const char* title);
void printMenu(const char* title, const char* options[], int numOptions);

#endif //FAMILYEXPENSESYSTEM_COMMON_H