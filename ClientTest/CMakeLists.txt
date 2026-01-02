# 指定最低版本
cmake_minimum_required(VERSION 3.15)

# 指定项目名称
project(ClientTest)

# 为当前路径以及子目录的源文件加入由-D预编译定义
# add_definitions(-DFOO -DDEBUG ...)

set(CMAKE_CXX_STANDARD 17)

# 头文件查找目录
include_directories(
    ${ROOT_DIR}/include
    ${ROOT_DIR}/include/third_party
)

# 库文件查找目录
link_directories("")

# 把src目录下所有源文件写入变量DIR_SRCS
aux_source_directory(./ DIR_SRCS)

# 以DIR_SRCS为源文件生成目标文件a.out
add_executable(${PROJECT_NAME} 
    ${DIR_SRCS}
)

# 目标文件链接的库
target_link_libraries(${PROJECT_NAME} ggmsg )