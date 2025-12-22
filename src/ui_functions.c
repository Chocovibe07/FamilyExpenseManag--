#include "ui_functions.h"
#include "query_functions.h"
#include<stddef.h>
#include <list_operations.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include"file_operations.h"
#include "sort_functions.h"
#include "statistic_functions.h"

// 显示主菜单
void showMainMenu(ExpenseNode** head) {
    const char* options[] = {
        "查询消费记录",
        "排序消费记录",
        "统计消费情况",
        "录入新消费记录",
        "更新消费记录",
        "显示所有记录",
        "删除记录",
        "退出系统"
    };
    
    while (1) {
        printMenu("家庭日常消费记录管理系统", options, 8);
        
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                showQueryMenu(*head);
                break;
            case 2:
                showSortMenu(head);
                break;
            case 3:
                showStatisticMenu(*head);
                break;
            case 4:
                showAddMenu(head);
                break;
            case 5:
                showUpdateMenu(head);
                break;
            case 6:
                printHeader("所有消费记录");
                printList(*head);
                pressAnyKeyToContinue();
                break;
            case 7:
                showDeleteMenu(head);
                break;
            case 8:
                // 保存数据到文件
                saveToFile("data/fee.dat", *head);
                printf("数据已保存到文件。\n");
                freeList(*head);
                exit(0);
            default:
                printf("无效的选择，请重新输入。\n");
                pressAnyKeyToContinue();
        }
    }
}

// 显示查询菜单
void showQueryMenu(ExpenseNode* head) {
    const char* options[] = {
        "按日期区间段查询",
        "按交易金额查询",
        "按家庭成员加日期区间查询",
        "按消费品类加日期区间查询",
        "返回主菜单"
    };
    
    while (1) {
        printMenu("查询消费记录", options, 5);
        
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        
        if (choice == 5) return;
        
        Date startDate, endDate;
        float minAmount, maxAmount;
        char member[20];

        switch (choice) {
            case 1: // 按日期区间查询
                printf("请输入开始日期(YYYYMMDD): ");
                char startDateStr[9];
                scanf("%8s", startDateStr);
                clearInputBuffer();
                
                printf("请输入结束日期(YYYYMMDD): ");
                char endDateStr[9];
                scanf("%8s", endDateStr);
                clearInputBuffer();
                
                if (parseDate(startDateStr, &startDate) && parseDate(endDateStr, &endDate)) {
                    ExpenseNode* results = queryByDateRange(head, startDate, endDate);
                    printQueryResults(results);
                } else {
                    printf("日期格式错误!\n");
                }
                break;
                
            case 2: // 按金额查询
                printf("请输入最小金额: ");
                scanf("%f", &minAmount);
                clearInputBuffer();
                
                printf("请输入最大金额: ");
                scanf("%f", &maxAmount);
                clearInputBuffer();
                
                ExpenseNode* amountResults = queryByAmount(head, minAmount, maxAmount);
                printQueryResults(amountResults);
                break;
                
            case 3: // 按成员和日期查询
                printf("请输入家庭成员(如father, mother等): ");
                scanf("%19s", member);
                clearInputBuffer();
                
                printf("请输入开始日期(YYYYMMDD): ");
                scanf("%8s", startDateStr);
                clearInputBuffer();
                
                printf("请输入结束日期(YYYYMMDD): ");
                scanf("%8s", endDateStr);
                clearInputBuffer();
                
                if (parseDate(startDateStr, &startDate) && parseDate(endDateStr, &endDate)) {
                    ExpenseNode* memberResults = queryByMemberAndDate(head, member, startDate, endDate);
                    printQueryResults(memberResults);
                } else {
                    printf("日期格式错误!\n");
                }
                break;
                
            case 4: // 按品类和日期查询
                printf("请选择消费品类:\n");
                for (int i = 0; i < CONSUMPTION_TYPE_COUNT; i++) {
                    printf("%d. %s\n", i + 1, getTypeName((ConsumptionType)i));
                }
                printf("选择(1-%d): ", CONSUMPTION_TYPE_COUNT);
                
                int typeChoice;
                scanf("%d", &typeChoice);
                clearInputBuffer();
                
                if (typeChoice >= 1 && typeChoice <= CONSUMPTION_TYPE_COUNT) {
                    ConsumptionType type = (ConsumptionType) (typeChoice - 1);
                    
                    printf("请输入开始日期(YYYYMMDD): ");
                    scanf("%8s", startDateStr);
                    clearInputBuffer();
                    
                    printf("请输入结束日期(YYYYMMDD): ");
                    scanf("%8s", endDateStr);
                    clearInputBuffer();
                    
                    if (parseDate(startDateStr, &startDate) && parseDate(endDateStr, &endDate)) {
                        ExpenseNode* typeResults = queryByTypeAndDate(head, type, startDate, endDate);
                        printQueryResults(typeResults);
                    } else {
                        printf("日期格式错误!\n");
                    }
                } else {
                    printf("无效的消费品类选择!\n");
                }
                break;
                
            default:
                printf("无效的选择，请重新输入。\n");
        }
        
        pressAnyKeyToContinue();
    }
}

// 显示排序菜单
void showSortMenu(ExpenseNode** head) {
    const char* options[] = {
        "按消费日期排序",
        "按消费金额排序",
        "返回主菜单"
    };
    
    while (1) {
        printMenu("排序消费记录", options, 3);
        
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        
        if (choice == 3) return;
        
        if (choice < 1 || choice > 2) {
            printf("无效的选择，请重新输入。\n");
            pressAnyKeyToContinue();
            continue;
        }
        
        printf("排序方式(1.升序 2.降序): ");
        int orderChoice;
        scanf("%d", &orderChoice);
        clearInputBuffer();
        
        bool ascending = (orderChoice == 1);
        
        *head = sortList(*head, choice, ascending);
        
        printf("排序完成!\n");
        printList(*head);
        pressAnyKeyToContinue();
    }
}

// 显示统计菜单
void showStatisticMenu(ExpenseNode* head) {
    const char* options[] = {
        "给定时间段家庭消费总额统计",
        "特定成员在给定时间段消费总额统计",
        "特定品类在给定时间段消费总额统计",
        "返回主菜单"
    };
    
    while (1) {
        printMenu("统计消费情况", options, 4);
        
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        
        if (choice == 4) return;
        
        Date startDate, endDate;
        char startDateStr[9], endDateStr[9];
        
        printf("请输入开始日期(YYYYMMDD): ");
        scanf("%8s", startDateStr);
        clearInputBuffer();
        
        printf("请输入结束日期(YYYYMMDD): ");
        scanf("%8s", endDateStr);
        clearInputBuffer();
        
        if (!parseDate(startDateStr, &startDate) || !parseDate(endDateStr, &endDate)) {
            printf("日期格式错误!\n");
            pressAnyKeyToContinue();
            continue;
        }
        
        switch (choice) {
            case 1: // 家庭总消费
                printHeader("家庭消费统计");
                printStatistics(head, startDate, endDate);
                break;
                
            case 2: { // 特定成员
                char member[20];
                printf("请输入家庭成员: ");
                scanf("%19s", member);
                clearInputBuffer();
                
                float total = calculateMemberTotal(head, member, startDate, endDate);
                printf("\n%s 在 %s 至 %s 期间的消费总额: %.2f\n", 
                       member, startDateStr, endDateStr, total);
                break;
            }
                
            case 3: { // 特定品类
                printf("请选择消费品类:\n");
                for (int i = 0; i < CONSUMPTION_TYPE_COUNT; i++) {
                    printf("%d. %s\n", i + 1, getTypeName((ConsumptionType)i));
                }
                printf("选择(1-%d): ", CONSUMPTION_TYPE_COUNT);
                
                int typeChoice;
                scanf("%d", &typeChoice);
                clearInputBuffer();
                
                if (typeChoice >= 1 && typeChoice <= CONSUMPTION_TYPE_COUNT) {
                    ConsumptionType type = (ConsumptionType)(typeChoice - 1);
                    float total = calculateTypeTotal(head, type, startDate, endDate);
                    
                    printf("\n%s 在 %s 至 %s 期间的消费总额: %.2f\n", 
                           getTypeName(type), startDateStr, endDateStr, total);
                } else {
                    printf("无效的消费品类选择!\n");
                }
                break;
            }
                
            default:
                printf("无效的选择，请重新输入。\n");
        }
        
        pressAnyKeyToContinue();
    }
}

// 显示添加菜单
void showAddMenu(ExpenseNode** head) {
    ExpenseRecord record;
    
    printf("\n");
    printf("**************************************************\n");
    printf("* 录入新消费记录 *\n");
    printf("**************************************************\n");
    
    // 输入日期
    char dateStr[9];
    printf("请输入消费日期(YYYYMMDD): ");
    scanf("%8s", dateStr);
    clearInputBuffer();
    
    while (!parseDate(dateStr, &record.date)) {
        printf("日期格式错误，请重新输入(YYYYMMDD): ");
        scanf("%8s", dateStr);
        clearInputBuffer();
    }
    
    // 输入成员
    printf("请输入成员身份: ");
    scanf("%19s", record.member);
    clearInputBuffer();
    
    // 输入金额
    printf("请输入消费金额: ");
    scanf("%lf", &record.amount);
    clearInputBuffer();
    
    // 输入品类
    printf("请选择消费品类:\n");
    for (int i = 0; i < CONSUMPTION_TYPE_COUNT; i++) {
        printf("%d. %s\n", i + 1, getTypeName((ConsumptionType)i));
    }
    printf("选择(1-%d): ", CONSUMPTION_TYPE_COUNT);
    
    int typeChoice;
    scanf("%d", &typeChoice);
    clearInputBuffer();
    
    while (typeChoice < 1 || typeChoice > CONSUMPTION_TYPE_COUNT) {
        printf("无效的选择，请重新输入(1-%d): ", CONSUMPTION_TYPE_COUNT);
        scanf("%d", &typeChoice);
        clearInputBuffer();
    }
    
    record.type = (ConsumptionType)(typeChoice - 1);
    
    // 输入支出方式
    printf("请输入支出方式: ");
    scanf("%19s", record.method);
    clearInputBuffer();
    
    // 输入消费场所
    printf("请输入消费场所: ");
    scanf("%49s", record.site);
    clearInputBuffer();
    
    // 输入商品详情
    printf("请输入商品详情: ");
    scanf("%99s", record.detail);
    clearInputBuffer();
    
    *head = addExpense(*head, record);
    printf("消费记录添加成功!\n");
    
    // 保存到文件
    saveToFile("data/fee.dat", *head);
}

// 显示更新菜单
void showUpdateMenu(ExpenseNode** head) {
    if (*head == NULL) {
        printf("没有消费记录可供更新。\n");
        pressAnyKeyToContinue();
        return;
    }
    
    printHeader("所有消费记录");
    printList(*head);
    
    int index;
    printf("\n请输入要修改的记录编号(1-%d): ", getListLength(*head));
    scanf("%d", &index);
    clearInputBuffer();
    
    index--; // 转换为0-based索引
    
    if (index < 0 || index >= getListLength(*head)) {
        printf("无效的记录编号!\n");
        pressAnyKeyToContinue();
        return;
    }
    
    ExpenseRecord* record = getExpenseAt(*head, index);
    if (record == NULL) {
        printf("无法获取指定记录!\n");
        pressAnyKeyToContinue();
        return;
    }
    
    printf("\n");
    printf("**************************************************\n");
    printf("* 更新消费记录 *\n");
    printf("**************************************************\n");
    
    // 显示当前记录
    printf("当前记录:\n");
    Date d = record->date;
    char dateStr[11];
    sprintf(dateStr, "%04d%02d%02d", d.year, d.month, d.day);
    
    printf("日期: %s\n", dateStr);
    printf("成员: %s\n", record->member);
    printf("金额: %.2f\n", record->amount);
    printf("品类: %s\n", getTypeName(record->type));
    printf("方式: %s\n", record->method);
    printf("场所: %s\n", record->site);
    printf("详情: %s\n", record->detail);
    
    // 询问是否修改
    char confirm;
    printf("\n是否要修改此记录?(y/n): ");
    scanf(" %c", &confirm);
    clearInputBuffer();
    
    if (confirm != 'y' && confirm != 'Y') {
        return;
    }
    
    // 修改记录
    ExpenseRecord newRecord = *record;

    printf("请输入新的消费日期(YYYYMMDD, 回车保留原值): ");
    fgets(dateStr, 9, stdin);
    dateStr[strcspn(dateStr, "\n")] = 0; // 移除换行符
    
    if (strlen(dateStr) > 0 && parseDate(dateStr, &newRecord.date)) {
        // 日期已更新
    }
    
    printf("请输入新的成员身份(回车保留原值): ");
    fgets(newRecord.member, 20, stdin);
    newRecord.member[strcspn(newRecord.member, "\n")] = 0;
    if (strlen(newRecord.member) == 0) {
        strcpy(newRecord.member, record->member);
    }
    
    char amountStr[20];
    printf("请输入新的消费金额(回车保留原值): ");
    fgets(amountStr, 20, stdin);
    amountStr[strcspn(amountStr, "\n")] = 0;
    if (strlen(amountStr) > 0) {
        newRecord.amount = atof(amountStr);
    }

    printf("请选择新的消费品类(1-%d, 0保留原值): ", CONSUMPTION_TYPE_COUNT);
    fgets(dateStr, 9, stdin);
    dateStr[strcspn(dateStr, "\n")] = 0;
    
    if (strlen(dateStr) > 0 && atoi(dateStr) > 0 && atoi(dateStr) <= CONSUMPTION_TYPE_COUNT) {
        int typeChoice = atoi(dateStr);
        newRecord.type = (ConsumptionType)(typeChoice - 1);
    }
    
    printf("请输入新的支出方式(回车保留原值): ");
    fgets(newRecord.method, 20, stdin);
    newRecord.method[strcspn(newRecord.method, "\n")] = 0;
    if (strlen(newRecord.method) == 0) {
        strcpy(newRecord.method, record->method);
    }
    
    printf("请输入新的消费场所(回车保留原值): ");
    fgets(newRecord.site, 50, stdin);
    newRecord.site[strcspn(newRecord.site, "\n")] = 0;
    if (strlen(newRecord.site) == 0) {
        strcpy(newRecord.site, record->site);
    }
    
    printf("请输入新的商品详情(回车保留原值): ");
    fgets(newRecord.detail, 100, stdin);
    newRecord.detail[strcspn(newRecord.detail, "\n")] = 0;
    if (strlen(newRecord.detail) == 0) {
        strcpy(newRecord.detail, record->detail);
    }
    
    *head = updateExpense(*head, index, newRecord);
    printf("记录更新成功!\n");

    // 保存到文件
    saveToFile("data/fee.dat", *head);
    pressAnyKeyToContinue();
}

    /* 删除记录的界面与逻辑 */
    void showDeleteMenu(ExpenseNode**head){
        if (head == NULL || *head == NULL) {
            printf("\n没有可删除的记录。\n");
            pressAnyKeyToContinue();
            return;
        }

        printHeader("删除记录");
        printList(*head);

        printf("请输入要删除的记录索引（从0开始），输入-1取消: ");
        int idx;
        if (scanf("%d", &idx) != 1) {
            clearInputBuffer();
            printf("输入无效。\n");
            pressAnyKeyToContinue();
            return;
        }
        clearInputBuffer();

        if (idx == -1) {
            return;
        }

        int len = getListLength(*head);
        if (idx < 0 || idx >= len) {
            printf("无效的索引: %d\n", idx);
            pressAnyKeyToContinue();
            return;
        }

        /* 调用链表删除函数并持久化 */
        *head = deleteExpense(*head, idx);
        if (saveToFile("data/fee.dat", *head)) {
            printf("删除成功并已保存。\n");
        } else {
            printf("删除成功但保存失败。\n");
        }

        pressAnyKeyToContinue();
    }
