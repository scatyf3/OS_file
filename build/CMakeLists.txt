cmake_minimum_required(VERSION 3.12)
project(filsys)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# 添加可执行文件
add_executable(filsys
    ../src/main.cpp
    ../src/igetput.cpp
    ../src/iallfre.cpp
    ../src/ballfre.cpp
    ../src/name.cpp
    ../src/access.cpp
    ../src/log.cpp
    ../src/close.cpp
    ../src/creat.cpp
    ../src/delete.cpp
    ../src/dir.cpp
    ../src/open.cpp
    ../src/rdwt.cpp
    ../src/format.cpp
    ../src/install.cpp
    ../src/halt.cpp
    ../src/shell.cpp
)

# 添加头文件搜索路径（如果有需要的话）
# include_directories(include)

# 设置编译选项（如果有需要的话）
# target_compile_options(filsys PRIVATE -Wall -Wextra)

# 链接其他库（如果有需要的话）
# target_link_libraries(filsys my_library)