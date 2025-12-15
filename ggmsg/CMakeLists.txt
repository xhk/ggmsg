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

# 安装配置
include(GNUInstallDirs)

# 安装头文件
install(
    DIRECTORY include/ 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ggmsg
    FILES_MATCHING PATTERN "*.h"
)

# 安装库文件
install(
    TARGETS ggmsg
    EXPORT ggmsgTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 导出CMake配置文件
install(
    EXPORT ggmsgTargets
    FILE ggmsgTargets.cmake
    NAMESPACE ggmsg::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ggmsg
)

# 生成并安装包配置文件
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ggmsgConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ggmsg
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ggmsgConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ggmsgConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ggmsgConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ggmsg
)
