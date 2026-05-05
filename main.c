/**
* @file main.c
 * @brief 入口文件
 * @author ChenZR
 */
/* Includes" "------------------------------------------------------------------*/
#include "cliConfig.h"

#include "cliPipeline.h"
/* Includes< >------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
/* Main------------------------------------------------------------------*/
void printCmdFunction(const char* logContent)
{
    printf("%s", logContent);
}

void printFileFunction(const char* logContent, const char* file)
{
    printf("%s [%s]", logContent,file);
}

CommandNode* registerCommandTree(CliConfig* cli)
{
    if (cli == NULL) return NULL;

    CommandNode* commandTree = NULL;
    commandTree = registerCommand(cli, "set", NULL);
    registerCommand(cli, "reset", NULL);
    registerCommand(cli, "get", NULL);
    registerCommand(cli, "load", NULL);
    registerCommand(cli, "unload", NULL);
    registerCommand(cli, "start", NULL);
    registerCommand(cli, "pause", NULL);
    registerCommand(cli, "resume", NULL);
    registerCommand(cli, "finish", NULL);
    return commandTree;
}

int main()
{
    CliConfig* cli = initCli("CLI1", "cli.log", registerCommandTree,
        printCmdFunction, printFileFunction);
    bool isQuit = false;
    while (isQuit == false)
    {
        char input[256];
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;
        if (strcmp(input, "quit\n") == 0)
        {
            isQuit = true;
        }
        else
        {
            Pipeline* pipe = initPipeline(cli, input);
            unInitPipeline(pipe);
        }
    }
    unInitCli(cli);
    return 0;
}