/**
 * @file cliPipeline.h
 * @brief 管道
 * @author ChenZR
 */
#ifndef CLIPIPELINE_H
#define CLIPIPELINE_H
/* Includes" "------------------------------------------------------------------*/
/* Includes< >------------------------------------------------------------------*/
#include <stdint.h>
/* Forward Declaration------------------------------------------------------------------*/
typedef struct CliConfig CliConfig;
/* DataType Definition------------------------------------------------------------------*/
typedef struct WordList {
    char** words;           ///< words 单词数组
    uint16_t count;         ///< count 单词数量
    uint16_t capacity;      ///< capacity 链表容量
}WordList;                  ///< WordList 单词列表
typedef struct PipelineStage
{
    WordList* words;        ///< words 单词
}PipelineStage;             ///< PipelineStage 管道阶段
typedef struct PipelineStageList
{
    PipelineStage** stages;         ///< stages 管道数组
    PipelineStage* currentStage;    ///< currentStage 当前正在处理的管道阶段
    uint16_t count;                 ///< count 管道数量
    uint16_t capacity;              ///< capacity 管道容量
}PipelineStageList;
typedef struct Pipeline
{
    char* commandline;              ///< commandline 保存原始命令行字符串
    WordList* wordList;             ///< wordList 按空格等规则拆分后的单词数组
    PipelineStageList* stageList;   ///< stageList 管道阶段列表
}Pipeline;                          ///< Pipeline 管道
/* Variable Declare------------------------------------------------------------------*/

/* Functions Declare------------------------------------------------------------------*/
/**
 * @name initPipeline
 * @brief 管道初始化
 *
 * @param[in] cli CLI结构体
 * @param[in] commandLine 命令行字符串
 * @return Pipeline* 初始化成功返回Pipeline指针，失败返回NULL
 */
Pipeline* initPipeline(const CliConfig* cli, const char* commandLine);
/**
 * @name unInitPipeline
 * @brief 释放管道资源
 *
 * @param[in] pipeline 管道资源
 * @return void
 */
void unInitPipeline(Pipeline* pipeline);
#endif
