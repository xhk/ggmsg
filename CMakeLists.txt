# 指定最低版本
cmake_minimum_required(VERSION 3.25)


# 指定项目名称
project(ggmsg)

# ==========================================
# 设置全局输出目录
# ==========================================

# 1. 设置可执行文件输出目录 (Windows 下是 .exe 和 .dll)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 2. 设置动态库输出目录 (Linux 下是 .so)
#    注意：在 Windows 上 DLL 属于 RUNTIME，但在 Linux 上 .so 属于 LIBRARY
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 3. 设置静态库输出目录 (.lib, .a)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(ggmsg)
add_subdirectory(ggmsgTest)
add_subdirectory(ServerClientTest)
add_subdirectory(ClientTest)
add_subdirectory(ServerTest)


#add_subdirectory(jggmsg)
