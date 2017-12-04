#include "common.h"
#include "syscall.h"

extern int fs_open(const char *pathname, int flags, int mode);
extern int fs_close(int fd);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int mm_brk(uint32_t new_brk);

int sys_brk(void *addr) {
  return mm_brk((uint32_t)addr);;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
    case SYS_none:
      // SYS_none, too short for a func.
      SYSCALL_ARG1(r) = 1; 
      break;
    case SYS_open:
      SYSCALL_ARG1(r) = fs_open((void*)SYSCALL_ARG2(r), SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_read:
      SYSCALL_ARG1(r) = fs_read(SYSCALL_ARG2(r), (void*)SYSCALL_ARG3(r), SYSCALL_ARG4(r)); 
      break;
    case SYS_write:
      SYSCALL_ARG1(r) = fs_write(SYSCALL_ARG2(r), (void*)SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_exit:
      _halt(SYSCALL_ARG2(r));
      break;
    case SYS_close:
      SYSCALL_ARG1(r) = fs_close(SYSCALL_ARG2(r));
      break;
    case SYS_lseek:
      SYSCALL_ARG1(r) = fs_lseek(SYSCALL_ARG2(r), SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_brk:
      SYSCALL_ARG1(r) = sys_brk((void*)SYSCALL_ARG2(r));
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
