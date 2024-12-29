// SPDX-License-Identifier: UNLICENSED
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Khinochek Vladislav IO-21 <Vladhinochek@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_PARM_DESC(printCount, "Iterations of printing 'Hello, world!'");

static unsigned int printCount = 1;
module_param(printCount, uint, 0444); // Parameter of type uint

// Structure for the list
struct timestamp_event {
	struct list_head list;
	ktime_t timestamp;
};

// List head
static LIST_HEAD(eventList);

static int __init hello_init(void)
{
	unsigned int i = 0;

	if (printCount == 0 || (printCount >= 5 && printCount <= 10))
		pr_warn("Warning: Continuing...\n");

	if (printCount > 10) {
		pr_err("Error: Module not loaded!\n");
		// Module load error
		return -EINVAL;
	}

	// Print "Hello, world!" printCount times
	for (i = 0; i < printCount; i++) {
		// Allocate memory for an event
		struct timestamp_event *event = kmalloc(sizeof(*event),
			 GFP_KERNEL);

		// Get the current kernel time
		event->timestamp = ktime_get();
		// Add to the list
		list_add_tail(&event->list, &eventList);
		pr_info("Hello, world!\n");
	}
	return 0;
}

static void __exit hello_exit(void)
{
	struct timestamp_event *event;
	struct list_head *pos,  *tmp;

	// Traverse the list
	list_for_each_safe(pos, tmp, &eventList) {
		event = list_entry(pos, struct timestamp_event, list);
		pr_debug("Start of debugging");
		pr_debug("Event timestamp: %lld ns\n",
			 ktime_to_ns(event->timestamp));
		list_del(pos);	// Remove the element from the list
		kfree(event);	// Free memory
	}
	pr_debug("End of debugging");

}

module_init(hello_init);
module_exit(hello_exit);
