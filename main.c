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
            Pipeline* pipe = initPipeline(cli, input);
            for (uint16_t i = 0; i < pipe->stageList->count; i++)
            {
                for (uint16_t j = 0; j < pipe->stageList->stages[i]->words->count; j++)
                {
                    printf("%s\r\n", pipe->stageList->stages[i]->words->words[j]);
                }
                printf("\n");
            }
            unInitPipeline(pipe);
        }
    }
    unInitCli(cli);
    return 0;
}