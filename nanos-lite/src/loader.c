#include "common.h"

extern void* new_page(void);

extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_close(int fd);
extern size_t fs_filesz(int fd);

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern uintptr_t fs_getdiskoffset(int fd);

extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  uint32_t i = 0;
  void *pageptr;
  for(i = 0; i < fs_filesz(fd); i += 0x1000) {
    pageptr = new_page();
    _map(as, (void *)(0x8048000 + i), pageptr);
    fs_read(fd, pageptr, ((fs_filesz(fd) - i >= 0x1000) ? 0x1000 : fs_filesz(fd) - i));
  }
  
  fs_close(fd);

  new_page();
  
  return (uintptr_t)DEFAULT_ENTRY;
}
