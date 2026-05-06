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

bool setLed(const CliConfig* cli, const char *commandName, char** argv, uint16_t argc)
{
    if (cli == NULL || commandName == NULL) return false;

    if (argc != 1)  return false;
    const char* ledState = argv[0];
    if (strcmp(ledState, "on") == 0)
    {
        printLog(cli, LogInfo, "LED is turned ON. [Fun: %s]\r\n", __FUNCTION__);
    }
    else if (strcmp(ledState, "off") == 0)
    {
        printLog(cli, LogInfo, "LED is turned OFF. [Fun: %s]\r\n", __FUNCTION__);
    }
    else
    {
        return false;
    }
    return true;
}

CommandNode* registerCommandTree(CliConfig* cli)
{
    if (cli == NULL) return NULL;

    CommandNode* setCommand = NULL;
    setCommand = registerCommand(cli, "set", NULL, NULL);
    registerCommand(cli, "led", setCommand, setLed);
    return setCommand;
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
            runCli(cli, input);
        }
    }
    unInitCli(cli);
    return 0;
}