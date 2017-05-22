#include "cpu/exec/helper.h"

#define DATA_BYTE 1

#define instr seto
#define instr_value (cpu.OF==1)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setno
#define instr_value (cpu.OF==0)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setb
#define instr_value (cpu.CF==1)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setae
#define instr_value (cpu.CF==0)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr sete
#define instr_value (cpu.ZF==1)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setne
#define instr_value (cpu.ZF==0)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setbe
#define instr_value (cpu.CF==1||cpu.ZF==1)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr seta
#define instr_value (cpu.CF==0&&cpu.ZF==0)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr sets
#define instr_value (cpu.SF==1)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setns
#define instr_value (cpu.SF==0)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setp
#define instr_value (cpu.PF==1)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setnp
#define instr_value (cpu.PF==0)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setl
#define instr_value (cpu.SF!=cpu.OF)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setge
#define instr_value (cpu.SF==cpu.OF)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setle
#define instr_value ((cpu.SF!=cpu.OF)||cpu.ZF==1)
#include "setcc-template.h"
#undef instr_value
#undef instr
#define instr setg
#define instr_value ((cpu.SF==cpu.OF)&&cpu.ZF==0)
#include "setcc-template.h"
#undef instr_value
#undef instr

#undef DATA_BYTE
