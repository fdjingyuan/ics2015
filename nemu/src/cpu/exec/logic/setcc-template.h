#include "cpu/exec/template-start.h"

static void concat3(instr,_execute_,SUFFIX)() {
	if(instr_value) OPERAND_W(op_src, 1);
	else OPERAND_W(op_src, 0);

	print_asm(str(instr) " %s", op_src->str);
}

make_helper(concat3(instr,_rm_,SUFFIX)){
	return idex(eip,concat(decode_rm_,SUFFIX),concat3(instr,_execute_,SUFFIX));
}

#include "cpu/exec/template-end.h"
