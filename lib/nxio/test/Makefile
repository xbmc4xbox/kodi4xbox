XBE_TITLE = nxio
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/src/main.c $(CURDIR)/src/test.c
NXDK_DIR ?= $(CURDIR)/../nxdk

CFLAGS += -I../include -Iinclude
LDFLAGS += -L.. ../build/nxio.lib

include $(NXDK_DIR)/Makefile
