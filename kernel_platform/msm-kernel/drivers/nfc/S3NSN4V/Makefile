#
# Makefile for Samsung NRNx NFC driver
#

$(info "SAMSUNG NFC S3NSN4V IS BUILT AS DLKM")
obj-m += sec_nfc.o
BOARD_VENDOR_KERNEL_MODULES +=  $(KERNEL_MODULES_OUT)/sec_nfc.ko

ccflags-$(CONFIG_NFC_DEBUG) := -DDEBUG