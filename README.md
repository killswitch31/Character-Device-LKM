
This is a simlpe charater device which reads a strings from a User process and return the string to the user process when requested.



To compile this code:

1.$ make 

To execute:

1.$ sudo insmod chardev.ko

2.$ ./test write "HELLO_TSN2434"

3.$ ./test read 14
