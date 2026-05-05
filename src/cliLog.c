/**
 * @file cliLog.c
 * @brief 日志
 * @author ChenZR
 */
/* Includes" "------------------------------------------------------------------*/
#include "cliLog.h"
#include "cliConfig.h"
/* Includes< >------------------------------------------------------------------*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
/* Macro Definition------------------------------------------------------------------*/
/* DataType Definition------------------------------------------------------------------*/
/* Variable Definition-----------------------------------------------------*/
/* Functions Declare------------------------------------------------------------------*/
/**
 * @name getLogTime
 * @brief 读取系统时间戳，并返回”2026-05-03 09:20:37“样式的时间字符串
 * @details
 *
 * @return const char* 时间戳字符串
 */
const char* getLogTime(void);
/**
 * @name writeLevel
 * @brief 根据日志类型在日志内容前添加日志级别标签
 *
 * @param[in] cli CLI配置结构体
 * @param[in] contentLength 日志内容长度
 * @param[in] type 日志类型
 * @return uint16_t 添加日志级别标签后的日志内容长度
 */
uint16_t writeLevel(const CliConfig* cli, uint16_t contentLength, const LogType type);
/**
 * @name writeTime
 * @brief 在日志内容前添加时间戳
 *
 * @param[in] cli CLI配置结构体
 * @param[out] contentLength 日志内容长度
 * @return uint16_t 添加时间戳后的日志内容长度
 */
uint16_t writeTime(const CliConfig* cli, uint16_t contentLength);
/* Function Definition------------------------------------------------------------------*/
LogConfig* initLog(const char* filePath, const uint16_t bufferSize, const LogMode mode, const LogStyle style)
{
    if (filePath == NULL || bufferSize == 0) return NULL;

    LogConfig* log = NULL;
    log = calloc(1, sizeof(LogConfig));
    if (log == NULL) return NULL;

    log->mode = mode;
    log->style = style;
    log->bufferSize = bufferSize;
    log->filePath = strdup(filePath);
    if (log->filePath == NULL)
    {
        uninitLog(log);
        return NULL;
    }
    log->buffer = calloc(log->bufferSize, sizeof(char));
    if (log->buffer == NULL)
    {
        uninitLog(log);
        return NULL;
    }

    return log;
}

void uninitLog(LogConfig* logConfig)
{
    if (logConfig == NULL) return;

    if (logConfig->filePath) free(logConfig->filePath);
    logConfig->filePath = NULL;
    if (logConfig->buffer) free(logConfig->buffer);
    logConfig->buffer = NULL;

    free(logConfig);
}

void printLog(const CliConfig* cli, const LogType type, const char *format, ...)
{
    if (cli == NULL || format == NULL) return;
    if (cli->printCmd == NULL
        && (cli->logConfig->mode == CmdMode || cli->logConfig->mode == CFMode)) return;
    if (cli->printFile == NULL
        && (cli->logConfig->mode == FileMode || cli->logConfig->mode == CFMode)) return;

    va_list args;
    va_start(args, format);
    uint16_t contentLength = 0;
    contentLength = vsnprintf(cli->logConfig->buffer,
            cli->logConfig->bufferSize, format, args);
    /// Note vsnprintf 返回写入的字符数，不包括终止符，传递的时候所以进行了加1
    contentLength = writeLevel(cli,contentLength+1,type);
    writeTime(cli,contentLength+1);
    va_end(args);
    if (cli->logConfig->mode == CmdMode || cli->logConfig->mode == CFMode)
    {
        cli->printCmd(cli->logConfig->buffer);
    }
    if (cli->logConfig->mode == FileMode || cli->logConfig->mode == CFMode)
    {
        cli->printFile(cli->logConfig->buffer,cli->logConfig->filePath);
    }
}

const char* getLogTime(void)
{
    static char timeBuffer[20];
    const time_t timeNow = time(NULL);
    const struct tm* timeInfo = localtime(&timeNow);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    return timeBuffer;
}

uint16_t writeLevel(const CliConfig* cli, uint16_t contentLength, const LogType type)
{
    if (cli == NULL || cli->logConfig->style == TStyle || type == LogNormal) return contentLength;

    char tempBuffer[contentLength];
    snprintf(tempBuffer, contentLength, "%s", cli->logConfig->buffer);
    if (type == LogInfo)
    {
        contentLength = snprintf(cli->logConfig->buffer, cli->logConfig->bufferSize,
             "[INFO] %s",
             tempBuffer);
    }
    else if (type == LogWarning)
    {
        contentLength = snprintf(cli->logConfig->buffer, cli->logConfig->bufferSize,
             "[WARNING] %s",
             tempBuffer);
    }
    else if (type == LogError)
    {
        contentLength = snprintf(cli->logConfig->buffer, cli->logConfig->bufferSize,
             "[ERROR] %s",
             tempBuffer);
    }
    return contentLength;
}

uint16_t writeTime(const CliConfig* cli,uint16_t contentLength)
{
    if (cli == NULL || cli->logConfig->style == LStyle) return contentLength;

    char tempBuffer[contentLength];
    snprintf(tempBuffer, contentLength, "%s", cli->logConfig->buffer);
    contentLength = snprintf(cli->logConfig->buffer, cli->logConfig->bufferSize,
         "[%s] %s",
         getLogTime(),tempBuffer);
    return contentLength;
}