/*-
 * Copyright (c) 2013, Meitian Huang <_@freeaddr.info>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Generates instructions
 *
 * This file maintains an array of instructions and a counter (initialised to
 * 0). Every time a gen_* function is called, a new instruction is insert into
 * the array at the position specified by the counter and the counter increment
 * by one.
 */

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

/**
 * Generates instructions
 *
 * Their names are already descriptive...
 */
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

/**
 * Backpatches/change the target address of the instruction at @i to @addr
 */
void backpatch(int i, int addr);

/**
 * @returns the index of next instruction (not yet generated)
 */
int get_next_code_index(void);

/**
 * Outputs the assembly code
 */
void gen_debug(void);

/**
 * Outputs the binary code
 */
void translate_to_binary(void);
#endif /* end of include guard: INSTRUCTION_H_ */

