/**
* @file cliCommand.h
 * @brief 命令
 * @author ChenZR
 */
#ifndef CLICOMMAND_H
#define CLICOMMAND_H
#ifdef __cplusplus
extern "C" {
#endif
/* Includes" "------------------------------------------------------------------*/
/* Includes< >------------------------------------------------------------------*/
/* Forward Declaration------------------------------------------------------------------*/
typedef struct CliConfig CliConfig;
typedef struct CommandNode CommandNode;
/* DataType Definition------------------------------------------------------------------*/
typedef struct CommandNode
{
    char* name;             ///< name 当前节点的名字
    char* fullName;         ///< fullName 完整命令路径
    CommandNode *previous;      ///< previous 命令树同级前一个节点
    CommandNode *next;          ///< next 命令树同级下一个节点
    CommandNode *children;      ///< children 命令树下一级第一个子节点
    CommandNode *parent;        ///< parent 命令树父节点
}CommandNode;
/* Variable Declare------------------------------------------------------------------*/
/* Functions Declare------------------------------------------------------------------*/
/**
 * @name registerCommand
 * @brief 命令注册函数
 *
 * @param[in] cli 命令行结构体指针
 * @param[in] name 命令名称
 * @param[in] parent 父命令节点
 * @return Command* 注册的命令节点结构体，
 *
 * @note 如果是第一次注册那么应将其返回值赋值给cli->commandTree
 * @note 如果不是第一次第一次注册且parent不为NULL，则节点挂载在对应的父节点下
 * @note 如果不是第一次第一次注册且parent为NULL，则节点挂载在根命令列表的末尾
 */
CommandNode* registerCommand(CliConfig* cli, const char *name ,CommandNode* parent);
/**
 * @name unRegisterCommand
 * @brief 命令注销函数
 *
 * @param[in] cli 命令行结构体指针
 * @param[in] command 命令节点
 * @return none
 */
void unRegisterCommand(CliConfig* cli,CommandNode* command);
/**
 * @name unRegisterCommandTree
 * @brief 命令树注销函数
 *
 * @param[in] cli 命令行结构体指针
 * @return none
 */
void unRegisterCommandTree(CliConfig *cli);
#ifdef __cplusplus
}
#endif
#endif
