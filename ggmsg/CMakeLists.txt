# 指定最低版本
cmake_minimum_required(VERSION 3.15)

# 指定项目名称
project(ggmsg)

# 为当前路径以及子目录的源文件加入由-D预编译定义
# add_definitions(-DFOO -DDEBUG ...)

# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定为C++17 版本
set(CMAKE_CXX_STANDARD 17)

# 查找Boost库
find_package(Boost REQUIRED)

message(STATUS "Found Boost: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

# Boost.Asio是头文件库，不需要单独的组件
message(STATUS "Boost.Asio will be used from headers")

# 头文件查找目录
include_directories(
    ${Boost_INCLUDE_DIRS}
    include
)

# 库文件查找目录
# link_directories(${ROOT_DIR}/lib)

# sub_dir指定包含CMakeLists.txt和源码文件的子目录位置
# binary_dir是输出路径， 一般可以不指定
#add_subdirectory(sub_dir [binary_dir])

# 把src目录下所有源文件写入变量DIR_SRCS
aux_source_directory(src DIR_SRCS)

# 以DIR_SRCS为源文件生成目标文件a.out
add_library(ggmsg SHARED
    ${DIR_SRCS}
)

# 创建库文件
# add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)

# 目标文件链接的库
#target_link_libraries(ggmsg )
