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

// 仅保留一份前置声明
static void printBar(const char* label, double value, double maxVal);
static void printTypeChart(ExpenseNode* head, Date startDate, Date endDate);
// 新增：预算求和前置声明，避免未定义引用
static double sumTotalByPeriod(ExpenseNode* head, Date s, Date e);

// 预算数据与持久化（唯一）
static Date budgetStart = (Date){0};
static Date budgetEnd   = (Date){0};
static double totalBudget = 0.0;
#define MAX_MEMBER_BUDGET 20
static char budgetMembers[MAX_MEMBER_BUDGET][20];
static double budgetAmounts[MAX_MEMBER_BUDGET];
static int budgetCount = 0;
static const char* BUDGET_FILE = "data/budget.dat";

static void saveBudgetData() {
    FILE* fp = fopen(BUDGET_FILE, "wb");
    if (!fp) return;
    fwrite(&budgetStart, sizeof(Date), 1, fp);
    fwrite(&budgetEnd,   sizeof(Date), 1, fp);
    fwrite(&totalBudget, sizeof(double), 1, fp);
    fwrite(&budgetCount, sizeof(int), 1, fp);
    fwrite(budgetMembers, sizeof(budgetMembers[0]), MAX_MEMBER_BUDGET, fp);
    fwrite(budgetAmounts, sizeof(double), MAX_MEMBER_BUDGET, fp);
    fclose(fp);
}

static void loadBudgetData() {
    if (!fileExists(BUDGET_FILE)) return;
    FILE* fp = fopen(BUDGET_FILE, "rb");
    if (!fp) return;
    fread(&budgetStart, sizeof(Date), 1, fp);
    fread(&budgetEnd,   sizeof(Date), 1, fp);
    fread(&totalBudget, sizeof(double), 1, fp);
    fread(&budgetCount, sizeof(int), 1, fp);
    fread(budgetMembers, sizeof(budgetMembers[0]), MAX_MEMBER_BUDGET, fp);
    fread(budgetAmounts, sizeof(double), MAX_MEMBER_BUDGET, fp);
    fclose(fp);
}

// 替换为竖向布局输出
static void printRecordVertical(int idx, ExpenseRecord* r) {
    printf("----------- 记录 %d -----------\n", idx);
    printf("日期   : %04d-%02d-%02d\n", r->date.year, r->date.month, r->date.day);
    printf("成员   : %s\n", r->member);
    printf("金额   : %.2f\n", r->amount);
    printf("品类   : %s\n", getTypeName(r->type));
    printf("方式   : %s\n", r->method);
    printf("场所   : %s\n", r->site);
    printf("详情   : %s\n", r->detail);
}

static void printListPretty(ExpenseNode* head, const int startIndex) {
    if (!head) {
        printf("\n当前没有可显示的记录。\n");
        return;
    }
    int idx = startIndex;
    for (ExpenseNode* p = head; p; p = p->next, ++idx) {
        printRecordVertical(idx, &p->data);
    }
}

// 新增：返回上一步判定
static bool isBackToken(const char* s) {
    return strcmp(s, "0") == 0 || strcmp(s, "b") == 0 || strcmp(s, "B") == 0;
}

// 新增：通用辅助函数
static int safeReadInt(int fallback) {
    int v;
    if (scanf("%d", &v) != 1) {
        clearInputBuffer();
        return fallback;
    }
    clearInputBuffer();
    return v;
}

static int countNodes(ExpenseNode* head) {
    int cnt = 0;
    for (ExpenseNode* p = head; p; p = p->next) cnt++;
    return cnt;
}

static void printQueryResultList(ExpenseNode* results) {
    if (!results) {
        printf("没有符合条件的记录。\n");
        return;
    }
    int cnt = countNodes(results);
    printf("共找到 %d 条记录：\n", cnt);
    printListPretty(results, 1);
}

// 新增：组合筛选辅助
static int dateToInt(Date d) { return d.year * 10000 + d.month * 100 + d.day; }

static bool matchesCriteria(const ExpenseRecord* r,
                            bool useDate, int startInt, int endInt,
                            bool useMin, double minA, bool useMax, double maxA,
                            bool useMember, const char* member,
                            bool useType, ConsumptionType type,
                            bool useMethod, const char* method,
                            bool useSite, const char* siteKey,
                            bool useDetail, const char* detailKey) {
    int di = dateToInt(r->date);
    if (useDate && (di < startInt || di > endInt)) return false;
    if (useMin && r->amount < minA) return false;
    if (useMax && r->amount > maxA) return false;
    if (useMember && strcmp(r->member, member) != 0) return false;
    if (useType && r->type != type) return false;
    if (useMethod && strcmp(r->method, method) != 0) return false;
    if (useSite && strstr(r->site, siteKey) == NULL) return false;
    if (useDetail && strstr(r->detail, detailKey) == NULL) return false;
    return true;
}

static ExpenseNode* filterByCriteria(
        ExpenseNode* head,
        bool useDate, int startInt, int endInt,
        bool useMin, double minA, bool useMax, double maxA,
        bool useMember, const char* member,
        bool useType, ConsumptionType type,
        bool useMethod, const char* method,
        bool useSite, const char* siteKey,
        bool useDetail, const char* detailKey) {
    ExpenseNode* res = NULL;
    for (ExpenseNode* p = head; p; p = p->next) {
        if (matchesCriteria(&p->data, useDate, startInt, endInt,
                            useMin, minA, useMax, maxA,
                            useMember, member, useType, type,
                            useMethod, method, useSite, siteKey,
                            useDetail, detailKey)) {
            res = addExpense(res, p->data);
        }
    }
    return res;
}

static void setTotalBudget() {
    char buf[16], startStr[16], endStr[16];
    printf("设置预算开始日期(YYYYMMDD，输入0返回): ");
    fgets(startStr, sizeof(startStr), stdin); startStr[strcspn(startStr, "\n")] = 0;
    if (isBackToken(startStr)) return;
    printf("设置预算结束日期(YYYYMMDD，输入0返回): ");
    fgets(endStr, sizeof(endStr), stdin); endStr[strcspn(endStr, "\n")] = 0;
    if (isBackToken(endStr)) return;
    if (!parseDate(startStr, &budgetStart) || !parseDate(endStr, &budgetEnd)) {
        printf("日期格式错误。\n");
        return;
    }
    printf("请输入家庭总预算金额: ");
    fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
    totalBudget = atof(buf);
    printf("家庭总预算已设置为 %.2f\n", totalBudget);
    saveBudgetData();
}

// 新增：修改家庭总预算（可调整时间段与金额）
static void updateTotalBudget() {
    if (dateToInt(budgetStart) == 0 || dateToInt(budgetEnd) == 0) {
        printf("尚未设置家庭总预算，请先设置。\n");
        return;
    }
    char startStr[16], endStr[16], buf[16];
    printf("当前预算区间: %04d%02d%02d - %04d%02d%02d，总预算: %.2f\n",
           budgetStart.year, budgetStart.month, budgetStart.day,
           budgetEnd.year, budgetEnd.month, budgetEnd.day,
           totalBudget);

    printf("新的开始日期(YYYYMMDD，回车保留): ");
    fgets(startStr, sizeof(startStr), stdin); startStr[strcspn(startStr, "\n")] = 0;
    if (strlen(startStr) > 0 && !parseDate(startStr, &budgetStart)) {
        printf("开始日期格式错误，保持原值。\n");
    }

    printf("新的结束日期(YYYYMMDD，回车保留): ");
    fgets(endStr, sizeof(endStr), stdin); endStr[strcspn(endStr, "\n")] = 0;
    if (strlen(endStr) > 0 && !parseDate(endStr, &budgetEnd)) {
        printf("结束日期格式错误，保持原值。\n");
    }

    printf("新的总预算金额(回车保留): ");
    fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
    if (strlen(buf) > 0) totalBudget = atof(buf);

    printf("家庭总预算已更新。\n");
    saveBudgetData();
}

static void setMemberBudget() {
    if (budgetCount >= MAX_MEMBER_BUDGET) {
        printf("成员预算数量已达上限。\n");
        return;
    }
    char name[20], buf[16];
    printf("请输入成员姓名(输入0返回): ");
    fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
    if (isBackToken(name)) return;
    printf("请输入该成员预算金额: ");
    fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
    strncpy(budgetMembers[budgetCount], name, sizeof(budgetMembers[0]) - 1);
    budgetMembers[budgetCount][sizeof(budgetMembers[0]) - 1] = '\0';
    budgetAmounts[budgetCount] = atof(buf);
    budgetCount++;
    printf("成员预算已记录。\n");
    saveBudgetData();
}

// 新增：修改已有成员预算
static void updateMemberBudget() {
    if (budgetCount == 0) {
        printf("暂无成员预算可修改。\n");
        return;
    }
    printf("当前成员预算列表：\n");
    for (int i = 0; i < budgetCount; ++i) {
        printf("%d) %-10s 预算: %.2f\n", i + 1, budgetMembers[i], budgetAmounts[i]);
    }
    printf("选择要修改的成员编号(1-%d，0返回): ", budgetCount);
    int idx = safeReadInt(0);
    if (idx == 0 || idx < 1 || idx > budgetCount) return;

    --idx; // 转为0基
    char name[20], buf[16];
    printf("成员姓名(回车保留原值): ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    if (strlen(name) > 0) {
        strncpy(budgetMembers[idx], name, sizeof(budgetMembers[idx]) - 1);
        budgetMembers[idx][sizeof(budgetMembers[idx]) - 1] = '\0';
    }

    printf("新的预算金额(回车保留原值): ");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = 0;
    if (strlen(buf) > 0) {
        budgetAmounts[idx] = atof(buf);
    }
    printf("成员预算已更新。\n");
    saveBudgetData();
}

static void showBudgetStatus(ExpenseNode* head) {
    if (dateToInt(budgetStart) == 0 || dateToInt(budgetEnd) == 0 || totalBudget <= 0) {
        printf("尚未设置家庭总预算。\n");
        return;
    }
    double totalSpent = sumTotalByPeriod(head, budgetStart, budgetEnd);
    printf("\n家庭预算区间 %04d-%02d-%02d 至 %04d-%02d-%02d\n",
           budgetStart.year, budgetStart.month, budgetStart.day,
           budgetEnd.year, budgetEnd.month, budgetEnd.day);
    printBar("总预算", totalBudget, totalBudget);
    printBar("已支出", totalSpent, totalBudget);
    if (totalSpent > totalBudget) printf("提示: 已超出总预算 %.2f\n", totalSpent - totalBudget);
    for (int i = 0; i < budgetCount; ++i) {
        double spent = calculateMemberTotal(head, budgetMembers[i], budgetStart, budgetEnd);
        printBar(budgetMembers[i], spent, budgetAmounts[i] > 0 ? budgetAmounts[i] : spent);
        if (budgetAmounts[i] > 0 && spent > budgetAmounts[i]) {
            printf("  └─ 超出成员预算 %.2f\n", spent - budgetAmounts[i]);
        }
    }
}

// 统计图表辅助（唯一）
static void printBar(const char* label, double value, double maxVal) {
    if (maxVal <= 0) maxVal = 1;
    int len = (int)((value / maxVal) * 50);
    if (len < 0) len = 0;
    const char* bar = "█";
    char lab[32];
    snprintf(lab, sizeof(lab), "%s", label);
    printf("%-16s │ ", lab);
    for (int i = 0; i < len; ++i) printf("%s", bar);
    printf(" (%.2f)\n", value);
}

static void printTypeChart(ExpenseNode* head, Date startDate, Date endDate) {
    double totals[CONSUMPTION_TYPE_COUNT] = {0};
    double maxVal = 0;
    for (int i = 0; i < CONSUMPTION_TYPE_COUNT; ++i) {
        totals[i] = calculateTypeTotal(head, (ConsumptionType)i, startDate, endDate);
        if (totals[i] > maxVal) maxVal = totals[i];
    }
    printf("\n按品类分布图:\n");
    for (int i = 0; i < CONSUMPTION_TYPE_COUNT; ++i) {
        printBar(getTypeName((ConsumptionType)i), totals[i], maxVal);
    }
}

// 修复：预算区间求和
static double sumTotalByPeriod(ExpenseNode *head, Date s, Date e) {
    int startInt = dateToInt(s), endInt = dateToInt(e);
    if (startInt > endInt) { int tmp = startInt; startInt = endInt; endInt = tmp; }
    double sum = 0.0;
    for (ExpenseNode* p = head; p; p = p->next) {
        int di = dateToInt(p->data.date);
        if (di >= startInt && di <= endInt) sum += p->data.amount;
    }
    return sum;
}

// 显示预算管理菜单
static void showBudgetMenu(ExpenseNode* head) {
    const char* opts[] = {
        "设置家庭总预算及时间段",
        "设置成员预算",
        "查看预算执行情况",
        "修改成员预算",
        "修改家庭总预算",
        "返回上级"
    };
    while (1) {
        printMenu("预算管理", opts, 6);
        int c = safeReadInt(-1);
        if (c == 6 || c == -1) return;
        switch (c) {
            case 1: setTotalBudget(); break;
            case 2: setMemberBudget(); break;
            case 3: showBudgetStatus(head); pressAnyKeyToContinue(); break;
            case 4: updateMemberBudget(); pressAnyKeyToContinue(); break;
            case 5: updateTotalBudget(); pressAnyKeyToContinue(); break;
            default: printf("无效选择。\n"); pressAnyKeyToContinue();
        }
    }
}

// 显示主菜单
void showMainMenu(ExpenseNode** head) {
    loadBudgetData();  // 仅调用一次
    const char* options[] = {
        "查询消费记录",
        "排序消费记录",
        "统计消费情况",
        "录入新消费记录",
        "更新消费记录",
        "显示所有记录",
        "删除记录",
        "预算管理",        // 新增
        "退出系统"
    };
    
    while (1) {
        printMenu("家庭日常消费记录管理系统", options, 9);
        
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
                printListPretty(*head, 1);
                pressAnyKeyToContinue();
                break;
            case 7:
                showDeleteMenu(head);
                break;
            case 8:
                showBudgetMenu(*head);
                break;
            case 9:
                saveToFile("data/fee.dat", *head);
                saveBudgetData();          // 退出前保存预算
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
        "多条件组合筛选",
        "返回主菜单"
    };
    
    Date startDate, endDate;
    char member[20];
    char startDateStr[9], endDateStr[9];
    char buf[128];
    
    while (1) {
        printMenu("查询消费记录", options, 6);
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        if (choice == 6) return;

        switch (choice) {
            case 1: { // 按日期区间查询
                printf("请输入开始日期(YYYYMMDD，输入0返回): ");
                scanf("%8s", startDateStr); clearInputBuffer();
                if (isBackToken(startDateStr)) break;
                printf("请输入结束日期(YYYYMMDD，输入0返回): ");
                scanf("%8s", endDateStr); clearInputBuffer();
                if (isBackToken(endDateStr)) break;
                
                if (parseDate(startDateStr, &startDate) && parseDate(endDateStr, &endDate)) {
                    ExpenseNode* results = queryByDateRange(head, startDate, endDate);
                    printQueryResultList(results);
                } else {
                    printf("日期格式错误!\n");
                }
                break;
            }
            case 2: { // 按金额查询
                printf("请输入最小金额(输入0返回): ");
                fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
                if (isBackToken(buf)) break;
                float minAmount = atof(buf);
                printf("请输入最大金额(输入0返回): ");
                fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
                if (isBackToken(buf)) break;
                float maxAmount = atof(buf);
                
                ExpenseNode* amountResults = queryByAmount(head, minAmount, maxAmount);
                printQueryResultList(amountResults);
                break;
            }
            case 3: { // 按成员和日期查询
                printf("请输入家庭成员(输入0返回): ");
                scanf("%19s", member); clearInputBuffer();
                if (isBackToken(member)) break;
                printf("请输入开始日期(YYYYMMDD，输入0返回): ");
                scanf("%8s", startDateStr); clearInputBuffer();
                if (isBackToken(startDateStr)) break;
                printf("请输入结束日期(YYYYMMDD，输入0返回): ");
                scanf("%8s", endDateStr); clearInputBuffer();
                if (isBackToken(endDateStr)) break;
                
                if (parseDate(startDateStr, &startDate) && parseDate(endDateStr, &endDate)) {
                    ExpenseNode* memberResults = queryByMemberAndDate(head, member, startDate, endDate);
                    printQueryResultList(memberResults);
                } else {
                    printf("日期格式错误!\n");
                }
                break;
            }
            case 4: { // 按品类和日期查询
                printf("请选择消费品类:\n");
                for (int i = 0; i < CONSUMPTION_TYPE_COUNT; i++) {
                    printf("%d. %s\n", i + 1, getTypeName((ConsumptionType)i));
                }
                printf("选择(1-%d，0返回): ", CONSUMPTION_TYPE_COUNT);
                
                int typeChoice;
                scanf("%d", &typeChoice); clearInputBuffer();
                if (typeChoice == 0) break;
                
                if (typeChoice >= 1 && typeChoice <= CONSUMPTION_TYPE_COUNT) {
                    ConsumptionType type = (ConsumptionType) (typeChoice - 1);
                    
                    printf("请输入开始日期(YYYYMMDD，输入0返回): ");
                    scanf("%8s", startDateStr); clearInputBuffer();
                    if (isBackToken(startDateStr)) break;
                    printf("请输入结束日期(YYYYMMDD，输入0返回): ");
                    scanf("%8s", endDateStr); clearInputBuffer();
                    if (isBackToken(endDateStr)) break;
                    
                    if (parseDate(startDateStr, &startDate) && parseDate(endDateStr, &endDate)) {
                        ExpenseNode* typeResults = queryByTypeAndDate(head, type, startDate, endDate);
                        printQueryResultList(typeResults);
                    } else {
                        printf("日期格式错误!\n");
                    }
                } else {
                    printf("无效的消费品类选择!\n");
                }
                break;
            }
            case 5: { // 多条件组合筛选
                bool useDate = false, useMin = false, useMax = false;
                bool useMember = false, useType = false, useMethod = false;
                bool useSite = false, useDetail = false;
                int startInt = 0, endInt = 0;
                double minA = 0, maxA = 0;
                ConsumptionType type = FOOD;
                char method[20] = {0}, siteKey[50] = {0}, detailKey[100] = {0};

                printf("开始日期(YYYYMMDD，留空/0为不限): ");
                fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
                if (isBackToken(buf)) break;
                if (strlen(buf) > 0 && parseDate(buf, &startDate)) { useDate = true; startInt = dateToInt(startDate); }

                printf("结束日期(YYYYMMDD，留空/0为不限): ");
                fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
                if (isBackToken(buf)) break;
                if (strlen(buf) > 0 && parseDate(buf, &endDate)) { useDate = true; endInt = dateToInt(endDate); }

                printf("最小金额(留空为不限): ");
                fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
                if (isBackToken(buf)) break;
                if (strlen(buf) > 0) { useMin = true; minA = atof(buf); }

                printf("最大金额(留空为不限): ");
                fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
                if (isBackToken(buf)) break;
                if (strlen(buf) > 0) { useMax = true; maxA = atof(buf); }

                printf("成员身份(留空为不限): ");
                fgets(member, sizeof(member), stdin); member[strcspn(member, "\n")] = 0;
                if (strlen(member) > 0) useMember = true;

                printf("消费品类(1-%d，0为不限): ", CONSUMPTION_TYPE_COUNT);
                int t = safeReadInt(-1);
                if (t > 0 && t <= CONSUMPTION_TYPE_COUNT) { useType = true; type = (ConsumptionType)(t - 1); }

                printf("支出方式(留空为不限): ");
                fgets(method, sizeof(method), stdin); method[strcspn(method, "\n")] = 0;
                if (strlen(method) > 0) useMethod = true;

                printf("消费场所关键词(留空为不限): ");
                fgets(siteKey, sizeof(siteKey), stdin); siteKey[strcspn(siteKey, "\n")] = 0;
                if (strlen(siteKey) > 0) useSite = true;

                printf("商品详情关键词(留空为不限): ");
                fgets(detailKey, sizeof(detailKey), stdin); detailKey[strcspn(detailKey, "\n")] = 0;
                if (strlen(detailKey) > 0) useDetail = true;

                ExpenseNode* results = filterByCriteria(head,
                    useDate, startInt, endInt,
                    useMin, minA, useMax, maxA,
                    useMember, member,
                    useType, type,
                    useMethod, method,
                    useSite, siteKey,
                    useDetail, detailKey);

                printQueryResultList(results);
                break;
            }
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
        
        printf("排序方式(1.升序 2.降序，0返回): ");
        int orderChoice; scanf("%d", &orderChoice); clearInputBuffer();
        if (orderChoice == 0) continue;
        
        bool ascending = (orderChoice == 1);
        
        *head = sortList(*head, choice, ascending);
        
        printf("排序完成!\n");
        printListPretty(*head, 1);
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
        
        printf("请输入开始日期(YYYYMMDD，输入0返回): ");
        scanf("%8s", startDateStr); clearInputBuffer();
        if (isBackToken(startDateStr)) continue;
        printf("请输入结束日期(YYYYMMDD，输入0返回): ");
        scanf("%8s", endDateStr); clearInputBuffer();
        if (isBackToken(endDateStr)) continue;
        
        if (!parseDate(startDateStr, &startDate) || !parseDate(endDateStr, &endDate)) {
            printf("日期格式错误!\n");
            pressAnyKeyToContinue();
            continue;
        }
        
        switch (choice) {
            case 1: // 家庭总消费
                printHeader("家庭消费统计");
                printStatistics(head, startDate, endDate);
                printTypeChart(head, startDate, endDate); // 新增分布图
                break;
                
            case 2: { // 特定成员
                char member[20];
                printf("请输入家庭成员: ");
                scanf("%19s", member);
                clearInputBuffer();
                
                float total = calculateMemberTotal(head, member, startDate, endDate);
                printf("\n%s 在 %s 至 %s 期间的消费总额: %.2f\n", 
                       member, startDateStr, endDateStr, total);
                printBar(member, total, total); // 单成员条形图
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
                    printBar(getTypeName(type), total, total); // 单品类条形图
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

// 显示添加菜单（支持0返回上一层）
void showAddMenu(ExpenseNode** head) {
    printf("\n");
    printf("**************************************************\n");
    printf("* 录入新消费记录 *\n");
    printf("**************************************************\n");

    int batch = 1;
    printf("请输入要录入的记录数量(默认1，输入0返回): ");
    batch = safeReadInt(1);
    if (batch == 0) return;
    bool aborted = false;
    for (int i = 0; i < batch; ++i) {
        printf("\n-- 正在录入第 %d/%d 条 --\n", i + 1, batch);

        ExpenseRecord record;
        char dateStr[9];

        printf("请输入消费日期(YYYYMMDD，输入0返回): ");
        scanf("%8s", dateStr); clearInputBuffer();
        if (isBackToken(dateStr)) { aborted = true; break; }
        while (!parseDate(dateStr, &record.date)) {
            printf("日期格式错误，请重新输入(YYYYMMDD，输入0返回): ");
            scanf("%8s", dateStr); clearInputBuffer();
            if (isBackToken(dateStr)) { aborted = true; break; }
        }
        if (aborted) break;
        printf("请输入成员身份(输入0返回): ");
        scanf("%19s", record.member); clearInputBuffer();
        if (isBackToken(record.member)) { aborted = true; break; }
        char buf[64];
        printf("请输入消费金额(输入0返回): ");
        fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
        if (isBackToken(buf)) { aborted = true; break; }
        record.amount = atof(buf);
        printf("请选择消费品类:\n");
        for (int i = 0; i < CONSUMPTION_TYPE_COUNT; i++) {
            printf("%d. %s\n", i + 1, getTypeName((ConsumptionType)i));
        }
        printf("选择(1-%d，0返回): ", CONSUMPTION_TYPE_COUNT);
        int typeChoice;
        scanf("%d", &typeChoice); clearInputBuffer();
        if (typeChoice == 0) { aborted = true; break; }
        while (typeChoice < 1 || typeChoice > CONSUMPTION_TYPE_COUNT) {
            printf("无效的选择，请重新输入(1-%d，0返回): ", CONSUMPTION_TYPE_COUNT);
            scanf("%d", &typeChoice); clearInputBuffer();
            if (typeChoice == 0) { aborted = true; break; }
        }
        if (aborted) break;
        record.type = (ConsumptionType)(typeChoice - 1);

        printf("请输入支出方式: ");
        scanf("%19s", record.method); clearInputBuffer();

        printf("请输入消费场所: ");
        scanf("%49s", record.site); clearInputBuffer();

        printf("请输入商品详情: ");
        scanf("%99s", record.detail); clearInputBuffer();

        *head = addExpense(*head, record);
        printf("第 %d 条消费记录添加成功!\n", i + 1);
    }
    if (!aborted) {
        saveToFile("data/fee.dat", *head);
        printf("已保存 %d 条记录。\n", batch);
    }
}

// 显示更新菜单
void showUpdateMenu(ExpenseNode** head) {
    if (*head == NULL) {
        printf("没有消费记录可供更新。\n");
        pressAnyKeyToContinue();
        return;
    }
    
    printHeader("所有消费记录");
    printListPretty(*head, 1);
    
    int index;
    printf("\n请输入要修改的记录编号(1-%d，0返回): ", getListLength(*head));
    scanf("%d", &index); clearInputBuffer();
    if (index == 0) return;
    
    index--; // 转换为0-based 索引
    
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

    ExpenseRecord newRecord = *record;

    // 新增：选择修改字段的循环
    while (1) {
        printf("\n请选择要修改的字段：\n");
        printf("1. 消费日期\n");
        printf("2. 成员身份\n");
        printf("3. 消费金额\n");
        printf("4. 消费品类\n");
        printf("5. 支出方式\n");
        printf("6. 消费场所\n");
        printf("7. 商品详情\n");
        printf("8. 完成修改并保存\n");
        printf("选择: ");

        int opt = safeReadInt(-1);
        if (opt == 8) break;
        switch (opt) {
            case 1: {
                char dateStr[16];
                printf("请输入新的消费日期(YYYYMMDD, 回车保留原值): ");
                fgets(dateStr, sizeof(dateStr), stdin);
                dateStr[strcspn(dateStr, "\n")] = 0;
                if (strlen(dateStr) > 0 && parseDate(dateStr, &newRecord.date)) {
                    // 更新成功
                } else if (strlen(dateStr) > 0) {
                    printf("日期格式错误，未修改。\n");
                }
                break;
            }
            case 2: {
                char buf[32];
                printf("请输入新的成员身份(回车保留原值): ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) {
                    strncpy(newRecord.member, buf, sizeof(newRecord.member) - 1);
                    newRecord.member[sizeof(newRecord.member) - 1] = '\0';
                }
                break;
            }
            case 3: {
                char buf[32];
                printf("请输入新的消费金额(回车保留原值): ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) {
                    newRecord.amount = atof(buf);
                }
                break;
            }
            case 4: {
                printf("请选择新的消费品类(1-%d, 0保留原值): ", CONSUMPTION_TYPE_COUNT);
                int t = safeReadInt(0);
                if (t > 0 && t <= CONSUMPTION_TYPE_COUNT) {
                    newRecord.type = (ConsumptionType)(t - 1);
                }
                break;
            }
            case 5: {
                char buf[32];
                printf("请输入新的支出方式(回车保留原值): ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) {
                    strncpy(newRecord.method, buf, sizeof(newRecord.method) - 1);
                    newRecord.method[sizeof(newRecord.method) - 1] = '\0';
                }
                break;
            }
            case 6: {
                char buf[64];
                printf("请输入新的消费场所(回车保留原值): ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) {
                    strncpy(newRecord.site, buf, sizeof(newRecord.site) - 1);
                    newRecord.site[sizeof(newRecord.site) - 1] = '\0';
                }
                break;
            }
            case 7: {
                char buf[128];
                printf("请输入新的商品详情(回车保留原值): ");
                fgets(buf, sizeof(buf), stdin);
                buf[strcspn(buf, "\n")] = 0;
                if (strlen(buf) > 0) {
                    strncpy(newRecord.detail, buf, sizeof(newRecord.detail) - 1);
                    newRecord.detail[sizeof(newRecord.detail) - 1] = '\0';
                }
                break;
            }
            default:
                printf("无效的选择。\n");
        }
    }

    *head = updateExpense(*head, index, newRecord);
    printf("记录更新成功!\n");

    // 保存到文件
    saveToFile("data/fee.dat", *head);
    pressAnyKeyToContinue();
}

/* 删除记录的界面与逻辑 */
void showDeleteMenu(ExpenseNode** head){
    if (head == NULL || *head == NULL) {
        printf("\n没有可删除的记录。\n");
        pressAnyKeyToContinue();
        return;
    }

    printHeader("删除记录");
    printListPretty(*head, 1);

    int len = getListLength(*head);
    printf("请输入要删除的记录编号(1-%d，0取消): ", len);
    int idx = safeReadInt(-1);
    if (idx == 0) return;          // 取消
    if (idx < 1 || idx > len) {
        printf("无效的编号。\n");
        pressAnyKeyToContinue();
        return;
    }

    char confirm;
    printf("确认删除记录 %d ? (y/n): ", idx);
    scanf(" %c", &confirm);
    clearInputBuffer();
    if (confirm != 'y' && confirm != 'Y') return;

    *head = deleteExpense(*head, idx - 1); // 转为0基索引
    if (saveToFile("data/fee.dat", *head)) {
        printf("删除成功并已保存。\n");
    } else {
        printf("删除成功但保存失败。\n");
    }

    pressAnyKeyToContinue();
}
