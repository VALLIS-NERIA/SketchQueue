
obj-m += skq.o  
skq-objs += main.o countmax.o countsketch.o fss.o hashheap.o hashtable.o sketch_queue.o util.o 
CURRENT_PATH:=$(shell pwd)  
VERSION_NUM:=$(shell uname -r)  
LINUX_PATH:=/usr/src/linux-headers-$(VERSION_NUM)  
CC_FLAGS += --std=gnu99 -Wno-declaration-after-statement
  
all:  
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) modules  
  
clean:  
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) clean  
  