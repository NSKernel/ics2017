#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound at 0x%08X", addr, cpu.eip); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

paddr_t page_translate(vaddr_t vaddr, bool is_write) {
  uint32_t DIR = vaddr >> 22;
  uint32_t PAGE = vaddr >> 12 & 0x003FF000;
  uint32_t OFFSET = vaddr & 0x00000FFF;

  uint32_t *PageTable = (uint32_t *)(((uint32_t *)cpu.cr3)[DIR] & 0xFFFFF000);
  assert(((uint32_t *)cpu.cr3)[DIR] & 0x00000001); // Present
  ((uint32_t *)cpu.cr3)[DIR] |= 0x00000020; // Set accessed
  uint32_t PageTableEntry = PageTable[PAGE];
  assert(PageTableEntry & 0x00000001); // Present
  PageTable[PAGE] |= 0x00000020; // Set accessed
  if (is_write) 
      PageTable[PAGE] |= 0x00000040; // Set dirty
  paddr_t PhysicalAddr = (PageTableEntry & 0xFFFFF000) + OFFSET;

  return PhysicalAddr;
}

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  if (is_mmio(addr) != -1)
    return mmio_read(addr, len, is_mmio(addr));
  else
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  if (is_mmio(addr) != -1)
    mmio_write(addr, len, data, is_mmio(addr));
  else
    memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  uint32_t OFFSET = addr & 0x00000FFF;

  if (OFFSET + len > 0x00001000) {
    int len1 = 0x00001000 - OFFSET;
    int len2 = len - len1;
    uint32_t part1 = paddr_read(page_translate(addr, false), len1); // low len1 bytes
    uint32_t part2 = paddr_read(page_translate(addr + len1, false), len2); // high len2 bytes
    // stich together
    return part1 + (part2 << (len1 * 8));
  }

  return paddr_read(page_translate(addr, false), len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  uint32_t OFFSET = addr & 0x00000FFF;
  
  if (OFFSET + len > 0x00001000) {
    int len1 = 0x00001000 - OFFSET;
    int len2 = len - len1;
    paddr_write(page_translate(addr, true), len1, data); // low len1 bytes
    data >>= (len1 * 8);
    paddr_write(page_translate(addr + len1, true), len2, data); // high len2 bytes
  }

  paddr_write(page_translate(addr, true), len, data);
}
