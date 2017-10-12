#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0, &id_dest->val, &id_src->val);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  int32_t tempint = id_dest->val;
  tempint = ((tempint << (8 * (4 - id_dest->width))) >> (8 * (4 - id_dest->width)));
  rtl_sar(&t0, (uint32_t*)(&tempint), &id_src->val);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0); 
  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(shr);
}

make_EHelper(rol) {
  rtl_rol(&t0, &id_dest->val, &id_src->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(rol);
}

make_EHelper(ror) {
  rtl_ror(&t0, &id_dest->val, &id_src->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  // here we leave CF and OF unchanged
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(ror);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);
  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  t0 = id_dest->val;
  rtl_not(&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(not);
}
