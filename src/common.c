#include "common.h"
#include <stdio.h>
// 清除输入缓冲区
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 按任意键继续
void pressAnyKeyToContinue() {
    printf("\n按回车键继续...");
    clearInputBuffer();
    getchar();
}

// 打印标题
void printHeader(const char* title) {
    printf("\n");
    printf("**************************************************\n");
    printf("* %s *\n", title);
    printf("**************************************************\n");
}

// 打印菜单
void printMenu(const char* title, const char* options[], int numOptions) {
    printHeader(title);
    for (int i = 0; i < numOptions; i++) {
        printf("%d. %s\n", i + 1, options[i]);
    }
    printf("\n请选择(1-%d): ", numOptions);
}
