obj-m += char_device.o  # Add your driver source file to the module list

KDIR := /lib/modules/$(shell uname -r)/build  # Path to the kernel build directory

all:
    $(MAKE) -C $(KDIR) M=$(PWD) modules  # This command compiles the module

clean:
    $(MAKE) -C $(KDIR) M=$(PWD) clean  # This command cleans up the build artifacts
