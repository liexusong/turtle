/*-
 * Copyright (c) 2013, Meitian Huang <_@freeaddr.info>
 * Copyright (c) 2013, Siyuan Zhang <u4761413@uds.anu.edu.au>
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

#include "global.h"
#include "instruction.h"

/**
 * 16-bit target machine anyway...
 */
#define INSTRUCTIONS_SIZE (2<<16)

int             next_code_index = 0;

/**
 * Compact representation of an instruction
 */
struct instruction {
    enum I_instruction kind;
    int             op;
};

/**
 * We use a static array because it is easier...
 */
static struct instruction instructions[INSTRUCTIONS_SIZE];

/**
 * @return the _number_ corresponds to @i
 *
 * It is up to the caller to decide how to print the number.
 */
static int      do_translate_to_binary(struct instruction *i);
/**
 * @returns the two's complement representation of @i
 */
static int      two_complement(int i);

static int
two_complement(int i)
{
    if (i > 0x7f) { // Won't work...
        assert(0);
        panic();
        return 0; // Not reachable
    } else if (i < 0) {
        return i + 0x100;
    } else {
        return i;
    }
}

void
gen_debug(void)
{
    printf("Total instructions: %d\n", next_code_index);

    for (int i = 0; i < next_code_index; ++i) {
        if (lflag) {
            fprintf(fout, "%d  ", i);
        }

        switch (instructions[i].kind) {
        case I_Halt:
            fprintf(fout, "Halt\n");
            break;

        case I_Up:
            fprintf(fout, "Up\n");
            break;

        case I_Down:
            fprintf(fout, "Down\n");
            break;

        case I_Move:
            fprintf(fout, "Move\n");
            break;

        case I_Add:
            fprintf(fout, "Add\n");
            break;

        case I_Sub:
            fprintf(fout, "Sub\n");
            break;

        case I_Neg:
            fprintf(fout, "Neg\n");
            break;

        case I_Mul:
            fprintf(fout, "Mul\n");
            break;

        case I_Test:
            fprintf(fout, "Test\n");
            break;

        case I_Rts:
            fprintf(fout, "Rts\n");
            break;

        case I_Load_GP:
            if (instructions[i].op >= 0) {
                fprintf(fout, "Load %d GP\n", instructions[i].op);
            } else {
                fprintf(fout, "Load (%d) GP\n", instructions[i].op);
            }

            break;

        case I_Load_FP:
            if (instructions[i].op >= 0) {
                fprintf(fout, "Load %d FP\n", instructions[i].op);
            } else {
                fprintf(fout, "Load (%d) FP\n", instructions[i].op);
            }

            break;

        case I_Store_GP:
            if (instructions[i].op >= 0) {
                fprintf(fout, "Store %d GP\n", instructions[i].op);
            } else {
                fprintf(fout, "Store (%d) GP\n", instructions[i].op);
            }

            break;

        case I_Store_FP:
            if (instructions[i].op >= 0) {
                fprintf(fout, "Store %d FP\n", instructions[i].op);
            } else {
                fprintf(fout, "Store (%d) FP\n", instructions[i].op);
            }

            break;

        case I_Read_GP:
            if (instructions[i].op >= 0) {
                fprintf(fout, "Read %d GP\n", instructions[i].op);
            } else {
                fprintf(fout, "Read (%d) GP\n", instructions[i].op);
            }

            break;

        case I_Read_FP:
            if (instructions[i].op >= 0) {
                fprintf(fout, "Read %d FP\n", instructions[i].op);
            } else {
                fprintf(fout, "Read (%d) FP\n", instructions[i].op);
            }

            break;

        case I_Jsr:
            fprintf(fout, "Jsr\n");
            break;

        case I_Jump:
            fprintf(fout, "Jump\n");
            break;

        case I_Jeq:
            fprintf(fout, "Jeq\n");
            break;

        case I_Jlt:
            fprintf(fout, "Jlt\n");
            break;

        case I_Loadi:
            fprintf(fout, "Loadi\n");
            break;

        case I_Pop:
            fprintf(fout, "Pop\n");
            break;

        case I_Word:
            fprintf(fout, "Word %d\n", instructions[i].op);
            break;
        }
    }
}

static int
do_translate_to_binary(struct instruction *i)
{
    switch (i->kind) {
    case I_Halt:
        return 0x0000;

    case I_Up:
        return 0x0A00;

    case I_Down:
        return 0x0C00;

    case I_Move:
        return 0x0E00;

    case I_Add:
        return 0x1000;

    case I_Sub:
        return 0x1200;

    case I_Neg:
        return 0x2200;

    case I_Mul:
        return 0x1400;

    case I_Test:
        return 0x1600;

    case I_Rts:
        return 0x2800;

    case I_Load_GP:
        return 0x0600 + two_complement(i->op);

    case I_Load_FP:
        return 0x0700 + two_complement(i->op);

    case I_Store_GP:
        return 0x0400 + two_complement(i->op);

    case I_Store_FP:
        return 0x0500 + two_complement(i->op);

    case I_Read_GP:
        return 0x0200 + two_complement(i->op);

    case I_Read_FP:
        return 0x0300 + two_complement(i->op);

    case I_Jsr:
        return 0x6800;

    case I_Jump:
        return 0x7000;

    case I_Jeq:
        return 0x7200;

    case I_Jlt:
        return 0x7400;

    case I_Loadi:
        return 0x5600;

    case I_Pop:
        return 0x5E00;

    case I_Word:
        return i->op;
    }

    assert(0);
    panic();
    return 0; // Not reachable
}

void
translate_to_binary(void)
{
    printf("Total instructions: %d\n", next_code_index);

    for (int i = 0; i < next_code_index; ++i) {
        if (fout == stdout) {
            fprintf(fout, "%d  ", i);
        }

        fprintf(fout, "%d\n", do_translate_to_binary(instructions + i));
    }
}

void
gen_Halt(void)
{
    instructions[next_code_index].kind = I_Halt;
    ++next_code_index;
}

void
gen_Up(void)
{
    instructions[next_code_index].kind = I_Up;
    ++next_code_index;
}

void
gen_Down(void)
{
    instructions[next_code_index].kind = I_Down;
    ++next_code_index;
}

void
gen_Move(void)
{
    instructions[next_code_index].kind = I_Move;
    ++next_code_index;
}

void
gen_Add(void)
{
    instructions[next_code_index].kind = I_Add;
    ++next_code_index;
}

void
gen_Sub(void)
{
    instructions[next_code_index].kind = I_Sub;
    ++next_code_index;
}

void
gen_Neg(void)
{
    instructions[next_code_index].kind = I_Neg;
    ++next_code_index;
}

void
gen_Mul(void)
{
    instructions[next_code_index].kind = I_Mul;
    ++next_code_index;
}

void
gen_Test(void)
{
    instructions[next_code_index].kind = I_Test;
    ++next_code_index;
}

void
gen_Rts(void)
{
    instructions[next_code_index].kind = I_Rts;
    ++next_code_index;
}

void
gen_Load_GP(int offset)
{
    instructions[next_code_index].kind = I_Load_GP;
    instructions[next_code_index].op = offset;
    ++next_code_index;
}

void
gen_Load_FP(int offset)
{
    instructions[next_code_index].kind = I_Load_FP;
    instructions[next_code_index].op = offset;
    ++next_code_index;
}

void
gen_Store_GP(int offset)
{
    instructions[next_code_index].kind = I_Store_GP;
    instructions[next_code_index].op = offset;
    ++next_code_index;
}

void
gen_Store_FP(int offset)
{
    instructions[next_code_index].kind = I_Store_FP;
    instructions[next_code_index].op = offset;
    ++next_code_index;
}

void
gen_Read_GP(int offset)
{
    instructions[next_code_index].kind = I_Read_GP;
    instructions[next_code_index].op = offset;
    ++next_code_index;
}

void
gen_Read_FP(int offset)
{
    instructions[next_code_index].kind = I_Read_FP;
    instructions[next_code_index].op = offset;
    ++next_code_index;
}

void
gen_Jsr(int address)
{
    instructions[next_code_index].kind = I_Jsr;
    instructions[next_code_index].op = address;
    ++next_code_index;
    instructions[next_code_index].kind = I_Word;
    instructions[next_code_index].op = address;
    ++next_code_index;
}

void
gen_Jump(int address)
{
    instructions[next_code_index].kind = I_Jump;
    instructions[next_code_index].op = address;
    ++next_code_index;
    instructions[next_code_index].kind = I_Word;
    instructions[next_code_index].op = address;
    ++next_code_index;
}

void
gen_Jeq(int address)
{
    instructions[next_code_index].kind = I_Jeq;
    instructions[next_code_index].op = address;
    ++next_code_index;
    instructions[next_code_index].kind = I_Word;
    instructions[next_code_index].op = address;
    ++next_code_index;
}

void
gen_Jlt(int address)
{
    instructions[next_code_index].kind = I_Jlt;
    instructions[next_code_index].op = address;
    ++next_code_index;
    instructions[next_code_index].kind = I_Word;
    instructions[next_code_index].op = address;
    ++next_code_index;
}

void
gen_Loadi(int v)
{
    instructions[next_code_index].kind = I_Loadi;
    instructions[next_code_index].op = v;
    ++next_code_index;
    instructions[next_code_index].kind = I_Word;
    instructions[next_code_index].op = v;
    ++next_code_index;
}

void
gen_Pop(int n)
{
    instructions[next_code_index].kind = I_Pop;
    instructions[next_code_index].op = n;
    ++next_code_index;
    instructions[next_code_index].kind = I_Word;
    instructions[next_code_index].op = n;
    ++next_code_index;
}

void
backpatch(int i, int addr)
{
    instructions[i + 1].op = addr;
}

int
get_next_code_index(void)
{
    return next_code_index;
}
