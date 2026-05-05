#[[
 @ ProjectName: AtomixCli
 @ Author: ChenZR
 @ Brief: 命令行解释器库
 @ Date: 20240415
 ]]
#------------------------------------------------------------------------------#
#工程配置
cmake_minimum_required(VERSION 4.1)
project(AtomixCli C)
#编译器配置
set(CMAKE_C_STANDARD 11)
#项目组织
add_executable(AtomixCli main.c
        include/cliConfig.h
        include/cliLog.h
        include/cliPipeline.h
        src/cliConfig.c
        src/cliLog.c
        src/cliPipeline.c)
target_include_directories(
        AtomixCli
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

