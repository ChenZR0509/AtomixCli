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

CliConfig* initCli(const char* name, const char* filePath, const PrintCmdFunction printCmd,
    const PrintFileFunction printFile)
{
    if (name == NULL || filePath == NULL || printCmd == NULL || printFile == NULL) return NULL;

    CliConfig* cli = NULL;
    cli = calloc(1, sizeof(CliConfig));
    if (cli == NULL) return NULL;

    cli->name = strdup(name);
    if (cli->name == NULL)
    {
        unInitCli(cli);
        return NULL;
    }
    //日志输出
    cli->printCmd = printCmd;
    cli->printFile = printFile;
    cli->logConfig = initLog(filePath, 1024, CmdMode, TLStyle);
    if (cli->logConfig == NULL)
    {
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

    cli->printCmd = NULL;
    cli->printFile = NULL;

    free(cli);
}