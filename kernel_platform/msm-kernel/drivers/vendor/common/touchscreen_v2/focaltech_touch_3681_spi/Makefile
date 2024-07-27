# Makefile for the focaltech touchscreen drivers.
ccflags-y += -I$(srctree)/drivers/vendor/common/touchscreen_v2/
ccflags-y += -I$(srctree)/drivers/vendor/common/touchscreen_v2/focaltech_touch_3681_spi/focaltech_test/
ccflags-y += -I$(srctree)/drivers/vendor/common/touchscreen_v2/focaltech_touch_3681_spi/firmware_config/$(CONFIG_TOUCHSCREEN_BOARD_NAME)/
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)	+=  focaltech_core.o
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)	+=  focaltech_ex_fun.o
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)	+=  focaltech_ex_mode.o
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)	+=  focaltech_gesture.o
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)	+=  focaltech_esdcheck.o
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)   +=  focaltech_point_report_check.o
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)   +=  focaltech_test/


obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)	+=  focaltech_flash.o
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)   +=  focaltech_flash/
obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)	+=  focaltech_common_interface.o

obj-$(CONFIG_TOUCHSCREEN_FTS_3681_SPI)   +=  focaltech_spi.o
