#include "common.h"
#include "syscall.h"


uintptr_t sys_write(int fd, const void *buf, size_t count) {
  uintptr_t succ = 0;
  //Log("Print char");
  if (fd == 1 || fd == 2) {
    while(count--) {
      _putc(((char*)buf)[succ]);
      succ++;
    }
  }
  return succ;
}

int sys_brk(void *addr) {
  _heap.end = (void *)addr;
  return 0;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
    case SYS_none:
      // SYS_none, too short for a func.
      SYSCALL_ARG1(r) = 1; 
      break;
      
    case SYS_write:
      SYSCALL_ARG1(r) = sys_write(SYSCALL_ARG2(r), (void*)SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_exit:
      _halt(SYSCALL_ARG2(r));
      break;
    case SYS_brk:
      SYSCALL_ARG1(r) = sys_brk((void*)SYSCALL_ARG2(r));
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
