#include <linux/init.h>           
#include <linux/module.h>         
#include <linux/device.h>         
#include <linux/kernel.h>         
#include <linux/fs.h>    
#include <linux/types.h>
#include <linux/errno.h>     
#include <linux/slab.h>     
#include <linux/uaccess.h> 

MODULE_LICENSE("GPL");            
MODULE_AUTHOR("Tapan Singh");    
MODULE_DESCRIPTION("A simple Linux char driver Read/Write");  
MODULE_VERSION("0.1");    

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define CLASS_NAME "chardev_class"
#define BUF_LEN 256

static int Major;
static int device_open_counter = 0;
//static char msg[BUF_LEN] = {0};
static short message_size;
//static char *msg_Ptr;
static struct class* chardevClass = NULL;
static struct device* chardevDevice = NULL;

char *tx_buffer;
char *rx_buffer;

int actual_rx_size = 0;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static int __init chardev_init(void){
	
	printk(KERN_INFO "\n**********chardev: Initializing the chardev LKM**********\n");
	
	Major = register_chrdev(0, DEVICE_NAME, &fops);
	
	if(Major<0){
		printk(KERN_INFO "chardev: Registering char device failed with Major Number: %d.\n", Major);
		return Major;
	}
	
	printk(KERN_INFO "chardev: Registering char device successfule with Major Number %d.\n", Major);	
	
	chardevClass = class_create(THIS_MODULE, CLASS_NAME);
	
	if(IS_ERR(chardevClass)){
		unregister_chrdev(Major, DEVICE_NAME);
		printk(KERN_INFO "chardev: Failed to register device class\n");
		return PTR_ERR(chardevClass);
	}
	
	printk(KERN_INFO "chardev: Device class registered SUCCESSFUL.\n");
	
	chardevDevice = device_create(chardevClass, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
	if(IS_ERR(chardevDevice)){
		class_destroy(chardevClass);
		unregister_chrdev(Major, DEVICE_NAME);
		printk(KERN_INFO "chardev: Failed to create the device.\n");
		return PTR_ERR(chardevDevice);
	}
	
	printk(KERN_INFO "chardev: Device class creation SUCCESSFUL.\n");
	
	tx_buffer = kmalloc(BUF_LEN, GFP_KERNEL);
	rx_buffer = kmalloc(BUF_LEN, GFP_KERNEL);
	
	if (!tx_buffer && !rx_buffer){
		printk(KERN_INFO "chardev: Memory allocation for tx and rx FAILED!\n");
		return -ENOMEM;
	}
	
	printk(KERN_INFO "chardev: Memory allocation for tx and rx SUCCESSFUL!\n");
	
	memset(tx_buffer, 0, BUF_LEN);
	memset(rx_buffer, 0, BUF_LEN);
	
	return SUCCESS;
}
	
static void __exit chardev_exit(void){
	
	device_destroy(chardevClass, MKDEV(Major, 0));
	class_unregister(chardevClass);
	class_destroy(chardevClass);
	unregister_chrdev(Major, DEVICE_NAME);
	
	if(tx_buffer)
		kfree(tx_buffer);
	if(rx_buffer)
		kfree(rx_buffer);
	
	printk(KERN_INFO "----------chardev: Goodbye from the LKM!----------\n\n");
}

static int device_open(struct inode *inodep, struct file *filep){
	
	device_open_counter++;
	
	printk(KERN_INFO "chardev: Device has been opened %d time(s).\n", device_open_counter);
	
	return SUCCESS;
}

static int device_release(struct inode *inodep, struct file *filep){
	
	printk(KERN_INFO "chardev: Device successfully closed\n");
    
    return SUCCESS;
}

static ssize_t device_read(struct file *filep, char *buffer, size_t length, loff_t *offset){

	ssize_t bytes;
	
	if(actual_rx_size < length){
		bytes = actual_rx_size;
	}
	else{
		bytes = length;
	}
	
	printk(KERN_INFO "chardev: User requesting (%d) bytes. tx_buffer has (%d) bytes\n", length, actual_rx_size);
	
	if(bytes > BUF_LEN){
		printk(KERN_INFO "chardev: User trying to read more than the allocted memory.\n");
		return -EFAULT;
	}
	
	int retval = copy_to_user(buffer, rx_buffer, bytes);
	
	if(retval){
		printk(KERN_INFO "chardev: copy_to_user FAILED!\n");
		return -EFAULT;
	}
	else{
		printk(KERN_INFO "chardev: copy_to_user SUCCESSFUL.\n");
		actual_rx_size -= bytes;
		return bytes; 
	}
}

static ssize_t device_write(struct file *filep, const char *buffer, size_t length, loff_t *offset){
	
	memset(tx_buffer, 0, BUF_LEN);
	memset(rx_buffer, 0, BUF_LEN);
	
	printk(KERN_INFO "chardev: Message received from User Space of Length: %lu bytes\n", length);
	if(length > BUF_LEN){
		printk(KERN_INFO "chardev: User attempting to write more than the allocated memory\n");
		return -EFAULT;
		}
	int retval = copy_from_user(tx_buffer, buffer, length);
	
	printk(KERN_INFO "chardev: Message received from User Space: [%s]\n", tx_buffer);
	
	memcpy(rx_buffer, tx_buffer, length);
	
	printk(KERN_INFO "chardev: memcpy(tx_buffer, rx_buffer, length) SUCCESSFUL. rx_buffer contains: [%s]\n", rx_buffer);
	
	actual_rx_size = length;
	
	return length;

}

module_init(chardev_init);
module_exit(chardev_exit);

















	
