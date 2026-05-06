/**
 * @file cliCommand.c
 * @brief 命令
 * @author ChenZR
 */
/* Includes" "------------------------------------------------------------------*/
#include "cliLog.h"
#include "cliCommand.h"
#include "cliConfig.h"
#include "cliPipeline.h"
/* Includes< >------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
/* Macro Definition------------------------------------------------------------------*/
/* DataType Definition------------------------------------------------------------------*/
/* Variable Definition-----------------------------------------------------*/
/* Functions Declare------------------------------------------------------------------*/
/**
 * @name setCmdFullName
 * @brief 设置命令节点路径
 *
 * @param[in] command 命令节点
 * @return bool 是否设置成功
 */
bool setCmdFullName(CommandNode* command);
/**
 * @name freeCommand
 * @brief 释放命令资源
 *
 * @param[in] command 命令节点
 * @return void
 */
void freeCommand(CommandNode* command);
/* Function Definition------------------------------------------------------------------*/
CommandNode* registerCommand(CliConfig* cli, const char *name ,CommandNode* parent, CommandFunction function)
{
    /// Note parent可以为空，其表示直接挂载在命令树上是一级节点
    if (cli == NULL || name == NULL) return NULL;

    CommandNode* command = NULL;
    command = calloc(sizeof(CommandNode), 1);
    if (command == NULL)
    {
        return NULL;
    }

    command->parent = parent;
    command->name = strdup(name);
    if (command->name == NULL)
    {
        freeCommand(command);
        return NULL;
    }
    if (setCmdFullName(command) == false)
    {
        freeCommand(command);
        return NULL;
    }
    /*
     * Note 命令树注册
     * Note 1、如果存在父节点且父节点的子节点为空，则将当前节点挂载在父节点的子节点上
     * Note 2、如果存在父节点且父节点的子节点不为空，则将子节点存储在tempCommand，待后续将其挂载至next上
     * Note 3、如果不存在父节点且CLI结构体命令树节点为空，则令当前节点为命令树节点
     * Note 4、如果不存在父节点且CLI结构体命令树节点为不空，则将命令树节点存储在tempCommand，待后续将其挂载至next上
     */
    CommandNode* tempCommand = NULL;
    if (parent != NULL)
    {
        if (parent->children == NULL)
        {
            parent->children = command;
        }
        else
        {
            tempCommand = parent->children;
        }
    }
    else
    {
        if (cli->commandTree == NULL)
        {
            cli->commandTree = command;
            return command;
        }
        else
        {
            tempCommand = cli->commandTree;
        }
    }
    /*
     * Note 同一层级命令挂载，循环查找最后的子节点，并将相关节点挂载在其之后
     */
    while (tempCommand && tempCommand->next)
    {
        tempCommand = tempCommand->next;
    }
    if (tempCommand != NULL)
    {
        tempCommand->next = command;
        command->previous = tempCommand;
    }

    command->function = function;
    return command;
}

void freeCommand(CommandNode* command)
{
    if (command->name)
    {
        free(command->name);
        command->name = NULL;
    }
    if (command->fullName)
    {
        free(command->fullName);
        command->fullName = NULL;
    }

    command->parent = NULL;
    command->previous = NULL;
    command->next = NULL;
    command->children = NULL;
    command->function = NULL;
    free(command);
}

void unRegisterCommand(CliConfig* cli, CommandNode* command)
{
    if (cli == NULL || command == NULL) return;

    /// Note 递归注销子命令，一路访问子节点，直到最底层children
    CommandNode* nextCommand = command->children;
    while (nextCommand)
    {
        CommandNode* tempCommand = nextCommand->next;
        unRegisterCommand(cli,nextCommand);
        nextCommand = tempCommand;
    }

    /// Note 如果删除节点同一层级前面有命令节点，则将其删除节点的前后两个节点链接起来
    if (command->previous)
    {
        command->previous->next = command->next;
    }
    if (command->next)
    {
        command->next->previous = command->previous;
    }
    /// Note 若要删除的节点为命令树根节点，则将删除节点的下一个节点赋值给CLI命令树
    if (command == cli->commandTree)
    {
        cli->commandTree = command->next;
        if (command->next)
        {
            command->next->parent = NULL;
            command->next->previous = NULL;
        }
    }
    /// Note 如果删除节点刚好存储在他父节点的children属性上，那么需要更改父节点的children属性
    if (command->parent && command->parent->children == command)
    {
        command->parent->children = command->next;
        if (command->next)
        {
            command->next->parent = command->parent;
            command->next->previous = NULL;
        }
    }

    freeCommand(command);
}

void unRegisterCommandTree(CliConfig *cli)
{
    if (cli == NULL) return;

    CommandNode* command = cli->commandTree;
    while (command)
    {
        CommandNode* next = command->next;
        unRegisterCommand(cli, command);
        command = next;
    }
    cli->commandTree = NULL;
}

bool setCmdFullName(CommandNode* command)
{
    if (command == NULL) return false;

    /// Note 如果命令路径本身有值，则释放内存后在进行复制
    if (command->fullName)
    {
        free(command->fullName);
        command->fullName = NULL;
    }

    /// Note 计算命令完整路径字符长度
    uint16_t count = 0;
    size_t fullNameLength = 0;
    const CommandNode* node = command;
    while (node)
    {
        if (node->name == NULL) return false;
        count++;    ///< 计算一共有几个单词，用于后续计算空格数量
        fullNameLength += strlen(node->name);
        node = node->parent;
    }
    if (count > 1)
    {
        fullNameLength += count - 1;
    }

    /// Note 进行字符拼接
    char* fullName = calloc((size_t)fullNameLength+1, sizeof(char));
    if (fullName == NULL) return false;
    node = command;
    while (node)
    {
        const size_t oldLen = strlen(fullName);
        const size_t nameLen = strlen(node->name);
        memmove(fullName + nameLen + (oldLen > 0 ? 1 : 0),
                fullName,
                oldLen + 1);
        memcpy(fullName, node->name, nameLen);
        if (oldLen > 0) fullName[nameLen] = ' ';
        node = node->parent;
    }
    command->fullName = fullName;
    return true;
}

CommandNode* findCommand(const CliConfig* cli, const CommandNode* parentNode, const char* name)
{
    if (cli == NULL || name == NULL) return NULL;

    const size_t inputLength = strlen(name);
    CommandNode* tempCommand = NULL;

    if (parentNode == NULL)
    {
        tempCommand = cli->commandTree;
    }
    else
    {
        tempCommand = parentNode->children;
    }
    for (; tempCommand != NULL; tempCommand = tempCommand->next)
    {
        if (tempCommand->name == NULL) continue;
        if (strlen(tempCommand->name) != inputLength) continue;
        if (strcmp(tempCommand->name, name) == 0)
        {
            return tempCommand;
        }
    }
    return NULL;
}