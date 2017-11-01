#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,
  
extern _Screen _screen;
extern ssize_t _copy_pixle(const uint32_t *pixels, off_t offset, size_t len);
extern uint32_t* const fb;

extern int _read_key();
extern unsigned long _uptime();

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  size_t retsize;
  int key = _read_key();
  char buffer[128];
  bool down = false;
  if (key & 0x8000) {
    key ^= 0x8000;
    down = true;
  }
  
  if (key != _KEY_NONE) {
    sprintf(buffer, "k%s %s\n", down ? "d" : "u", keyname[key]);
    retsize = strlen(buffer) > len ? len : strlen(buffer);
    memcpy(buf, buffer, retsize);
    return retsize;
  }
  else {
    sprintf(buffer, "t %d\n", _uptime());
    retsize = strlen(buffer) > len ? len : strlen(buffer);
    memcpy(buf, buffer, retsize);
    return retsize;
  }
}

static char dispinfo[128] __attribute__((used));

ssize_t dispinfo_read(void *buf, off_t offset, size_t len) {
  ssize_t it = 0;
  
  for (; it < len && offset + it < 128; it++) {
    ((char *)buf)[it] = dispinfo[offset + it];
  }
  return it;
}

ssize_t fb_write(const void *buf, off_t offset, size_t len) {
  return _copy_pixle((uint32_t *)buf, offset, len);
}

void init_device() {
  _ioe_init();
  
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", _screen.width, _screen.height);
}
