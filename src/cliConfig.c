/**
* @file cliConfig.c
 * @brief cliConfig 命令行解释器配置
 * @author ChenZR
 */
/* Includes" "------------------------------------------------------------------*/
#include "cliConfig.h"
/* Includes< >------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
/* Macro Definition------------------------------------------------------------------*/
/* DataType Definition------------------------------------------------------------------*/
/* Variable Definition-----------------------------------------------------*/
/* Functions Declare------------------------------------------------------------------*/
/* Function Definition------------------------------------------------------------------*/
CliConfig* initCli(const char* name, const char* filePath,
    const RegisterCommandTree registerCommandTree,
    const PrintCmdFunction printCmd,
    const PrintFileFunction printFile)
{
    if (name == NULL || filePath == NULL
        || registerCommandTree == NULL
        || printCmd == NULL || printFile == NULL) return NULL;

    CliConfig* cli = NULL;
    cli = calloc(1, sizeof(CliConfig));
    if (cli == NULL) return NULL;

    //函数注册
    cli->printCmd = printCmd;
    cli->printFile = printFile;
    cli->registerCommandTree = registerCommandTree;
    //基础信息
    cli->name = strdup(name);
    if (cli->name == NULL)
    {
        unInitCli(cli);
        return NULL;
    }
    //日志输出
    cli->logConfig = initLog(filePath, 1024, CmdMode, TLStyle);
    if (cli->logConfig == NULL)
    {
        unInitCli(cli);
        return NULL;
    }
    //命令树
    cli->commandTree = cli->registerCommandTree(cli);
    if (cli->commandTree == NULL)
    {
        printLog(cli,LogError, "Failed to init CLI. [Cli: %s]",__FUNCTION__,cli->name);
        unInitCli(cli);
        return NULL;
    }
    return cli;
}

void unInitCli(CliConfig* cli)
{
    if (cli == NULL) return;

    if (cli->name)
    {
        free(cli->name);
        cli->name = NULL;
    }
    if (cli->logConfig)
    {
        uninitLog(cli->logConfig);
        cli->logConfig = NULL;
    }
    if (cli->commandTree)
    {
        unRegisterCommandTree(cli);
        cli->commandTree = NULL;
    }

    cli->printCmd = NULL;
    cli->printFile = NULL;

    free(cli);
}