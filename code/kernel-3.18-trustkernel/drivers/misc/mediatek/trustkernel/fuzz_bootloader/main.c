#include<linux/kernel.h>
#include <linux/platform_device.h>
#include<linux/module.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/mm.h>
#include<linux/sched.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<asm/io.h>
#include<asm/uaccess.h>
#include<linux/semaphore.h>
#include<linux/slab.h>

#define MEM_CLEAR	0x1
#define MEM_MAJOR	253

static int mem_major = MEM_MAJOR;
static struct class *driver_class;
static dev_t devno;

struct mem_dev {
	struct cdev cdev;
	struct semaphore sem;
};

struct mem_dev *mem_devp = NULL;

int tk_mem_open(struct inode *inode, struct file *filp)
{
	if (mem_devp == NULL)
		return -EINVAL;

	if (filp == NULL)
		return -EINVAL;

	filp->private_data = mem_devp;
	return 0;
}

int tk_mem_release(struct inode *inode, struct file *filp)
{
	filp->private_data = NULL;
	return 0;
}

static ssize_t tk_mem_read(struct file *filp, char __user *buf,
			size_t size, loff_t *ppos)
{
	unsigned long vaddr = 0;
	if(copy_from_user(&vaddr, buf, sizeof(unsigned long))) {
		printk("failed to copy_from_user\n");
		return -EINVAL;
	}
	if(vaddr == 0 || size <= 0) {
		printk("tk_mem_read: bad param\n");
		return -EINVAL;
	}
	printk("tk_mem_read: read addr 0x%lx size 0x%lx\n",  vaddr, size);
	if(copy_to_user(buf, (void*)vaddr, size)) {
		printk("failed to read target memory 0x%lx\n", vaddr);
		return -EINVAL;
	}
	return 0;
}

static ssize_t tk_mem_write(struct file *filp, const char __user *buf,
			size_t size, loff_t *ppos)
{
	unsigned long paddr;
	unsigned long vaddr;
	if(buf == NULL || size <= 0) {
		printk("tk_mem_write: bad param\n");
		return -EINVAL;
	}
	paddr = *(unsigned long*)buf;
	printk("tk_mem_write: remap addr 0x%lx size 0x%lx\n",  paddr, size);
	vaddr = (unsigned long)ioremap_nocache(paddr, size);
	if(vaddr == 0) {
		printk("failed to remap addr: 0x%lx size: 0x%lx\n", paddr, size);
		return -EINVAL;
	}
	printk("tk_mem_write: remap addr 0x%lx to 0x%lx\n",  paddr, vaddr);
	return vaddr;
}

static const struct file_operations mem_fops = {
	.owner =		THIS_MODULE,
	.read =			tk_mem_read,
	.write =		tk_mem_write,
	.open =			tk_mem_open,
	.release =		tk_mem_release,
};

static void mem_setup_cdev(struct mem_dev *dev, int index)
{
	int err = 0;
	int devno = MKDEV(mem_major, index);

	cdev_init(&dev->cdev, &mem_fops);
	dev->cdev.owner = mem_fops.owner;
	err = cdev_add(&dev->cdev, devno, 1);

	if (err)
		pr_err("Error %d adding socket %d.\n", err, index);
}




int tk_mem_init(void)
{
	int result = 0;
	struct device *class_dev = NULL;
	devno = MKDEV(mem_major, 0);

	printk("<<<<<<<<<<<tk_mem init>>>>>>>>>>>>>>>");

	result = alloc_chrdev_region(&devno, 0, 1, "tk_mem");
	mem_major = MAJOR(devno);

	if (result < 0)
		return result;

	driver_class = class_create(THIS_MODULE, "tk_mem");

	if (IS_ERR(driver_class)) {
		result = -ENOMEM;
		pr_err("class_create failed %d.\n", result);
		goto unregister_chrdev_region;
	}

	class_dev = device_create(driver_class, NULL, devno, NULL, "tk_mem");

	if (!class_dev) {
		result = -ENOMEM;
		pr_err("class_device_create failed %d.\n", result);
		goto class_destroy;
	}

	mem_devp = kmalloc(sizeof(struct mem_dev), GFP_KERNEL);

	if (mem_devp == NULL) {
		result = -ENOMEM;
		goto class_device_destroy;
	}

	memset(mem_devp, 0, sizeof(struct mem_dev));
	mem_setup_cdev(mem_devp, 0);
	sema_init(&mem_devp->sem, 1);

	goto return_fn;

class_device_destroy:
	device_destroy(driver_class, devno);
class_destroy:
	class_destroy(driver_class);
unregister_chrdev_region:
	unregister_chrdev_region(devno, 1);
return_fn:
	return result;
}

void tk_mem_exit(void)
{
	device_destroy(driver_class, devno);
	class_destroy(driver_class);
	cdev_del(&mem_devp->cdev);
	kfree(mem_devp);
	unregister_chrdev_region(MKDEV(mem_major, 0), 1);
}

MODULE_AUTHOR("TrustKernel");
MODULE_LICENSE("GPL");

module_param(mem_major, int, S_IRUGO);

module_init(tk_mem_init);
module_exit(tk_mem_exit);
