#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "global.h"

enum I_instruction {
    I_Halt,
    I_Up,
    I_Down,
    I_Move,
    I_Add,
    I_Sub,
    I_Neg,
    I_Mul,
    I_Test,
    I_Rts,
    I_Load_GP,
    I_Load_FP,
    I_Store_GP,
    I_Store_FP,
    I_Read_GP,
    I_Read_FP,
    I_Jsr,
    I_Jump,
    I_Jeq,
    I_Jlt,
    I_Loadi,
    I_Pop,
    I_Word,
};

void gen_prepare(void);

void gen_Halt(void);
void gen_Up(void);
void gen_Down(void);
void gen_Move(void);
void gen_Add(void);
void gen_Sub(void);
void gen_Neg(void);
void gen_Mul(void);
void gen_Test(void);
void gen_Rts(void);
void gen_Load_GP(int offset);
void gen_Load_FP(int offset);
void gen_Store_GP(int offset);
void gen_Store_FP(int offset);
void gen_Read_GP(int offset);
void gen_Read_FP(int offset);
void gen_Jsr(int address);
void gen_Jump(int address);
void gen_Jeq(int address);
void gen_Jlt(int address);
void gen_Loadi(int v);
void gen_Pop(int n);
void gen_Rts_Opt(void);

void backpatch(int i, int addr);

int get_next_code_index(void);

void gen_debug(void);

void translate_to_binary(void);
#endif /* end of include guard: INSTRUCTION_H_ */

