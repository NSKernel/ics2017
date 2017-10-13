#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  rtl_lm(&t0, &id_dest->addr, 2);
  cpu.idtr.limit = t0;
  t1 = id_dest->addr + 2;
  rtl_lm(&t0, &t1, 4);
  cpu.idtr.base = (t0 & (decoding.is_operand_size_16 ? 0x00FFFFFF : 0xFFFFFFFF));

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  raise_intr(id_dest->val, *eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  rtl_pop(&t0);
  decoding.jmp_eip = t0;
  decoding.is_jmp = 1;
  rtl_pop(&t0);
  cpu.cs = t0;
  rtl_pop(&t0);
  cpu.eflags = t0;
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  t0 = pio_read(id_src->val, id_dest->width);
  rtl_sr(R_EAX, id_dest->width, &t0);
  
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  t0 = 0;
  rtl_sr(R_EAX, id_dest->width, &t0);
  pio_write(id_dest->val, id_src->width, id_src->val);
  
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
