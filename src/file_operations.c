#include "file_operations.h"
#include <stdio.h>
#include<list_operations.h>

// 检查文件是否存在
bool fileExists(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// 从文件加载数据
bool loadFromFile(const char* filename, ExpenseNode** head) {
    if (!fileExists(filename)) {
        printf("数据文件不存在，将创建新文件。\n");
        return false;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("无法打开文件: %s\n", filename);
        return false;
    }

    // 读取记录数量
    int count;
    fread(&count, sizeof(int), 1, file);

    // 读取所有记录
    for (int i = 0; i < count; i++) {
        ExpenseRecord record;
        if (fread(&record, sizeof(ExpenseRecord), 1, file) != 1) {
            printf("读取记录失败!\n");
            break;
        }

        *head = addExpense(*head, record);
    }

    fclose(file);
    return true;
}

// 保存数据到文件
bool saveToFile(const char* filename, ExpenseNode* head) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("无法打开文件进行写入: %s\n", filename);
        return false;
    }

    // 写入记录数量
    int count = getListLength(head);
    fwrite(&count, sizeof(int), 1, file);

    // 写入所有记录
    ExpenseNode* current = head;
    while (current != NULL) {
        fwrite(&(current->data), sizeof(ExpenseRecord), 1, file);
        current = current->next;
    }

    fclose(file);
    return true;
}

// 创建示例数据
void createSampleData(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("无法创建示例数据文件: %s\n", filename);
        return;
    }

    // 创建5条示例数据
    int count = 5;
    fwrite(&count, sizeof(int), 1, file);

    ExpenseRecord records[5] = {
        {{2020, 3, 9}, "mother", 203.5, FOOD, "微信", "美团", "肉、鱼、青菜"},
        {{2020, 4, 7}, "son", 323.0, CLOTHING, "支付宝", "京东", "耐克足球鞋"},
        {{2020, 4, 15}, "father", 1500.0, MORTGAGE, "银行", "银行", "房贷"},
        {{2020, 4, 20}, "mother", 85.0, UTILITIES, "支付宝", "燃气公司", "燃气费"},
        {{2020, 4, 25}, "son", 25.0, TRANSPORT, "微信", "公交", "公交卡充值"}
    };

    for (int i = 0; i < count; i++) {
        fwrite(&records[i], sizeof(ExpenseRecord), 1, file);
    }

    fclose(file);
    printf("已创建示例数据文件: %s\n", filename);
}