# Character-Device-LKM


This is a simlpe charater device which reads a strings from a User process and return the string to the user process when requested.

To compile this code:
1.$ make \n
2.$ sudo insmod chardev\n
3.$ ./test write "HELLO_TSN2434"\n
4.$ ./test read 14\n
