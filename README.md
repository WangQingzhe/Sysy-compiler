# SysY Compiler

用于实现SysY编译器的代码框架。

## Getting Started

建议使用Ubuntu系统，原生版本与WSL版本均可。

[Ubuntu下载与安装说明](https://ubuntu.com/download/desktop)
[WSL Ubuntu安装说明](https://learn.microsoft.com/en-us/windows/wsl/install)

SysY编译器前端基于[ANTLR](https://www.antlr.org/index.html)工具实现，本仓库已经包含ANTLR 4.9.3版本的可执行程序与C++运行时库，但编译ANTLR运行时库存在一些依赖，需要提前安装。

```bash
sudo apt install uuid-dev libutfcpp-dev pkg-config python3-pip make git
pip3 install cmake
```

依赖安装完成后，可以开始构建SysY编译器（构建过程包含了ANTLR运行时库的构建）。

```bash
git clone https://gitee.com/xsu1989/sysy.git
cd sysy
cmake -S . -B build
cmake --build build
```

构建完成后，可以运行一个小的测试用例。该测试将逗号分隔的整数或字符串列表进行格式化后重新输出，即将相邻参数之间的分隔统一调整为逗号外加一个空格。

```bash
cat /test/funcrparams.sysy
# -> 1,0xa ,  011,	"hellow"
./build/bin/sysyc test/funcrparams.sy
# -> 1, 0xa, 011, "hellow"
```

## Documentation

[ANTLR手册](doc/The%20Definitive%20ANTLR%204%20Reference.pdf)
[SysY语言规范](doc/sysy-2022-spec.pdf)
[SysY运行时库](doc/sysy-2022-runtime.pdf)

## 实验1：用ANTLR实现SysY词法/语法分析器  

当前的代码框架已经部署好了编译环境，同学们可专注于程序开发。

在实验1中，同学们需要完成的任务包括

- 参照SysY语言规范，修改`src/SysY.g4`文件，实现SysY词法/语法的完整定义
- 修改任意代码后需要重新执行`cmake --build build`命令重新构建项目，ANTLR工具会从`SysY.g4`生成词法/语法分析器，生成的文件位于`./build/src`目录
- （进阶内容）修改`src/ASTPrinter.h`与`src/ASTPrinter.cpp`，实现从AST输出源程序，但输出的源程序是经过格式化的，测试用例为`test/format-test.sy`，格式化后的参考结果为`test/format-ref.sy`
