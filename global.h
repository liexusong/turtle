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

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "symbol.h"
#include "dbg.h"
#include "parser.h"

void lyyerror(YYLTYPE t, char *s, ...);

/**
 * Panic
 *
 * Closes the output file if there is any and exits the problem
 */
void panic(void);

extern int sflag; // -S flag
extern int lflag; // -d flag
extern FILE *fout; // stderr or an output file

/**
 * The following functions are used to reduce the amount of memory leak. They
 * are not really related to this assignment.
 */
#define FREE_LIST_CONTENT(list) do { \
                                    typeof(list) t; \
                                    while (list != NULL) { \
                                        t = list->tail; \
                                        free(list->head); \
                                        free(list); \
                                        list = t; \
                                    } \
                                } while (0)


#define FREE_LIST(list) do { \
                            typeof(list) t; \
                            while (list != NULL) { \
                                t = list->tail; \
                                free(list); \
                                list = t; \
                            } \
                        } while (0)

void record_allocated(void *head);
void free_allocated(void);

#endif /* end of include guard: GLOBAL_H_ */

