<font face="Ubuntu Mono">


###### OS大赛 - 内核设计loongarch赛道 - 俺争取不掉队 

-------------------------------------------------------------

[`<= 回到文件系统`](../fs.md)

# 缓存池设计

## 1. 缓存池结构概述

缓存池采用多级设计，可参考下图。

![](../img/os-buffer-pool.png){:height=%70 width=70%}

## 2. 缓存池结构说明

缓存池有64个block，每个block都会有若干个buffer（默认64个），最大上限为64个，可以动态更新（尚未实现）。每个buffer都是一个页面的大小（4KiB），因而一个block为256KiB。整个缓存池的大小最大可达16MiB，并且动态更新可以调整内存的使用率。

实际上，block号就是LBA的低6位，高位为tag，用于在block内部唯一地标识同一设备内的一个buffer。

block内部是所有buffer组成的链表，这个链表用于实现LRU算法，每次分配buffer时使用最近最少使用的buffer，而频繁使用的buffer可以得到保留，命中时就不会重新从硬盘中读入。每个buffer在block内部拥有一个索引index，是每个buffer本体的唯一索引，它可以用于加速搜索buffer的地址，而不必遍历整个链表。

> 关于index的说明: 
> 在使用时，buffer是按照LRU算法分配的，这意味着buffer与硬盘里的区域是动态映射的。block号和tag号并不能保证每次都找到同一个buffer。而index是buffer在block内部的唯一标识，实际上就是buffer数组的下标，不管buffer与硬盘区域如何映射，block号和index号能保证每次都找到同一个buffer。即，block+index能找到同一块内存区域，而不能保证找到LBA对应的磁盘数据；block+tag则能保证找到LBA对应的磁盘数据，但不保证buffer是同一块。
