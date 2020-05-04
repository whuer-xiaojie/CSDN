# Linux 内核模快编程

## 模块编写
    1) 入口函数
        module_init(init_func_name);
        ```C
        int __init xxx_func(void)
        {

        }
        ```

    2) 出口函数
        module_exit(exit_func_name);
        ```C
        void __exit xxx_func(void)
        {

        }
        ```

    3) 模块信息声明
        MODULE_AUTHOR("whuer Xiaojie <1939346428@qq.com>");

        MODULE_LICENSE("GPLv2");

        MODULE_DESCRIPTION("A simple test module!");
    
## 内核模块加载

- 静态加载

    把模块编译进内核，通过修改内核进行编译：

    1) 将编写好的源代码复制到Linux内核源代码的相应目录中

    2) 在目录的Kconfig文件中增加关于新代码对应项目的编译配置选项

    3) 在目录的Makefile文件中增加对应的新代码的编译条目

    `缺点:`是这样编译出来的内核镜像会变大,且得重新编译内核所有源代码，比较麻烦，一般项目打包的时候才需要这样做

- 动态加载

    1) `$ sudo lsmod` 查看内核已经安装的所有模块（内核中已加载的内核模块存放于`sys/module`目录下）
   
    2) `$ sudo insmod XXX.ko`加载模块到内核（加载函数会被调用）也可以使用 `$ sudo modprobe XXX.ko`,该命令功能强大，在加载内核模块的同时会加载该模块所依赖的其他模块

    3) `$ rmmod XXX`移除模块（卸载函数会被调用）`$ modeprobe -r XXX`卸载通过`modprobe`加载的模块

    4) `$ dmesg`查看内核debug信息`$dmesg -C`清空所有历史debug信息

    5)  内核模块的加载和卸载函数只会被执行一次

    Makefile格式如下：
    ```
    #current path
    PWD = $(shell pwd)

    # kernel version
    KVERS =$(shell uname -r)
    #kernel dir
    KERNDIR =/lib/modules/${KVERS}/build/

    #Specify flags for module compilation
    #EXTRA_CFLAGS =-g -o0

    #kernel modules
    obj-m += hello.o

    # for multi source code file suce file1.c file2.c
    # obj-m += modulename.o
    # modulename-objs +=file1.o file2.o
    
    build: kernel_modules

    kernel_modules:
    	make -C $(KERNDIR) M=$(PWD) modules

    clean:
    	make -C $(KERNDIR) M=$(PWD) clean
    ```

## DEMO
```C
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void) 
{
    printk(KERN_INFO "%s: init called\n", __func__);
    return 0;
}

static void __exit hello_exit(void) 
{
    printk(KERN_INFO "%s: exit called\n",__func__);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Whuer Xiaojie <1939346428@qq.com>");
MODULE_LICENSE("GPLv2");
MODULE_DESCRIPTION("A simple test module!");
```