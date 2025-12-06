# 指定最低版本
cmake_minimum_required(VERSION 2.8)

# 指定项目名称
project(jggmsg)

# 为当前路径以及子目录的源文件加入由-D预编译定义
# add_definitions(-DFOO -DDEBUG ...)

# 设置C++编译参数(CMAKE_CXX_FLAGS是全局变量)
# set(CMAKE_CXX_FLAGS "-Wall std=c++11 -Wextra -fPIC -g")

# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定为C++11 版本
set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    # warning level 4 and all warnings as errors
    #add_compile_options(/W4 /WX)
    #set(boost_home $ENV{boost_1_79_0_home})
	set(java_platform_inc $ENV{JAVA_HOME}/include/win32)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    #set(boost_home "")
	set(java_platform_inc $ENV{JAVA_HOME}/include/linux)
endif()

# 设置变量
#set(ROOT_DIR "./")

# 头文件查找目录
include_directories(
	$ENV{JAVA_HOME}/include
	${java_platform_inc}
#    ${ROOT_DIR}/include
#    ${ROOT_DIR}/include/third_party
#    ${boost_home}
)

# 库文件查找目录
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../build/ggmsg/Debug)

# sub_dir指定包含CMakeLists.txt和源码文件的子目录位置
# binary_dir是输出路径， 一般可以不指定
#add_subdirectory(sub_dir [binary_dir])

# 把src目录下所有源文件写入变量DIR_SRCS
aux_source_directory(./ DIR_SRCS)

# 以DIR_SRCS为源文件生成目标文件a.out
add_library(jggmsg SHARED
${DIR_SRCS}
)

# 创建库文件
# add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)

# 目标文件链接的库
target_link_libraries(jggmsg ggmsg)