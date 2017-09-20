# NEMU

This is a fork of the original version of NEMU by the Department of Computer Science of Nanjing University. The repository is intended for learning purpose and is regarded as the programming assignment of the Introduction to Computer Systems class in autumn 2017. It is ok for you to clone and use the code of this project if you are NOT one of the class member, but YOU ARE NOT ALLOWED TO LOOK OR USE ANY PIECE OF CODE IN THIS PROJECT IF YOU ARE ONE OF THE CLASS MEMBER.



NEMU(NJU Emulator) is a simple but complete full-system x86 emulator designed for teaching purpose.
Small x86 programs can run under NEMU.

The main features of NEMU include
* a small monitor with a simple debugger
  * single step
  * register/memory examination
  * expression evaluation without the support of symbols
  * watch point
  * differential testing with QEMU
* CPU core with support of most common used x86 instructions in protected mode
  * real mode is not supported
  * x87 floating point instructions are not supported
* DRAM
* I386 paging with TLB
  * protection is not supported
* I386 interrupt and exception
  * protection is not supported
* 4 devices
  * serial, timer, keyboard, VGA
  * most of them are simplified and unprogrammable
* 2 types of I/O
  * port-mapped I/O and memory-mapped I/O
