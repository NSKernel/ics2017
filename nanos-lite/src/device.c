#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
}

void fb_write(const void *buf, off_t offset, size_t len) {
}

void init_device() {
  _ioe_init();
  
  char str[] = "WIDTH:400\nHEIGHT:300";
  char *src = (char *)str;
  char *buf = (char *)dispinfo;
  while((*buf++ = *src++));
}
