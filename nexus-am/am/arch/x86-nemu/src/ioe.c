#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
#define KEY_REG 0x60
#define KEY_STATUS 0x64
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  return inl(RTC_PORT) - boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  int i;
  int j;
  for (i = y; i < y + h && i < _screen.height; i++) {
    for (j = x; j < x + w && j < _screen.width; j++)
      fb[i * _screen.width + j]=pixels[(i - y) * w + j - x];
  }
}

void _draw_sync() {
}

int _read_key() {
  if (inb(KEY_STATUS)) {
    return inl(KEY_REG);
  }
  return _KEY_NONE;
}
