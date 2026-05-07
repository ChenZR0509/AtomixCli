/**
 * @file cliConfig.h
 * @brief 解释器配置
 * @author ChenZR
 */
#ifndef CLICONFIG_H
#define CLICONFIG_H
#ifdef __cplusplus
extern "C" {
#endif
/* Includes" "------------------------------------------------------------------*/
#include "cliLog.h"
#include "cliCommand.h"
/* Includes< >------------------------------------------------------------------*/

/* Forward Declaration------------------------------------------------------------------*/
/**
 * @name PrintCmdFunction
 * @brief 日志输出函数指针
 * @details 将相关字符打印在屏幕或串口设备上
 *
 * @param[in] logContent 日志内容
 * @param[in] contentLength 包含终止符的字符串长度
 * @return none
 *
 * @note 若不需要日志输出功能，则定义一个空函数并将其地址传入CLI配置中，并配置日志模式为FileMode
 */
typedef void (*PrintCmdFunction)(const char* logContent, const uint16_t contentLength);
/**
 * @name PrintFileFunction
 * @brief 日志文件输出函数指针
 * @details 将相关字符写入至对应的文件中
 *
 * @param[in] logContent 日志内容
 * @param[in] file 文件名称
 * @param[in] contentLength 包含终止符的字符串长度
 * @return none
 *
 * @note 若不需要日志文件输出功能，则定义一个空函数并将其地址传入CLI配置中，并配置日志模式为CmdMode
 */
typedef void (*PrintFileFunction)(const char* logContent, const char* file, const uint16_t contentLength);
/**
 * @name *RegisterCommandTree
 * @brief 配置命令树函数指针
 * @details 调用命令注册函数实现命令树的构建
 *
 * @param[in] cli CLI结构体指针
 * @return CommandNode* 命令树根节点指针
 */
typedef CommandNode* (*RegisterCommandTree)(CliConfig* cli);
/* DataType Definition------------------------------------------------------------------*/
typedef struct CliConfig
{
    char* name;                     ///< name CLI 配置的名字
    LogConfig* logConfig;           ///< logConfig 日志
    CommandNode* commandTree;       ///< commandTree 命令树

    RegisterCommandTree registerCommandTree; ///< registerCommandTree 命令树注册函数指针
    PrintCmdFunction printCmd;      ///< cmdPrint 命令行打印函数指针
    PrintFileFunction printFile;    ///< filePrint 文件打印函数指针
}CliConfig;                         ///< CliConfig CLI 配置结构体
//* Variable Declare------------------------------------------------------------------*/

/* Functions Declare------------------------------------------------------------------*/
/**
 * @name initCli
 * @brief 初始化CLI
 *
 * @param[in] name cli名称
 * @param[in] filePath 日志文件路径
 * @param[in] registerCommandTree 命令树注册函数
 * @param[in] printCmd 终端打印函数
 * @param[in] printFile 文件打印函数
 * @return CliConfig* CLI配置结构体指针
 */
CliConfig* initCli(const char* name, const char* filePath,
    const RegisterCommandTree registerCommandTree,
    const PrintCmdFunction printCmd,
    const PrintFileFunction printFile);
/**
 * @name unInitCli
 * @brief 释放CLI
 *
 * @param[in] cli CLI结构体指针
 */
void unInitCli(CliConfig* cli);
/**
 * @name runCli
 * @brief 运行CLI
 *
 * @param[in] cli CLI结构体指针
 * @param[in] input 字符串输入
 * @return none
 */
void runCli(const CliConfig* cli, const char* input);
#ifdef __cplusplus
}
#endif
#endif
