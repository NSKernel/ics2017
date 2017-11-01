#include "common.h"

#define sticktog(a, b) a ## b

#define NAME(key) \
  [_KEY_##key] = #key,
  
extern _Screen _screen;
extern ssize_t _copy_pixle(const uint32_t *pixels, off_t offset, size_t len);
extern uint32_t* const fb;





static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

ssize_t dispinfo_read(void *buf, off_t offset, size_t len) {
  ssize_t it = 0;
  
  for (; it < len && offset + it < 128; it++) {
    ((char *)buf)[it] = dispinfo[offset + it];
  }
  Log("it = %d", it);
  return it;
}

ssize_t fb_write(const void *buf, off_t offset, size_t len) {
  return _copy_pixle((uint32_t *)buf, offset, len);
}

void init_device() {
  _ioe_init();
  
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", _screen.width, _screen.height);
}
