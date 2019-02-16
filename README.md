
This is a simlpe charater device which reads a strings from a User process and return the string to the user process when requested.



To compile this code:

1.$ make 

2.$ sudo insmod chardev

3.$ ./test write "HELLO_TSN2434"

4.$ ./test read 14
