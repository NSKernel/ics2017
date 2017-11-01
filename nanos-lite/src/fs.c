#include "fs.h"

extern void ramdisk_write(const void *buf, off_t offset, size_t len);
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern ssize_t dispinfo_read(void *buf, off_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

static const char filedispinfo[] = "WIDTH:400\nHEIGHT:300";

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  file_table[FD_FB].size = 400 * 320 * 4;
}

int fs_open(const char *pathname, int flags, int mode) {
  int i;
  for (i = 0; i < NR_FILES; i++) {
    if (!strcmp(pathname, file_table[i].name)) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("PANIC: File not found in fs_open.");
  return -1;
}

uintptr_t fs_getdiskoffset(int fd) {
  Log("\n\nCurrent offset of file %d is %d aka. 0x%08X\n\n", fd, file_table[fd].disk_offset, file_table[fd].disk_offset);
  return file_table[fd].disk_offset;
}

int fs_close(int fd) {
  return 0;
}

size_t fs_filesz(int fd) {
  return file_table[fd].size;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  if (fd < 3)
    return 0;
  switch (whence) {
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset = fs_filesz(fd) + offset;
      break;
    default:
      assert("ASSERT: Unexpected whence in fs_lseek");
  }
  return file_table[fd].open_offset;
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  ssize_t bytesread;
  switch (fd) {
    case 0:
    case 1:
    case 2:
      break;
    case FD_DISPINFO:
      return dispinfo_read(buf, file_table[fd].open_offset, len);
    default:
      bytesread = ((file_table[fd].open_offset + len <= fs_filesz(fd)) ? len : fs_filesz(fd) - file_table[fd].open_offset);
      if (bytesread >= 0) {
        ramdisk_read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, bytesread);
        file_table[fd].open_offset += bytesread;
        return bytesread;
      }
      return 0;
  }
  return 0;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  ssize_t byteswritten;
  switch (fd) {
    case FD_STDOUT:
    case FD_STDERR:
      byteswritten = 0;
      while(len--) {
        _putc(((char*)buf)[byteswritten]);
        byteswritten++;
      }
      return byteswritten;
    case FD_FB:
      
      break;
    case FD_DISPINFO:
      // read only
      return 0;
    default:
      byteswritten = ((file_table[fd].open_offset + len <= fs_filesz(fd)) ? len : fs_filesz(fd) - file_table[fd].open_offset);
      if (byteswritten >= 0) {
        ramdisk_write(buf, file_table[fd].open_offset + file_table[fd].disk_offset, byteswritten);
        file_table[fd].open_offset += byteswritten;
        return byteswritten;
      }
      return 0;
  }
  return 0;
}
