<font face="Maple Mono SC NF">

###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队

-------------------------------------------------------------

# XN6 - Xn6 is Not XV6

###### [`<!重要!> 自2024年4月25日起本仓库迁移至gitlab进行开发`](https://gitlab.eduxiji.net/T202410486992576/OSKernel2024-2k1000la-xv6.git)

俺争取不掉队设计的 XV6 OS 是基于 xv6-ls3A5000 修改而来的。XV6 OS主要使用C++开发，具有较为良好的可读性和可迁移性。

当前开发进度：

1. 架构已从1.x版本升级至2.0+，架构发生重大改变，最后一次开发1.x版本可见 [`tag v1.2`](https://github.com/LiShuang-mk/oscomp-2k1000la-xv6/tree/v1.2)

2. 已适配glibc用户程序，可以启动busybox-ash。

### 文档 

#### 0. [=> 开发环境搭建指引](./doc/develop-environmnet.md)

#### 1. [=> 工程架构](./doc/project.md)

#### 2. [=> 硬件接口设计](./doc/hsai.md)

#### 3. [=> HSAI 参考](./doc/hsai_reference.md)

#### 4. [=> ls2k的IO方式](./doc/ls2k_io.md)

#### 5. [=> 文件系统](./doc/fs.md)

#### 6. [=> 动态内存](./doc/dyn_mem.md)

#### 7. [=> 空间地址划分](./doc/memlayout.md)

#### 8. [=> uboot启动与调试](./doc/how_to_uboot.md)

#### 9. [=> 如何适配使用glibc的用户程序](./doc/how_to_adapt_glibc.md)
