#include "common.h"

extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_close(int fd);
extern size_t fs_filesz(int fd);

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern uintptr_t fs_getdiskoffset(int fd);

extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  fs_close(fd);
  
  return fs_getdiskoffset(fs_open(filename, 0, 0)) + (uintptr_t)DEFAULT_ENTRY;
}
