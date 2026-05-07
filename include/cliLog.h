/**
* @file cliLog.h
 * @brief 日志
 * @author ChenZR
 */
#ifndef CLILOG_H
#define CLILOG_H
#ifdef __cplusplus
extern "C" {
#endif
/* Includes" "------------------------------------------------------------------*/
/* Includes< >------------------------------------------------------------------*/
#include <stdint.h>
/* Forward Declaration------------------------------------------------------------------*/
typedef struct CliConfig CliConfig;
/* DataType Definition------------------------------------------------------------------*/
typedef enum
{
    LogNormal,      ///< LogNormal 普通日志
    LogError,       ///< LogError 错误日志
    LogWarning,     ///< LogWarning 警告日志
    LogInfo         ///< LogInfo 信息日志
}LogType;
typedef enum
{
    TLStyle,       ///< TLStyle 显示时间戳和日志等级
    LStyle,        ///< LStyle 只显示日志等级
    TStyle,        ///< TStyle 只显示时间戳
}LogStyle;
typedef enum
{
    CFMode,         ///< CFMode 同时输出到终端和日志文件
    CmdMode,        ///< CmdMode 只输出到终端
    FileMode,       ///< FileMode 只输出到日志文件
}LogMode;
typedef struct LogConfig
{
    char* filePath;         ///< filePath 日志文件指针
    uint16_t bufferSize;    ///< bufferSize 日志缓冲区大小
    char* buffer;           ///< buffer 日志缓冲区
    LogMode mode;           ///< mode 输出模式
    LogStyle style;         ///< style 输出样式
}LogConfig;
/* Variable Declare------------------------------------------------------------------*/
/* Functions Declare------------------------------------------------------------------*/
/**
 * @name initLog
 * @brief 初始化日志
 *
 * @param[in] filePath 日志输出文件路径
 * @param[in] bufferSize 缓冲区大小
 * @param[in] mode 日志模式 可选项：CFMode 同时输出到终端和日志文件 CmdMode 只输出到终端 FileMode 只输出到日志文件
 * @param[in] style 日志样式 可选项：TLStyle 显示时间戳和日志等级 LStyle 只显示日志等级 TStyle 只显示时间戳
 * @return LogConfig* 日志配置结构体指针
 */
LogConfig* initLog(const char* filePath, const uint16_t bufferSize, const LogMode mode, const LogStyle style);
/**
 * @name uninitLog
 * @brief 释放日志资源
 *
 * @param[in] logConfig 日志配置结构体指针
 * @return none
 */
void uninitLog(LogConfig* logConfig);
/**
 * @name printLog
 * @brief 打印日志
 *
 * @param[in] cli CLI结构体指针
 * @param[in] type 日志类型 可选项：LogNormal 普通日志 LogError 错误日志 LogWarning 警告日志 LogInfo 信息日志
 * @param[in] format 字符串
 * @param[in] ... 额外形参
 * @return none
 */
void printLog(const CliConfig* cli, const LogType type, const char *format, ...);
#ifdef __cplusplus
}
#endif
#endif
