<font face="Liberation Mono">

###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队

-------------------------------------------------------------

# xv6-qemu-Loongarch-ls2k 

俺争取不掉队设计的基于qemu-ls2k的os是基于xv6-ls3A5000修改而来的。

### 搭建开发环境

1. 克隆项目到本地。
2. 打开ls2k_debug.sh，更改qemu路径，qemu使用龙芯实验室提供的[大赛qemu](https://github.com/LoongsonLab/2k1000-materials)。
3. 使用`make initdir`初始化构建路径，每次新增模块都应执行这个命名。
4. 使用`make all`构建项目。
5. 执行`ls2k_debug.sh`运行。


### 文档 

#### 1. [Makefile工程](./doc/project.md)