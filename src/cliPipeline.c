/**
* @file cliPipeline.c
 * @brief 管道
 * @author ChenZR
 */
/* Includes" "------------------------------------------------------------------*/
#include "cliLog.h"
#include "cliPipeline.h"
#include "cliConfig.h"
/* Includes< >------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
/* Macro Definition------------------------------------------------------------------*/
/* DataType Definition------------------------------------------------------------------*/
/* Variable Definition-----------------------------------------------------*/
/* Functions Declare------------------------------------------------------------------*/
/**
 * @name initWordList
 * @brief 初始化单词列表
 *
 * @param[in] void
 * @return WordList* 初始化成功返回WordList指针，失败返回NULL
 */
WordList* initWordList(void);
/**
 * @name unInitWordList
 * @brief 释放单词列表
 *
 * @param[in] list 单词列表
 * @return 返回值说明。
 */
void unInitWordList(WordList* list);
/**
 * @name addWord
 * @brief 添加单词
 *
 * @param[in] list 单词列表
 * @param[in] word 单词指针
 * @return bool 如果添加成功返回true，失败返回false
 */
bool addWord(WordList* list, char* word);
/**
 * @name initStageList
 * @brief 初始化管道列表
 *
 * @param[in] void
 * @return StageList* 初始化成功返回WordList指针，失败返回NULL
 */
PipelineStageList* initStageList(void);
/**
 * @name unInitStageList
 * @brief 释放管道列表
 *
 * @param[in] list 管道列表
 * @return 返回值说明。
 */
void unInitStageList(PipelineStageList* list);
/**
 * @name addStage
 * @brief 添加管道阶段
 *
 * @param[in] stageList 管道列表
 * @param[in] pipelineStage 管道阶段
 * @return bool 如果添加成功返回true，失败返回false
 */
bool addStage(PipelineStageList* stageList, PipelineStage* pipelineStage);
/**
 * @name initPipelineStage
 * @brief 初始化管道阶段
 *
 * @return PipelineStage* 初始化成功返回PipelineStage指针，失败返回NULL
 */
PipelineStage* initPipelineStage(void);
/**
 * @name unInitPipelineStage
 * @brief 释放管道阶段资源
 *
 * @param[in] stage PipelineStage 管道阶段
 */
void unInitPipelineStage(PipelineStage* stage);
/**
 * @name unescapeWord
 * @brief 解析单词中的转义字符
 * @details 检索字符串里面的反斜杠‘\’，并将其删去
 *
 * @param[in] start 字符串起始指针
 * @param[int] end 字符串结束指针
 * @return char* 转义字符处理后的字符串指针
 *
 * @note 由于代码里面是先拷贝在移动指针，在进行判断，所以不会处理最后一个字符是反斜杠的情况(通常情况下是\n)
 * @warning
 */
char* unescapeWord(const char* start, const char* end);
/**
 * @name tokenizeCommandLine
 * @brief 分割命令行字符串，并将单词存入wordList
 *
 * @param[in] cli CLI结构体指针
 * @param[in] commandLine 命令行字符串
 * @param[out] wordList 解析结果
 * @return bool 如果解析成功则返回true，失败返回false
 */
bool tokenizeCommandLine(const CliConfig* cli, const char* commandLine, WordList* wordList);
/**
 * @name tokenizePipeline
 * @brief 分割管道命令行字符串，并将单词存入stage->wordList
 *
 * @param[in] cli CLI结构体指针
 * @param[in] wordList 命令行单词列表
 * @param[out] stages 管道阶段
 * @return bool 如果分词成功则返回true，失败返回false
 */
bool tokenizePipeline(const CliConfig* cli, const WordList* wordList, PipelineStageList* stages);
/**
 * @name parsePipeline
 * @brief 解析命令行，将管道阶段对应到具体的命令节点上，以方便后续的命令执行
 * @details
 *
 * @param[in] cli CLI结构体指针
 * @param[in] pipeline 管道结构体指针
 * @return bool 如果解析成功则返回true，失败返回false
 */
bool parsePipeline(const CliConfig* cli,const Pipeline* pipeline);
/* Function Definition------------------------------------------------------------------*/
Pipeline* initPipeline(const CliConfig* cli, const char* commandLine)
{
    if (cli == NULL || commandLine == NULL) return NULL;

    while (isspace(*commandLine)) commandLine++;

    Pipeline *pipeline = calloc(1, sizeof(Pipeline));
    if (pipeline == NULL) return NULL;

    pipeline->commandline = strdup(commandLine);
    if (pipeline->commandline == NULL)
    {
        unInitPipeline(pipeline);
        return NULL;
    }
    // 命令行分词
    pipeline->wordList = initWordList();
    if (pipeline->wordList == NULL)
    {
        unInitPipeline(pipeline);       /// Note 分配失败则释放资源
        return NULL;
    }
    if (tokenizeCommandLine(cli, commandLine, pipeline->wordList) == false)
    {
        printLog(cli, LogError, "Failed to tokenize command line. [Cli: %s]\n", cli->name);
        unInitPipeline(pipeline);       /// Note 解析失败则释放资源
        return NULL;
    }
    // 命令行分管道
    pipeline->stageList = initStageList();
    if (pipeline->stageList == NULL)
    {
        unInitPipeline(pipeline);       /// Note 分配失败则释放资源
        return NULL;
    }
    if (tokenizePipeline(cli, pipeline->wordList, pipeline->stageList) == false)
    {
        printLog(cli, LogError, "Failed to tokenize pipeline. [Cli: %s]\n", cli->name);
        unInitPipeline(pipeline);
        return NULL;
    }
    // 命令行解析
    if (parsePipeline(cli, pipeline) == false)
    {
        printLog(cli, LogError, "Failed to parse pipeline. [Cli: %s]\n", cli->name);
        unInitPipeline(pipeline);
        return NULL;
    }
    return pipeline;
}

void unInitPipeline(Pipeline* pipeline)
{
    if (pipeline == NULL) return;

    if (pipeline->commandline != NULL)
    {
        free(pipeline->commandline);
        pipeline->commandline = NULL;
    }
    if (pipeline->wordList != NULL)
    {
        unInitWordList(pipeline->wordList);
        pipeline->wordList = NULL;
    }
    if (pipeline->stageList != NULL)
    {
        unInitStageList(pipeline->stageList);
        pipeline->stageList = NULL;
    }
    free(pipeline);
}

WordList* initWordList(void)
{
    WordList* list = NULL;
    list = calloc(1, sizeof(WordList));
    if (list == NULL) return NULL;

    list->count = 0;
    list->capacity = 5;
    list->words = calloc(list->capacity, sizeof(char*));
    if (list->words == NULL)
    {
        unInitWordList(list);
        return NULL;
    }
    else
    {
        return list;
    }
}

void unInitWordList(WordList* list)
{
    if (list == NULL) return;

    if (list->words != NULL)
    {
        for (uint16_t i = 0; i < list->count; i++)
        {
            if (list->words[i] != NULL)
            {
                free(list->words[i]);
                list->words[i] = NULL;
            }
        }
    }
    free(list->words);

    list->words = NULL;
    list->count = 0;
    list->capacity = 0;

    free(list);
}

bool addWord(WordList* list, char* word)
{
    if (list == NULL || word == NULL) return false;

    if (list->count+1 >= list->capacity)
    {
        const uint16_t newCapacity = list->capacity * 2;
        char** newWords = realloc(list->words, newCapacity*sizeof(char*));
        if (newWords == NULL)
        {
            return false;
        }
        list->words = newWords;
        list->capacity = newCapacity;
    }
    list->words[list->count] = word;
    list->count++;
    list->words[list->count] = NULL;
    return true;
}

PipelineStageList* initStageList(void)
{
    PipelineStageList* list = NULL;
    list = calloc(1, sizeof(PipelineStageList));
    if (list == NULL) return NULL;

    list->count = 0;
    list->capacity = 1;
    list->currentStage = NULL;
    list->stages = calloc(list->capacity, sizeof(PipelineStage*));
    if (list->stages == NULL)
    {
        unInitStageList(list);
        return NULL;
    }
    else
    {
        return list;
    }
}

void unInitStageList(PipelineStageList* list)
{
    if (list == NULL) return;

    if (list->stages != NULL)
    {
        for (uint16_t i = 0; i < list->count; i++)
        {
            unInitPipelineStage(list->stages[i]);
             list->stages[i] = NULL;
        }
    }
    free(list->stages);

    list->stages = NULL;
    list->currentStage = NULL;
    list->count = 0;
    list->capacity = 0;

    free(list);
}

bool addStage(PipelineStageList* stageList, PipelineStage* pipelineStage)
{
    if (stageList == NULL || pipelineStage == NULL) return false;

    if (stageList->count+1 >= stageList->capacity)
    {
        const uint16_t newCapacity = stageList->capacity * 2;
        PipelineStage** newPipelineStage = realloc(stageList->stages, newCapacity*sizeof(PipelineStage*));
        if (newPipelineStage == NULL)
        {
            return false;
        }
        stageList->stages = newPipelineStage;
        stageList->capacity = newCapacity;
    }
    stageList->stages[stageList->count] = pipelineStage;
    stageList->count++;
    stageList->stages[stageList->count] = NULL;
    return true;
}

PipelineStage* initPipelineStage(void)
{
    PipelineStage* stage = NULL;
    stage = calloc(1, sizeof(PipelineStage));
    if (stage == NULL)  return NULL;

    stage->wordList = initWordList();
    if (stage->wordList == NULL)
    {
        unInitPipelineStage(stage);
        return NULL;
    }
    return stage;
}

void unInitPipelineStage(PipelineStage* stage)
{
    if (stage == NULL) return;

    if (stage->wordList != NULL)
    {
        unInitWordList(stage->wordList);
        stage->wordList = NULL;
    }
    if (stage->argv != NULL)
    {
        for (uint16_t i = 0; i < stage->argc; i++)
        {
            free(stage->argv[i]);
        }
        free(stage->argv);
        stage->argv = NULL;
        stage->argc = 0;
    }
    stage->commandNode = NULL;
    stage->argc = 0;
    free(stage);
}

char* unescapeWord(const char* start, const char* end)
{
    if (start == NULL || end == NULL) return NULL;
    if (start > end) return NULL;

    char* result = NULL;
    char *tempPointer = NULL;
    const size_t length = (size_t)(end-start);

    result = calloc(length+1, sizeof(char));
    if (result == NULL) return NULL;
    /// Note 由于是逐个字符拷贝，拷贝过程中不可避免的会移动指针，因此需要使用一个tempPointer
    tempPointer = result;
    while (start != end)
    {
        /// Note 如果遇到反斜杠'\'那么跳过反斜杠的拷贝
        if (*start == '\\') start++;
        /// Note 将字符拷贝到对应的地址
        else *tempPointer++ = *start++;
    }
    *tempPointer = '\0';
    return result;
}

bool tokenizeCommandLine(const CliConfig* cli, const char* commandLine, WordList* wordList)
{
    if (commandLine == NULL || wordList == NULL) return false;

    const char* tempPointer = commandLine;
    const char* wordStart = 0;
    char quotationMark = 0;

    while (isspace(*tempPointer))   tempPointer++;
    wordStart = tempPointer;
    /// Note 分割命令行单词，并将命令行单词存储wordList中
    while (*tempPointer)
    {
        /// Note 如果遇到反斜杠即跳过反斜杠和反斜杠后面紧跟的字符
        if (*tempPointer == '\\' && *(tempPointer+1))   tempPointer += 2;

        if (*tempPointer == '\0'                /// Note 字符串结束
            || *tempPointer == quotationMark    /// Note 遇到引号
            || (wordStart != NULL               /// Note 如果当前单词的起始地址不为NULL
                && quotationMark == 0           /// Note 且当前指针不在引号内
                && (isspace(*tempPointer) || *tempPointer == '|')))  ///Note 遇到空格或者管道符
        {
            /// Note 添加单词
            if (wordStart)
            {
                if (addWord(wordList, unescapeWord(wordStart, tempPointer))==false) return false;
            }
            /// Note 字符串结束跳出循环
            if (!*tempPointer) break;
            if (quotationMark != 0) tempPointer++;
            quotationMark = 0;
            wordStart = 0;
        }
        else if (!quotationMark && (*tempPointer == '"' || *tempPointer == '\''))
        {
            if (wordStart && wordStart != tempPointer)
            {
                if (addWord(wordList, unescapeWord(wordStart, tempPointer))==false) return false;
            }
            quotationMark = *tempPointer++;
            wordStart = tempPointer;
        }
        else
        {
            if (wordStart == 0)
            {
                if (*tempPointer == '|')
                {
                    if (addWord(wordList, strdup("|")) == false) return false;
                }
                else if (!isspace(*tempPointer))
                {
                    wordStart = tempPointer;
                }
            }
            tempPointer++;
        }
    }
    if (quotationMark != 0)
    {
        printLog(cli,LogError,"Failed to parse command line: quote mismatch!\n");
        return false;
    }
    return true;
}

bool tokenizePipeline(const CliConfig* cli, const WordList* wordList, PipelineStageList* stages)
{
    if (cli == NULL || wordList == NULL || stages == NULL) return false;

    uint16_t lastPosition = 0;
    PipelineStage* tempStage = NULL;
    for (uint16_t i = 0; i < wordList->count; i++)
    {
        /// Note 在单词列表中查找到管道符'|'，或者已经到达单词列表的末尾时，说明一个管道阶段已经结束
        if (strcmp(wordList->words[i], "|") == 0 || i == wordList->count-1)
        {
            /// Note 创建一个管道阶段
            tempStage = initPipelineStage();
            if (tempStage == NULL)  return false;
            /// 将前面的单词添加到管道阶段的单词列表中
            for (uint16_t j = lastPosition; j < i; j++)
            {
                char* tempWord = strdup(wordList->words[j]);
                if (tempWord == NULL)
                {
                    unInitPipelineStage(tempStage);
                    return false;
                }
                if (addWord(tempStage->wordList, tempWord) == false)
                {
                    free(tempWord);
                    unInitPipelineStage(tempStage);
                    return false;
                }
            }
            if (i == wordList->count-1 && strcmp(wordList->words[i], "|") != 0)
            {
                char* tempWord = strdup(wordList->words[i]);
                if (tempWord == NULL)  return false;
                if (addWord(tempStage->wordList, tempWord) == false)
                {
                    free(tempWord);
                    unInitPipelineStage(tempStage);
                    return false;
                }
            }
            /// Note 将管道阶段添加到管道列表中
            if (addStage(stages, tempStage) == false)
            {
                unInitPipelineStage(tempStage);
                return false;
            }
            lastPosition = i+1;
            tempStage = NULL;
        }
    }
    return true;
}

bool parsePipeline(const CliConfig* cli, const Pipeline* pipeline)
{
    if (cli == NULL || pipeline == NULL) return false;
    if (cli->commandTree == NULL) return false;
    if (pipeline->stageList == NULL) return false;

    for (uint16_t i = 0; i < pipeline->stageList->count; i++)
    {
        PipelineStage* stage = pipeline->stageList->stages[i];
        if (stage == NULL || stage->wordList == NULL) return false;

        const CommandNode* parentNode = NULL;
        CommandNode* matchedNode = NULL;
        CommandNode* resultNode = NULL;
        uint16_t j = 0;
        for (; j < stage->wordList->count; j++)
        {
            const char* word = stage->wordList->words[j];
            if (word == NULL) return false;
            matchedNode = findCommand(cli, parentNode, word);
            if (matchedNode == NULL) break;
            resultNode = matchedNode;
            parentNode = resultNode;
        }
        if (resultNode == NULL) return false;
        stage->commandNode = resultNode;
        stage->argc = stage->wordList->count-j;
        if (stage->argc > 0)
        {
            stage->argv = calloc(stage->argc+1, sizeof(char*));
            if (stage->argv == NULL) return false;
            for (uint16_t k = 0; k < stage->argc; k++)
            {
                stage->argv[k] = strdup(stage->wordList->words[j+k]);
                if (stage->argv[k] == NULL)
                {
                    for (uint16_t n = 0; n < k; n++)
                    {
                        free(stage->argv[n]);
                    }
                    free(stage->argv);
                    stage->argv = NULL;
                    stage->argc = 0;
                    return false;
                }
            }
            stage->argv[stage->argc] = NULL;
        }
        else
        {
            stage->argv = NULL;
        }
    }
    return true;
}

bool executePipeline(const CliConfig* cli, const Pipeline* pipeline)
{
    if (cli == NULL || pipeline == NULL) return false;
    if (pipeline->stageList == NULL) return false;

    for (uint16_t i = 0; i < pipeline->stageList->count; i++)
    {
        PipelineStage* stage = pipeline->stageList->stages[i];
        if (stage == NULL || stage->commandNode == NULL) return false;

        const CommandNode* commandNode = stage->commandNode;
        if (commandNode->callback != NULL)
        {
            if (commandNode->callback(cli, commandNode->name, stage->argv, stage->argc) == false)
            {
                printLog(cli, LogError, "Failed to execute command. [Cmd: %s]\n", commandNode->fullName);
                return false;
            }
        }
    }
    return true;
}