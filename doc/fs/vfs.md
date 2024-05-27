# VFS 虚拟文件系统
---------------------------
### VFS对象设计 

#### 1.SuperBlock
超级块的设计目前从简，只负责保存文件系统的一些元信息，就目前的FAT32而言，主要保存了Dbr。

#### 2.Dentry
Dentry是存在于缓存种的目录项，最初设计并没有考虑把他设计为一个基类，但是考虑到不同的FS中Dentry保存的属性可能不同，因此目前抽象为基类，它负责文件名和Inode之间的映射，目前只提供很少的接口。

#### 3.Inode
Inode负责保存文件的元数据，并为上层提供文件的一系列操作函数。

#### 4.FileSystem
FS，好像没什么好说的，主要保存挂载点，根节点......

#### 5.Path
路径，指定文件的路径，搜索路线

### 设计思路 && 一个进程是如何对一个文件进行操作
或许可以从进程是如何操作一个文件来体现出设计思路的：    

1. 进程使用 syscall_open(), 传递文件路径作为参数
2. VFS拿到这个路径后，对路径进行处理，包括但不限于：尝试从挂载表找到路径指向文件的FS的挂载点(pathHitTable), ；进入该FS进行search，拿到所需要文件的Dentry；获得Dentry指向的Inode，这时候就可以对File进行操作了。
 
目前需要提供的接口：  
我认为现在需要完善出一个能够给进程open一个file进行read的fs，其中缺少一些：
1. Fs的初始化仅仅初始化了根Root，根Dentry，因此如果我们想要去open一个了叫做 "echo_tesh"的文件的话，因为目前根Root的 `entry_children` 是空的，我们需要在inode中完善lookup，具体如下：   
    1.从根root的clusters中找到 `echo_test` 对应的clusters，并据此初始化一个新Inode，Dentry
2. 按照Kstat的标准给Fat32Dentry提供gid，uid等属性，用来初始化File的Data.kst（也不着急其实）
3.  再补充吧......  


