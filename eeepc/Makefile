obj-m += hello.o
hello-objs := hello_main.o ec.o

KVERSION := $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build $(INCLUDE) M=$(PWD) modules
	rm -r -f .tmp_versions *.mod.c .*.cmd *.symvers
clean:
	make -C /lib/modules/$(KVERSION)/build $(INCLUDE) M=$(PWD) clean
