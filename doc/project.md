<font face="ubuntu mono">

###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队

-------------------------------------------------------------

# Makefile工程

当前的工程项目只有kernel一个大模块，用户代码不应写在kernel中。
kernel中也会分为多个模块，这些模块共同组成系统架构，架构图如下所示。

![架构图 - v1.1](./img/architecture-v1.1.png)
<center>架构图 - v1.1</center>
</br></br>

![架构图 - v1.1](./img/architecture-v1.2.png)
<center>架构图 - v1.2</center>

## I. 如何插入新模块到kernel

1. 新建模块到kernel/下，例如，smp. 
2. 在kernel/include/下新建smp. 
3. 在kernel/Makefile中为submod变量添加smp. 
