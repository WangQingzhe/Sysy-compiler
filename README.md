# SysY Compiler

用于实现SysY编译器的代码框架。

## Getting Started

建议使用Ubuntu 22.04系统，原生版本与WSL版本均可。

[Ubuntu下载与安装说明](https://ubuntu.com/download/desktop)

[WSL Ubuntu安装说明](https://learn.microsoft.com/en-us/windows/wsl/install)

SysY编译器前端基于[ANTLR](https://www.antlr.org/index.html)工具实现，本仓库已经包含ANTLR 4.9.3版本的可执行程序与C++运行时库，但编译ANTLR运行时库存在一些依赖，需要提前安装。

```bash
sudo apt update
sudo apt install -y uuid-dev libutfcpp-dev pkg-config make git cmake openjdk-11-jre
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

## 实验2：从AST生成中间表示

exp2分支为大家准备好了进行实验2的基本代码框架，包括

- IR相关数据结构的定义：`src/IR.h`
- 创建IR对象的工具类`src/IRBuilder.h`
- IR生成器的示例代码`src/SysYIRGenerator.h`

在实验2中，同学们需要完成的任务包括

- 熟悉掌握IR定义与相关数据结构
- 从AST生成IR（基于visitor机制）

请同学们仔细阅读代码学习IR的定义。可以使用doxygen工具自动生成HTML文档

```bash
sudo apt install doxygen graphviz
doxygen doc/Doxyfile
```

上述命令执行完毕后，将在doxygen/html下找到生成的代码文档。

## 实验3：从SysY IR 生成ARMv7汇编代码
### 后端相关源码
当前ref2分支为ARMv7后端代码实验,已经包含了后端代码生成的代码框架,包含

- 后端生成代码源文件`src/backend/codegen.cpp`  
- 后端生成代码头文件`src/backend/codegen.hpp`

本实验需要基于以上两个源文件添加ARMv7后端生成代码(也可以按照自己的设计重头编写整个后端生成代码),完成这两个源文件中所有空函数的实现.不局限于本实验提供的后端代码框架,自由设计自己的ARMv7后端实现.

### 后端代码的编译与运行  
本实验也修改了驱动代码sysyc.cpp, sysyc.cpp可以调用后端生成的最顶层函数接口code_gen().该函数会逐层调用各层级的代码生产函数并最终生成ARMv7汇编代码并打印至屏幕.通过下列命令编译
```bash 
cmake -S . -B build
cmake --build build
```
通过下列命令运行编译产生的sysyc 
```bash  
./sysyc 01_add.sy
```
或者通过下列命令只生成SysY IR代码
```bash  
./sysyc 01_add.sy ir
```
### 测试  
本实验提供了两个sysy源文件用于测试,分别是位于sysy-backend/test的01_add.sy  11_add2.sy; 当完成sysyc的编译器后端后,可以通过sysy-backend/test下的Makefile文件编译生成测试程序的可执行二进制.

#### 在x86平台编译运行测试代码
##### 下载并配置ARMv7交叉编译器工具链
在[Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)下载安装交叉编译工具链,并设置环境变量PATH来使用交叉编译工具链
```bash  
export PATH=${your_arm-gnu-toolchain_path}/bin:$PATH;  
```
##### 安装qemu模拟器
```bash 
sudo apt update
sudo apt install qemu-system-arm qemu-user
```
##### 编译测试程序
使用如下命令调用完成后端实现的sysyc生成ARMv7汇编代码,并调用ARMv7交叉编译工具链汇编并连接生成可执行文件.
```bash 
cd test
../build/bin/sysyc 01_add.sy > 01_add.s
arm-none-linux-gnueabihf-gcc 01_add.s -o 01_add.out -static #注意使用-static选项来静态链接  
```

##### 使用qemu-arm模拟运行测试程序
```bash  
cd test
qemu-arm ./01_add.out
echo $? #查看测试程序返回值
```

##### 使用makefile编译与运行
```bash
cd test
make all -r #利用sysyc编译两个测试程序
make run -r #使用qemu-arm模拟运行两个测试程序
```
#### 在树莓派上编译运行测试代码
参见'Raspberry.pptx'设置树莓派的编译运行环境,也可以在自己的x86电脑上利用ARMv7交叉编译工具链来编译测试程序, 将编译产生的可执行文件上传到树莓派上运行.


### 交叉编译器生成汇编代码
可以通过如下的命令让交叉编译器生成汇编代码,以参考gcc后端的编译行为.  
```bash  
cd test  
cp 01_add.sy 01_add.c # 修改代码后缀名  
arm-none-linux-gnueabihf-gcc 01_add.c  -O0 -S  -o 01_add.S #O0编译优化  
```
请自己构造一些简单的c程序,阅读交叉编译器编译产生的汇编代码来理解ARMv7汇编代码与编译器后端行为.  

### 参考文档
参见doc/backend/下的ARMv7相关文档