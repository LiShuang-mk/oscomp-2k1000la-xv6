<font face="Liberation Mono">


###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队 

-------------------------------------------------------------

[`<= 回到目录`](../README.md)

# ~~关于HAL与CSR~~ *v2.x架构重大更改，本文档已废弃*

hal文件夹包含了硬件抽象层的代码，需要解释的是laregs.h是loongarch registers的HAL，这部分代码主要用于抽象出操作csr的统一接口，需要添加所需的csr操作函数请使用已经定义好的BUILD宏。

在内核中操作CSR应当使用Cpu类中定义的静态函数，而不应直接使用laregs中的函数，laregs中的代码属于C语言，而cpu.hh中的代码属于C++，cpu.hh中已经将C语言的函数封装成C++函数了。

在操作CSR的时候，应当参阅csr.hh中关于CSR的枚举。