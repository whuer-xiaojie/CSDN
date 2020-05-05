# globalmem_char_driver

## Install
    $: make
    $: sudo demsg -c
    $: sudo insmod multi_globalmem.ko
    $: lsmod
    $: sudo mknod /dev/globalmem c 230 0
    $: sudo dmesg

## Test
    $: gcc test_globalmem.c -o test_globalmem
    $: sudo ./test_globalmem message