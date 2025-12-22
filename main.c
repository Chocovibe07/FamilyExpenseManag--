#include <windows.h>
#include "data_structures.h"
#include "list_operations.h"
#include "file_operations.h"
#include "ui_functions.h"
#include <stdlib.h>
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // 初始化
    ExpenseNode* head = createList();
    const char* dataFile = "data/fee.dat";

    // 创建数据目录（如果不存在）
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif

    // 加载数据
    if (!loadFromFile(dataFile, &head)) {
        // 如果文件不存在，创建示例数据
        createSampleData(dataFile);
        loadFromFile(dataFile, &head);
    }

    // 显示主菜单
    showMainMenu(&head);

    return 0;
}