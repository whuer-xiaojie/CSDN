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

MODULE_AUTHOR("whuer Xiaojie <1939346428@qq.com>");
MODULE_LICENSE("GPLv2");
MODULE_DESCRIPTION("A simple test module!");