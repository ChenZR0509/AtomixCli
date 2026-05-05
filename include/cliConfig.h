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
/* DataType Definition------------------------------------------------------------------*/
typedef void (*PrintCmdFunction)(const char* logContent);
typedef void (*PrintFileFunction)(const char* logContent, const char* file);
typedef CommandNode* (*RegisterCommandTree)(CliConfig* cli);
typedef struct CliConfig                    ///< CliConfig CLI 配置结构体
{
    char* name;                     ///< name CLI 配置的名字
    LogConfig* logConfig;           ///< logConfig 日志配置
    CommandNode* commandTree;       ///< commandTree 命令树

    RegisterCommandTree registerCommandTree; ///< registerCommandTree 命令树注册函数指针
    PrintCmdFunction printCmd;      ///< cmdPrint 命令行打印函数指针
    PrintFileFunction printFile;    ///< filePrint 文件打印函数指针
}CliConfig;
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
#ifdef __cplusplus
}
#endif
#endif
