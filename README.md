# AtomicCli

---
>- 项目：AtomicCli(命令行解释器)
>- 语言：C
>- 作者：ChenZR
>- 系列：Atomic
>- 简介：基于C语言开发的命令行解释器库，其后续会应用于Sophonix系列项目中，目前仅支持日志的格式化输出、命令树构建、命令行字符解析、命令回调调用等基础功能。

---

### 第一章	总体介绍

- 项目结构：
  - include：
    - cliConfig.h：命令行解释器库的入口头文件
    - cliLog.h：日志打印输出头文件
    - cliCommand.h：命令头文件
    - cliPipeline.h：管道头文件
  - src：
    - cliConfig.c：命令行解释器库的入口源文件
    - cliLog.c：日志打印输出头文件
    - cliCommand.c：命令头文件
    - cliPipeline.c：管道头文件

### 第二章	cliConfig

- `CliConfig`：CLI配置结构体

  ```c
  typedef struct CliConfig             
  {
      char* name;                     ///< name CLI 配置的名字
      LogConfig* logConfig;           ///< logConfig 日志
      CommandNode* commandTree;       ///< commandTree 命令树
  
      RegisterCommandTree registerCommandTree; ///< registerCommandTree 命令树注册函数指针
      PrintCmdFunction printCmd;      ///< cmdPrint 命令行打印函数指针
      PrintFileFunction printFile;    ///< filePrint 文件打印函数指针
  }CliConfig;						  ///< CliConfig CLI 配置结构体
  ```

  >1. `name`：CLI名称
  >
  >2. `logConfig`：日志功能配置结构体，通过调用`initLog`进行初始化
  >
  >3. `commandTree`：命令树，通过调用`registerCommandTree`进行初始化
  >
  >4. `registerCommandTree`：命令树配置函数指针
  >
  >   ```c
  >   /**
  >    * @name *RegisterCommandTree
  >    * @brief 配置命令树函数指针
  >    * @details 调用命令注册函数实现命令树的构建
  >    *
  >    * @param[in] cli CLI结构体指针
  >    * @return CommandNode* 命令树根节点指针
  >    */
  >   typedef CommandNode* (*RegisterCommandTree)(CliConfig* cli);
  >   ```
  >
  >5. `printCmd`：
  >
  >   ```c
  >   /**
  >    * @name PrintCmdFunction
  >    * @brief 日志输出函数指针
  >    * @details 将相关字符打印在屏幕或串口设备上
  >    *
  >    * @param[in] logContent 日志内容
  >    * @return none
  >    *
  >    * @note 若不需要日志输出功能，则定义一个空函数并将其地址传入CLI配置中，并配置日志模式为FileMode
  >    */
  >   typedef void (*PrintCmdFunction)(const char* logContent);
  >   ```
  >
  >6. `printFile`：
  >
  >   ```c
  >   /**
  >    * @name PrintFileFunction
  >    * @brief 日志文件输出函数指针
  >    * @details 将相关字符写入至对应的文件中
  >    *
  >    * @param[in] logContent 日志内容
  >    * @param[in] file 文件名称
  >    * @return none
  >    *
  >    * @note 若不需要日志文件输出功能，则定义一个空函数并将其地址传入CLI配置中，并配置日志模式为CmdMode
  >    */
  >   typedef void (*PrintFileFunction)(const char* logContent, const char* file);
  >   ```

- `initCli`：CLI结构体初始化

  ```c
  /**
   * @name initCli
   * @brief 初始化CLI
   *
   * @param[in] name cli名称
   * @param[in] filePath 日志文件路径
   * @param[in] registerCommandTree 命令树注册函数
   * @param[in] printCmd 终端打印函数
   * @param[in] printFile 文件打印函数
   * @return CliConfig* CLI配置结构体指针
   */
  CliConfig* initCli(const char* name, const char* filePath,
      const RegisterCommandTree registerCommandTree,
      const PrintCmdFunction printCmd,
      const PrintFileFunction printFile);
  ```

  >1. 依据形参内容给`cliConfig`结构体的属性赋初值
  >2. 调用`initLog`函数以初始化`logConfig`
  >3. 调用`registerCommandTree`以初始化commandTree

- `unInitCli`：释放CLI资源

  ```C
  /**
   * @name unInitCli
   * @brief 释放CLI
   *
   * @param[in] cli CLI结构体指针
   */
  void unInitCli(CliConfig* cli);
  ```

  >1. 初始化结构体过程中给name分配了内存，在此进行释放
  >2. 调用`uninitLog`释放日志相关资源
  >3. 调用`unRegisterCommandTree`释放命令树相关资源
  >4. 将其余参数置为0，指针置为NULL

- `runCli`：运行CLI

  ```c
  /**
   * @name runCli
   * @brief 运行CLI
   *
   * @param[in] cli CLI结构体指针
   * @param[in] input 字符串输入
   * @return none
   */
  void runCli(const CliConfig* cli, const char* input);
  ```

  >1. 调用`initPipeline`初始化管道
  >2. 如果初始化成功则调用`executePipeline`执行相关回调函数
  >3. 执行完成后调用`unInitPipeline`释放管道资源

### 第三章	cliLog

- `LogType`：日志类型枚举

  ```c
  typedef enum
  {
      LogNormal,      ///< LogNormal 普通日志
      LogError,       ///< LogError 错误日志
      LogWarning,     ///< LogWarning 警告日志
      LogInfo         ///< LogInfo 信息日志
  }LogType;
  ```

- `LogStyle`：日志风格枚举

  ```c
  typedef enum
  {
      TLStyle,       ///< TLStyle 显示时间戳和日志等级
      LStyle,        ///< LStyle 只显示日志等级
      TStyle,        ///< TStyle 只显示时间戳
  }LogStyle;
  ```

- `LogMode`：日志模式枚举

  ```c
  typedef enum
  {
      CFMode,         ///< CFMode 同时输出到终端和日志文件
      CmdMode,        ///< CmdMode 只输出到终端
      FileMode,       ///< FileMode 只输出到日志文件
  }LogMode;
  ```

- `LogConfig`：日志配置结构体

  ```c
  typedef struct
  {
      char* filePath;         ///< filePath 日志文件指针
      uint16_t bufferSize;    ///< bufferSize 日志缓冲区大小
      char* buffer;           ///< buffer 日志缓冲区
      LogMode mode;           ///< mode 输出模式
      LogStyle style;         ///< style 输出样式
  }LogConfig;
  ```

  >1. `filePath`：日志文件输出路径描述
  >2. `bufferSize`：日志缓冲区大小，默认配置为1024
  >3. `buffer`：日志缓冲区
  >4. `mode`：日志模式
  >5. `style`：日志样式
  >
  >   ```c
  >   [INFO]
  >   [2025-05-07 07:10:30]
  >   [2025-05-07 07:10:30] [INFO]
  >   ```

- `initLog`：日志初始化函数

  ```c
  /**
   * @name initCli
   * @brief 初始化CLI
   *
   * @param[in] name cli名称
   * @param[in] filePath 日志文件路径
   * @param[in] registerCommandTree 命令树注册函数
   * @param[in] printCmd 终端打印函数
   * @param[in] printFile 文件打印函数
   * @return CliConfig* CLI配置结构体指针
   */
  CliConfig* initCli(const char* name, const char* filePath,
      const RegisterCommandTree registerCommandTree,
      const PrintCmdFunction printCmd,
      const PrintFileFunction printFile);c
  ```

- `uninitLog`：日志资源释放函数

  ```c
  /**
   * @name unInitCli
   * @brief 释放CLI
   *
   * @param[in] cli CLI结构体指针
   */
  void unInitCli(CliConfig* cli);
  ```

- `printLog`：日志打印函数

  ```c
  /**
   * @name runCli
   * @brief 运行CLI
   *
   * @param[in] cli CLI结构体指针
   * @param[in] input 字符串输入
   * @return nonec
   */
  void runCli(const CliConfig* cli, const char* input);
  ```


### 第四章	cliCommand

- `CommandNode`：命令节点数据结构体

  ```c
  typedef struct CommandNode
  {
      char* name;                 ///< name 当前节点的名字
      char* fullName;             ///< fullName 完整命令路径
      CommandNode *previous;      ///< previous 命令树同级前一个节点
      CommandNode *next;          ///< next 命令树同级下一个节点
      CommandNode *children;      ///< children 命令树下一级第一个子节点
      CommandNode *parent;        ///< parent 命令树父节点
      CommandFunction callback;   ///< callback 命令执行函数指针
  }CommandNode;
  ```

  >1. `name`：命令节点的名称
  >
  >   > [!NOTE]
  >   >
  >   > 示例：
  >   >
  >   > ```
  >   > set led on/off
  >   > set oled on/off
  >   > set timer on/off
  >   > get led status
  >   > get oled status
  >   > get timer status
  >   > ```
  >   >
  >   > 命令树：set
  >   >
  >   > ​                 |-----led(on/off)-----oled(on/off)-----timer(on/off)   
  >   >
  >   > ​               get
  >   >
  >   > ​                 |-----led(status)-----oled(status)-----timer(status)
  >
  >2. `fullName`：不包含选项参数的完整命令路径
  >
  >3. `previous`：同级别前一个命令节点
  >
  >4. `next`：同级别下一个命令节点
  >
  >5. `children`：当前节点的子节点
  >
  >6. `parent`：当前节点的父节点
  >
  >7. `callback`：命令回调函数

- `registerCommand`：

  ```c
  /**
   * @name registerCommand
   * @brief 命令注册函数
   *
   * @param[in] cli 命令行结构体指针
   * @param[in] name 命令名称
   * @param[in] parent 父命令节点
   * @param[in] callback 命令执行函数指针
   * @return Command* 注册的命令节点结构体，
   *
   * @note 如果是第一次注册那么应将其返回值赋值给cli->commandTree
   * @note 如果不是第一次第一次注册且parent不为NULL，则节点挂载在对应的父节点下
   * @note 如果不是第一次第一次注册且parent为NULL，则节点挂载在根命令列表的末尾
   */
  CommandNode* registerCommand(CliConfig* cli, const char *name ,CommandNode* parent, CommandFunction callback);
  ```

- `unRegisterCommand`：

  ```c
  /**
   * @name unRegisterCommand
   * @brief 命令注销函数
   *
   * @param[in] cli 命令行结构体指针
   * @param[in] command 命令节点
   * @return none
   */
  void unRegisterCommand(CliConfig* cli,CommandNode* command);
  ```

- `unRegisterCommandTree`：

  ```c
  /**
   * @name unRegisterCommandTree
   * @brief 命令树注销函数
   *
   * @param[in] cli 命令行结构体指针
   * @return none
   */
  void unRegisterCommandTree(CliConfig *cli);
  ```

- `findCommand`：

  ```c
  /**
   * @name findCommand
   * @brief 查询命令节点
   *
   * @param[in] cli CLI结构体指针
   * @param[in] parentNode 预查询命令的父节点，如果是一级节点，则父节点输入NULL
   * @param[in] name 预查询命令的名称
   * @return CommandNode* 若查询成功则返回命令节点指针，查询失败则返回NULL
   */
  CommandNode* findCommand(const CliConfig* cli, const CommandNode* parentNode, const char* name);
  ```

### 第五章	cliPipeLine

- `WordList`：单词列表

  ```c
  typedef struct WordList {
      char** words;           ///< words 单词数组
      uint16_t count;         ///< count 单词数量
      uint16_t capacity;      ///< capacity 链表容量
  }WordList;                  ///< WordList 单词列表
  ```

  >1. `words`：一个存储char*的数组
  >2. `count`：存储了多少个单词
  >3. `capacity`：最多可以存储多少个单词

- `PipelineStage`：

  ```c
  typedef struct PipelineStage
  {
      WordList* wordList;           ///< words 单词列表
      CommandNode* commandNode;     ///< commandNode 当前阶段匹配到的命令节点
      char** argv;                  ///< argv 当前阶段的参数单词数组
      uint16_t argc;                ///< argc 当前阶段的参数单词数量
  }PipelineStage;                   ///< PipelineStage 管道阶段
  ```

  >1. `wordList`：单词列表指针
  >2. `commandNode`：命令节点
  >3. `argv`：当前阶段的参数单词数组
  >4. `argc`：当前阶段的参数单词数量

- `PipelineStageList`：

  ```c
  typedef struct PipelineStageList
  {
      PipelineStage** stages;         ///< stages 管道数组
      PipelineStage* currentStage;    ///< currentStage 当前正在处理的管道阶段
      uint16_t count;                 ///< count 管道数量
      uint16_t capacity;              ///< capacity 管道容量
  }PipelineStageList;				   ///< PipelineStageList 管道阶段链表
  ```

  >1. `stages`：管道数组
  >2. `currentStage`：当前正在处理的管道阶段
  >3. `count`：管道数量
  >4. `capacity`：管道容量

- `Pipeline`：

  ```c
  typedef struct Pipeline
  {
      char* commandline;              ///< commandline 保存原始命令行字符串
      WordList* wordList;             ///< wordList 按空格等规则拆分后的单词数组
      PipelineStageList* stageList;   ///< stageList 管道阶段列表
  }Pipeline;                          ///< Pipeline 管道
  ```

  >1. `commandline`：保存原始命令行字符串
  >2. `wordList`：按空格等规则拆分后的单词数组
  >3. `stageList`：管道阶段列表

- `initPipeline`：

  ```c
  /**
   * @name initPipeline
   * @brief 管道初始化
   *
   * @param[in] cli CLI结构体
   * @param[in] commandLine 命令行字符串
   * @return Pipeline* 初始化成功返回Pipeline指针，失败返回NULL
   */
  Pipeline* initPipeline(const CliConfig* cli, const char* commandLine);
  ```

- `unInitPipeline`：

  ```c
  /**
   * @name unInitPipeline
   * @brief 释放管道资源
   *
   * @param[in] pipeline 管道资源
   * @return void
   */
  void unInitPipeline(Pipeline* pipeline);
  ```

- `executePipeline`：

  ```c
  /**
   * @name executePipeline
   * @brief 执行管道
   *
   * @param[in] cli CLI配置结构体
   * @param[in] pipeline 管道
   * @return 管道执行成功则返回true，失败则返回false
   *
   * @note
   * @warning
   */
  bool executePipeline(const CliConfig* cli, const Pipeline* pipeline);
  ```

  