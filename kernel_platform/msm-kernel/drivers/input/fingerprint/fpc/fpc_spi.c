/*
 * FPC Capacitive Fingerprint sensor device driver
 *
 * This driver will control the platform resources that the FPC fingerprint
 * sensor needs to operate. The major things are probing the sensor to check
 * that it is actually connected and let the Kernel know this and with that also
 * enabling and disabling of regulators, enabling and disabling of platform
 * clocks.
 *
 * The driver will expose most of its available functionality in sysfs which
 * enables dynamic control of these features from eg. a user space process.
 *
 * The sensor's IRQ events will be pushed to Kernel's event handling system and
 * are exposed in the drivers event node. This makes it possible for a user
 * space process to poll the input node and receive IRQ events easily. Usually
 * this node is available under /dev/input/eventX where 'X' is a number given by
 * the event system. A user space process will need to traverse all the event
 * nodes and ask for its parent's name (through EVIOCGNAME) which should match
 * the value in device tree named input-device-name.
 *
 *
 * Copyright (c) 2020-2021 Fingerprint Cards AB <tech@fingerprints.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License Version 2
 * as published by the Free Software Foundation.
 */
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/pm_wakeup.h>
#include <linux/regulator/consumer.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/kobject.h>

struct fpc_dev {
	s32 vdd;
	struct device *dev;
	struct class *fpc_class;
	struct device_node *node;
	dev_t  devt;
	struct cdev *c_dev;
};

#define FPC_MODULE_NAME "fpc1020"
#define GENERIC_OK    0
#define GENERIC_ERR  -1

#define FPC_ERR_CHECK_STATUS if (status) { pr_err("%s, err line:%d, status=%d\n", __func__, __LINE__, status); return GENERIC_ERR; }
#define FPC_ERR_CHECK_NULL(v) if (v == NULL) { pr_err("%s, err line:%d, %s is null\n", __func__, __LINE__, #v); return GENERIC_ERR; }

#define FPC_POWER_ON      "pn"
#define FPC_POWER_OFF     "pf"
#define FPC_GPIO_RELEASE  "rs"

static struct fpc_dev g_fpc_dev;
static void fpc_dev_destory(void);
static int fpc_dev_create(void);

static void fpc_gpio_release(void)
{
	pr_info("%s enter\n", __func__);
	if (gpio_is_valid(g_fpc_dev.vdd)) {
		devm_gpio_free(g_fpc_dev.dev, g_fpc_dev.vdd);
	}
	g_fpc_dev.vdd = -1;
}

static void fpc_power_off(void)
{
	pr_info("%s enter\n", __func__);
	if (gpio_is_valid(g_fpc_dev.vdd)) {
		gpio_set_value(g_fpc_dev.vdd, 0);
		pr_info("%s by gpio_set_value\n", __func__);
	}
}

static int fpc_power_on(void)
{
	int status = 0;

	pr_info("%s enter\n", __func__);
	if (g_fpc_dev.node == NULL) {
		g_fpc_dev.node = of_find_compatible_node(NULL, NULL, "fpc,fpc1020");
	}
	FPC_ERR_CHECK_NULL(g_fpc_dev.node)
	if (g_fpc_dev.vdd == -1) {
		g_fpc_dev.vdd = of_get_named_gpio(g_fpc_dev.node, "fpc_vdd", 0);
		pr_info("%s of_get_named_gpio vdd = %d\n", __func__, g_fpc_dev.vdd);
		if (gpio_is_valid(g_fpc_dev.vdd)) {
			status = devm_gpio_request(g_fpc_dev.dev, g_fpc_dev.vdd, FPC_MODULE_NAME);
			FPC_ERR_CHECK_STATUS
		}
	}
	if (gpio_is_valid(g_fpc_dev.vdd)) {
		pr_info("%s by gpio\n", __func__);
		gpio_direction_output(g_fpc_dev.vdd, 1);
	}
	return GENERIC_OK;
}
static int fpc_plat_probe(struct platform_device *dev)
{
	pr_info("%s", __func__);
	g_fpc_dev.dev = &dev->dev;
	return fpc_dev_create();
}

static int fpc_plat_remove(struct platform_device *dev)
{
	fpc_dev_destory();
	return GENERIC_OK;
}

static struct of_device_id fpc_of_match[] = {
	{ .compatible = "fpc,fpc1020", },
	{}
};
MODULE_DEVICE_TABLE(of, fpc_of_match);

static struct platform_driver fpc_plat_driver = {
	.driver = {
		.name = FPC_MODULE_NAME,
		.owner = THIS_MODULE,
		.of_match_table = fpc_of_match,
	},
	.probe = fpc_plat_probe,
	.remove = fpc_plat_remove,
};

static int __init fpc_init(void)
{
	pr_info("%s, fpc init enter\n", __func__);
	g_fpc_dev.vdd = -1;
	return platform_driver_register(&fpc_plat_driver);
}

static void __exit fpc_exit(void)
{
	platform_driver_unregister(&fpc_plat_driver);
}

static ssize_t fpc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int status = 0;
	const int len = 2;
	char cmd[8] = {0};

	pr_info("%s enter\n", __func__);
	status = copy_from_user(cmd, buf, len);
	FPC_ERR_CHECK_STATUS
	if (strncmp(cmd, FPC_POWER_ON, len) == 0) {
		fpc_power_on();
	} else if (strncmp(cmd, FPC_POWER_OFF, len) == 0) {
		fpc_power_off();
	} else if (strncmp(cmd, FPC_GPIO_RELEASE, len) == 0) {
		fpc_gpio_release();
	} else {
		pr_err("%s HELP:: id:read id, od:read optical id, pn:power on, pf:power off, rt:hw reset, rs:gpio release\n", __func__);
		return GENERIC_ERR;
	}
	return count;
}

static const struct file_operations fpc1020_ops = {
	.owner = THIS_MODULE,
	.write = fpc_write,
};

static int fpc_dev_create(void)
{
	struct device *c_device;
	int status = alloc_chrdev_region(&g_fpc_dev.devt, 0, 1, FPC_MODULE_NAME);

	pr_info("%s enter\n", __func__);
	FPC_ERR_CHECK_STATUS
	g_fpc_dev.fpc_class = class_create(THIS_MODULE, FPC_MODULE_NAME);
	if (IS_ERR(g_fpc_dev.fpc_class)) {
		pr_err("%s err class_create", __func__);
		return GENERIC_ERR;
	}
	c_device = device_create(g_fpc_dev.fpc_class, g_fpc_dev.dev, g_fpc_dev.devt, NULL, FPC_MODULE_NAME);
	if (IS_ERR(c_device)) {
		pr_err("%s err device_create\n", __func__);
		return GENERIC_ERR;
	}
	g_fpc_dev.c_dev = cdev_alloc();
	if (IS_ERR(g_fpc_dev.c_dev)) {
		pr_err("%s err cdev_alloc", __func__);
		return GENERIC_ERR;
	}
	cdev_init(g_fpc_dev.c_dev, &fpc1020_ops);
	status = cdev_add(g_fpc_dev.c_dev, MKDEV(MAJOR(g_fpc_dev.devt), 0), 1);
	FPC_ERR_CHECK_STATUS
	return GENERIC_OK;
}

static void fpc_dev_destory(void)
{
	pr_info("%s enter\n", __func__);
	fpc_power_off();
	fpc_gpio_release();
	device_destroy(g_fpc_dev.fpc_class, g_fpc_dev.devt);
	class_destroy(g_fpc_dev.fpc_class);
	cdev_del(g_fpc_dev.c_dev);
	unregister_chrdev_region(g_fpc_dev.devt, 1);
}

late_initcall(fpc_init);
module_exit(fpc_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("sheldon <sheldon.xie@fingerprints.com>");
