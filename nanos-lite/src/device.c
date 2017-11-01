#include "common.h"

#define sticktog(a, b) a ## b

#define NAME(key) \
  [_KEY_##key] = #key,
  
extern _Screen _screen;
extern void _copy_pixle(const uint32_t *pixels, off_t offset, size_t len);

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
  
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", _screen.width, _screen.height);
}
