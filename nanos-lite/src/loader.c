#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern uintptr_t fs_getdiskoffset(int fd);

extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  ramdisk_read((void*)0x4000000, 0, get_ramdisk_size());
  //Log("\n\nLoad program at 0x%08X\n\n", fs_getdiskoffset(fs_open(filename, 0, 0)) + (uintptr_t)DEFAULT_ENTRY);
  return fs_getdiskoffset(fs_open(filename, 0, 0)) + (uintptr_t)DEFAULT_ENTRY;
}
