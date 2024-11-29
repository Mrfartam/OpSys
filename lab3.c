#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parfenov Ivan");
MODULE_DESCRIPTION("Simple Kernel module");
MODULE_VERSION("1.0");

int __init my_module_init(void){
  pr_info("Welcome to the Tomsk State University\n");
  return 0;
}

void __exit my_module_exit(void){
  pr_info("Tomsk State University forever!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
