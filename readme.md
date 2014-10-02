# Nachos

CSE 120 Project for UCSD

## Install on the lab machines

To start and test:

```
$ git clone https://github.com/h8liu/nachos.git
$ cd nachos/code
$ make
$ cd threads
$ ./nachos
```

This should work on ieng machines.

## Install on a 64-bit Ubuntu machine

Nachos requires 32-bit headers and libraries. To install these on
Ubuntu 14.4 LTS 64-bit:

```
$ sudo apt-get install gcc-multilib g++-multilib lib32stdc++6
```

(Not sure if this works with other versions of Ubuntu or Linux.)

