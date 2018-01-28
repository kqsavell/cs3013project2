#Kyle Savell & Antony Qin

obj-m += procAncestry.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -g testAncestry.c -o testAncestry
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f testAncestry
