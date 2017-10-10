#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  t0 = id_dest->val;
  rtl_push(&t0);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
  rtl_sr(id_dest->reg, id_dest->width, &t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  t1 = cpu.esp;
  t0 = cpu.eax;
  rtl_push(&t0);
  t0 = cpu.ecx;
  rtl_push(&t0);
  t0 = cpu.edx;
  rtl_push(&t0);
  t0 = cpu.ebx;
  rtl_push(&t0);
  rtl_push(&t1);
  t0 = cpu.ebp;
  rtl_push(&t0);
  t0 = cpu.esi;
  rtl_push(&t0);
  t0 = cpu.edi;
  rtl_push(&t0);
  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&t0);
  cpu.edi = t0;
  rtl_pop(&t0);
  cpu.esi = t0;
  rtl_pop(&t0);
  cpu.ebp = t0;
  rtl_pop(&t0); // throw esp
  rtl_pop(&t0);
  cpu.ebx = t0;
  rtl_pop(&t0);
  cpu.edx = t0;
  rtl_pop(&t0);
  cpu.ecx = t0;
  rtl_pop(&t0);
  cpu.eax = t0;
  print_asm("popa");
}

make_EHelper(leave) {
  reg_l(4) = reg_l(5);
  rtl_pop(&t0);
  reg_w(5) = t0;
  
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    if(reg_w(0) < 0)
        reg_w(2) = 0xFFFF;
    else
        reg_w(2) = 0;
  }
  else {
    if(reg_l(0) < 0)
        reg_l(2) = 0xFFFFFFFF;
    else
        reg_l(2) = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    t1 = reg_b(0);
    rtl_sext(&t0, &t1, 1);
    reg_w(0) = t0;
  }
  else {
    t1 = reg_w(0);
    rtl_sext(&t0, &t1, 2);
    reg_l(0) = t0;
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
