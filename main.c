/**
* @file main.c
 * @brief 入口文件
 * @author ChenZR
 */
/* Includes" "------------------------------------------------------------------*/
#include "cliConfig.h"
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

int main()
{
    CliConfig* cli = initCli("CLI1", "cli.log", printCmdFunction, printFileFunction);
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
            printLog(cli, LogInfo, input);
        }
    }
    unInitCli(cli);
    return 0;
}