#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

MODULE_AUTHOR("Ryuichi Ueda and Ayaka Yui");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;

static volatile u32 *gpio_base = NULL;

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos){
	char c;
	int k;
	int i, j;
	int num = 4;
	int num2 = 6;

	if (copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;

	if (c == '0'){
		k = 0;		/*case０を実行*/
	} else if(c == '1'){
		k = 1;		/*case１を実行*/
	} else if(c == '2'){
		k = 2;		/*case２を実行*/
	} else if(c == '3'){
		k = 3;		/*case３を実行*/
	}
	
	switch(k){
		/*消灯*/
		case 0 :
			gpio_base[10] = 1 << 22;
			gpio_base[10] = 1 << 23;
			gpio_base[10] = 1 << 24;
			gpio_base[10] = 1 << 25;
			break;

		/*４つ同時に点滅*/
		case 1 :
			while(i <= num){
				gpio_base[7] = 1 << 22;
				gpio_base[7] = 1 << 23;
				gpio_base[7] = 1 << 24;
				gpio_base[7] = 1 << 25;
				ssleep(1);
				gpio_base[10] = 1 << 22;
				gpio_base[10] = 1 << 23;
				gpio_base[10] = 1 << 24;
				gpio_base[10] = 1 << 25;
				ssleep(1);

				i++;
			}
			break;

		/*２つずつ点滅*/
		case 2 :
			while(i <= num){
				gpio_base[7] = 1 << 22;
				gpio_base[7] = 1 << 24;
				ssleep(1);
				gpio_base[10] = 1 << 22;
				gpio_base[10] = 1 << 24;
				gpio_base[7] = 1 << 23;
				gpio_base[7] = 1 << 25;
				ssleep(1);
				gpio_base[10] = 1 << 23;
				gpio_base[10] = 1 << 25;

				i++;
			}
			break;

		/*当たり演出*/
		case 3 :
			while(i <= num){
				gpio_base[7] = 1 << 22;
				gpio_base[7] = 1 << 25;
				msleep(40);
				gpio_base[10] = 1 << 22;
				gpio_base[10] = 1 << 25;
				msleep(40);
				gpio_base[7] = 1 << 22;
				gpio_base[7] = 1 << 25;
				msleep(40);
				gpio_base[10] = 1 << 22;
				gpio_base[10] = 1 << 25;
				msleep(90);
				gpio_base[7] = 1 << 23;
				gpio_base[7] = 1 << 24;
				msleep(40);
				gpio_base[10] = 1 << 23;
				gpio_base[10] = 1 << 24;
				msleep(40);
				gpio_base[7] = 1 << 23;
				gpio_base[7] = 1 << 24;
				msleep(40);
				gpio_base[10] = 1 << 23;
				gpio_base[10] = 1 << 24;
				msleep(90);

				i++;
			}

			while(j <= num2){
				gpio_base[7] = 1 << 22;
				msleep(50);
				gpio_base[10] = 1 << 22;
				gpio_base[7] = 1 << 23;
				msleep(50);
				gpio_base[10] = 1 << 23;
				gpio_base[7] = 1 << 24;
				msleep(50);
				gpio_base[10] = 1 << 24;
				gpio_base[7] = 1 << 25;
				msleep(50);
				gpio_base[10] = 1 << 25;

				j++;
			}
			break;
	}
	return 1;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write
};

static int __init init_mod(void){
	int retval;

	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if (retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed. \n");
		return retval;
	}

	printk(KERN_INFO "%s is loaded. major:%d\n", __FILE__, MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if (retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d", MAJOR(dev), MINOR(dev));
		return retval;
	}

	cls = class_create(THIS_MODULE, "myled");
	if (IS_ERR(cls)){
		printk(KERN_ERR "class_create failed");
		return PTR_ERR(cls);
	}

	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));
	
	gpio_base = ioremap_nocache(0xfe200000, 0xA0);

	const u32 ledB1 = 22;
	const u32 indexB1 = ledB1 / 10;
	const u32 shiftB1 = (ledB1 % 10) * 3;
	const u32 maskB1 = ~(0x7 << shiftB1);
	gpio_base[indexB1] = (gpio_base[indexB1] & maskB1) | (0x1 << shiftB1);
	
	const u32 ledG = 23;
	const u32 indexG = ledG / 10;
	const u32 shiftG = (ledG % 10) * 3;
	const u32 maskG = ~(0x7 << shiftG);
	gpio_base[indexG] = (gpio_base[indexG] & maskG) | (0x1 << shiftG);

	const u32 ledR = 24;
	const u32 indexR = ledR / 10;
	const u32 shiftR = (ledR % 10) * 3;
	const u32 maskR = ~(0x7 << shiftR);
	gpio_base[indexR] = (gpio_base[indexR] & maskR) | (0x1 << shiftR);

	const u32 ledB2 = 25;
	const u32 indexB2 = ledB2 / 10;
	const u32 shiftB2 = (ledB2 % 10) * 3;
	const u32 maskB2 = ~(0x7 << shiftB2);
	gpio_base[indexB2] = (gpio_base[indexB2] & maskB2) | (0x1 << shiftB2);

	return 0;
}

static void __exit cleanup_mod(void){
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__, MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
