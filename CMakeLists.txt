cmake_minimum_required(VERSION 3.17)
project(FamilyExpenseSystem C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)

# 添加头文件目录
include_directories(include)

# 添加所有源文件（关键步骤！）
file(GLOB SOURCE_FILES
        "src/*.c"        # 确保包含src目录下所有.c文件
        "main.c"         # 主程序文件
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})