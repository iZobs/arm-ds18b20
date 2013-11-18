/*************************************************************************
	> File Name: DS18B20.c
	> Author: izobs
	> Mail: ivincentlin@gmail.com
	> Created Time: 2013年07月13日 星期六 08时22分57秒
 ************************************************************************/
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <mach/map.h>
#include <mach/regs-clock.h>
#include <linux/cdev.h>
#include <asm/unistd.h>
#include <linux/string.h>
#include <linux/miscdevice.h>

#define DQ  S3C2410_GPF4
#define CPG_IN S3C2410_GPF4_INP
#define CPG_OUT S3C2410_GPF4_OUTP

#define DEVICE_NAME "DS18B20"
#define DS18B20_MAJOR 32

#define DEBUG 0

static int ds18b20_major;
static unsigned char data[2] = { 0x00, 0x00 };

static unsigned char init_ds18b20(void)
{
	unsigned int ret = 1;
	s3c2410_gpio_cfgpin(DQ, CPG_OUT);	/*配置pin为输出 */
	s3c2410_gpio_pullup(DQ, 1);	/*拉高DQ口,为下面拉低准备 */
	s3c2410_gpio_setpin(DQ, 0);
	udelay(480);
	s3c2410_gpio_setpin(DQ, 1);	/*将18b20总线拉高，以便在15-60us后接送18b20的脉冲 */
	udelay(100);

	s3c2410_gpio_cfgpin(DQ, CPG_IN);	/*配置DQ为输出 */
	ret = s3c2410_gpio_getpin(DQ);
	udelay(400);

#if DEBUG
	printk("kernel:init ds18b20 is called,ret is %d\n", ret);
#endif

	return ret;
}

/*写一个字节*/

static void write_a_byte(unsigned char data)
{
	unsigned char i = 0;
	for (i = 0; i < 8; i++) {
		s3c2410_gpio_cfgpin(DQ, CPG_OUT);
		s3c2410_gpio_setpin(DQ, 1);
		udelay(2);
		s3c2410_gpio_setpin(DQ, 0);
		udelay(2);

		if (data & 0x01)	/*取出data的低位写入 */
			s3c2410_gpio_setpin(DQ, 1);
		else
			s3c2410_gpio_setpin(DQ, 0);
		udelay(50);
		s3c2410_gpio_setpin(DQ, 1);
		udelay(45);
		data >>= 1;
	}
	udelay(10);
}

//读一个字节
static unsigned char read_a_byte(void)
{
	unsigned char i;
	unsigned char data = 0x00;
	for (i = 0; i < 8; i++) {
		s3c2410_gpio_setpin(DQ, 1);
		s3c2410_gpio_cfgpin(DQ, CPG_OUT);

		udelay(2);
		s3c2410_gpio_setpin(DQ, 0);
		udelay(2);

		s3c2410_gpio_setpin(DQ, 1);
		s3c2410_gpio_cfgpin(DQ, CPG_IN);
		data >>= 1;
		if (s3c2410_gpio_getpin(DQ)) {
#if DEBUG
			printk("kernel:get_pin is %d\n",
			       s3c2410_gpio_getpin(DQ));
#endif

			data |= 0x80;	/*从高位读开始读 */
		}
		s3c2410_gpio_setpin(DQ, 1);
		udelay(70);
	}
	return data;
}

int open_ds18b20(struct inode *inode, struct file *filp)
{
	if (init_ds18b20() > 0)
		printk("kernel:init_ds18b20 failed\n");
	else
		printk("kernel:open device and inited\n");
	return 0;
}

static ssize_t read_ds18b20(struct file *filp, char __user * buf, size_t size,
			    loff_t * ppos)
{
	unsigned long err;
#if DEBUG
	printk("kernel:read_ds18b20 is called\n");
#endif

	init_ds18b20();
	write_a_byte(0xcc);	/*跳过读序列号操作 */
	write_a_byte(0x44);	/*启动温度转换 */
	mdelay(780);

	init_ds18b20();
	write_a_byte(0xcc);
	write_a_byte(0xbe);

	data[0] = read_a_byte();	/*低位 */
	data[1] = read_a_byte();	/*高位 */

	err = copy_to_user(buf, &data, sizeof(data));	/*将数据考贝到应用层 */

#if DEBUG
	printk("kernel:data[0] is %x,data[1] is %x\n", data[0], data[1]);
#endif

	return err ? -EFAULT : min(sizeof(data), size);
}

int release_ds18b20(struct inode *inode, struct file *filp)
{
	printk("device release\n");
	return 0;
}

static struct file_operations ds18b20_fops = {
	.owner = THIS_MODULE,
	.read = read_ds18b20,
	.open = open_ds18b20,
	.release = release_ds18b20,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &ds18b20_fops,
};

static int __init ds18b20_init(void)
{
	int ret;
	ret = misc_register(&misc);
	printk(DEVICE_NAME "init da18b20.......\n");
	return ret;
}

/*模块卸载函数*/
static void __exit ds18b20_exit(void)
{
	misc_deregister(&misc);
}

MODULE_AUTHOR("izobs");
MODULE_LICENSE("Dual BSD/GPL");

module_param(ds18b20_major, int, S_IRUGO);

module_init(ds18b20_init);
module_exit(ds18b20_exit);
