#include <am.h>

int main() {
  asm volatile("mov $0x8000000D, %eax");
  asm volatile("ror $1, %eax");
  asm volatile("ror $2, %eax");
  asm volatile("mov $0x8000D00D, %eax");
  asm volatile("ror $1, %ax");
  asm volatile("ror $3, %ax");
  asm volatile("mov $0x800000DD, %eax");
  asm volatile("ror $1, %al");
  asm volatile("ror $3, %al");
  asm volatile("mov $0xD000000D, %eax");
  asm volatile("rol $1, %eax");
  asm volatile("rol $2, %eax");
  asm volatile("mov $0xD000D00D, %eax");
  asm volatile("rol $1, %ax");
  asm volatile("rol $2, %ax");
  asm volatile("mov $0xD00000DD, %eax");
  asm volatile("rol $1, %al");
  asm volatile("rol $2, %al");
  return 0;
}
