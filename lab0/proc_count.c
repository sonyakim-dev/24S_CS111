#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

// initialize a pointer to to the /proc/count
static struct proc_dir_entry *entry;

// get called when the /proc/count is read
// counts the number of processes currently running on the system
static int proc_count(struct seq_file *m, void *v){
	struct task_struct *task;
	unsigned int count = 0;
	for_each_process(task){ // macro that iterates all the processes
		count++;
	}
	// formatted output to a seq_file
	seq_printf(m, "%d\n", count);
	return 0;
}

static int __init proc_count_init(void)
{
	// create a single-entry /proc file
	entry = proc_create_single("count", 0, NULL, proc_count);
	pr_info("proc_count: init\n");
	return 0;
}

static void __exit proc_count_exit(void)
{
	proc_remove(entry);
	pr_info("proc_count: exit\n");
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Soyeon Kim");
MODULE_DESCRIPTION("CS111 lab0 count proc number");
MODULE_LICENSE("GPL");
