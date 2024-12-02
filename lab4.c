#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>

#define PROC_FILENAME "lab4"

static char time_buffer[128];
static int time_buffer_len;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parfenov Ivan");
MODULE_DESCRIPTION("Simple Kernel module with time output for Cinderella");
MODULE_VERSION("2.0");

void calc_time(char *buffer, size_t buffer_size){
    struct timespec64 ts;
    struct tm tm;
    unsigned long seconds_now, seconds_left;
    int hours, minutes;

    ktime_get_real_ts64(&ts);

    time64_to_tm(ts.tv_sec + 7 * 3600, 0, &tm);

    seconds_now = tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec;
    seconds_left = 86400 - seconds_now;
    hours = seconds_left / 3600;
    minutes = (seconds_left % 3600) / 60 + 1;

    if(hours == 1 || hours == 21){
        if(minutes % 10 == 1 && minutes != 11)
            snprintf(buffer, buffer_size, "Через %d час, %d минуту карета обратится в тыкву\n", hours, minutes);
        else if((minutes % 10 == 2 || minutes % 10 == 3 || minutes % 10 == 4) && minutes / 10 != 1)
            snprintf(buffer, buffer_size, "Через %d час, %d минуты карета обратится в тыкву\n", hours, minutes);
        else
            snprintf(buffer, buffer_size, "Через %d час, %d минут карета обратится в тыкву\n", hours, minutes);
    }
    else if((hours % 10 == 2 || hours % 10 == 3 || hours % 10 == 4) && hours / 10 != 1){
        if(minutes % 10 == 1 && minutes != 11)
            snprintf(buffer, buffer_size, "Через %d часа, %d минуту карета обратится в тыкву\n", hours, minutes);
        else if((minutes % 10 == 2 || minutes % 10 == 3 || minutes % 10 == 4) && minutes / 10 != 1)
            snprintf(buffer, buffer_size, "Через %d часа, %d минуты карета обратится в тыкву\n", hours, minutes);
        else
            snprintf(buffer, buffer_size, "Через %d часа, %d минут карета обратится в тыкву\n", hours, minutes);
    }
    else{
        if(minutes % 10 == 1 && minutes != 11)
            snprintf(buffer, buffer_size, "Через %d часов, %d минуту карета обратится в тыкву\n", hours, minutes);
        else if((minutes % 10 == 2 || minutes % 10 == 3 || minutes % 10 == 4) && minutes / 10 != 1)
            snprintf(buffer, buffer_size, "Через %d часов, %d минуты карета обратится в тыкву\n", hours, minutes);
        else
            snprintf(buffer, buffer_size, "Через %d часов, %d минут карета обратится в тыкву\n", hours, minutes);
    }
    
}

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, size_t buffer_len, loff_t *offset){
    if(*offset > 0 || buffer_len < time_buffer_len)
        return 0;
    
    if(*offset == 0){
        calc_time(time_buffer, sizeof(time_buffer));
        time_buffer_len = strlen(time_buffer);
    }

    copy_to_user(buffer, time_buffer + *offset, time_buffer_len - *offset);

    pr_info("Чтение procfile %s\n", file_pointer->f_path.dentry->d_name.name);

    *offset += time_buffer_len - *offset;

    return time_buffer_len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

int __init my_module_init(void){
    pr_info("Welcome to the Tomsk State University\n");

    proc_create(PROC_FILENAME, 0, NULL, &proc_file_fops);

    pr_info("/proc/%s создан\n", PROC_FILENAME);
    return 0;
}

void __exit my_module_exit(void){
    remove_proc_entry(PROC_FILENAME, NULL);
    pr_info("Tomsk State University forever!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
