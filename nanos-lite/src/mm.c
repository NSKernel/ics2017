#include "proc.h"
#include "memory.h"

static void *pf = NULL;

void* new_page(void) {
  assert(pf < (void *)_heap.end);
  void *p = pf;
  pf += PGSIZE;
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {
  int iterator;
  void *pageptr;

  if (current->cur_brk == 0) {
    current->cur_brk = new_brk;
    current->max_brk = new_brk + (new_brk % 0x1000 != 0 ? (0x1000 - new_brk % 0x1000) : 0);
  }
  else {
    if (new_brk > current->max_brk) {
      for (iterator = current->max_brk; iterator < new_brk; iterator += 0x1000) {
        pageptr = new_page();
        _map(&(current->as), (void*)iterator, pageptr);
      }

      // Require max_brk to aligned to 4k
      // that is, max_brk got to be

      current->max_brk = new_brk + (new_brk % 0x1000 != 0 ? (0x1000 - new_brk % 0x1000) : 0);
    }

    current->cur_brk = new_brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _pte_init(new_page, free_page);
}
