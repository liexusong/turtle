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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "lexer.h"
#include "dbg.h"
#include <unistd.h>
#include <getopt.h>
#include "global.h"

/**
 * Please have a look at global.h for more information
 */

FILE           *fout;
int             sflag = 0;
int             lflag = 0;

struct allocated_linked_list_memory {
    void *head;
    struct allocated_linked_list_memory *tail;
};

static struct allocated_linked_list_memory *first_record = NULL;

void
free_allocated(void)
{
    FREE_LIST_CONTENT(first_record);
    FREE_LIST(first_record);
    first_record = NULL;
}

void
record_allocated(void *head)
{
    struct allocated_linked_list_memory *list = malloc(sizeof(*list));
    check_mem(list);
    list->head = head;
    list->tail = first_record;
    first_record = list;
    return;

error:
    return;
}

/*************************
 * Starts of relevant code
 ************************/

static void
print_help(void)
{
    printf("Usage: turtle [options] [file]...\n"
           "Options:\n"
           "-o FILE\t\tFILE\n"
           "-s\t\toutput assembly code\n"
           "-l\t\tdisplay line numbers\n");
}

void
panic(void)
{
    log_err("Panic!");

    if (fout != stdout) {
        fclose(fout);
    }

    exit(1);
}

int
main(int argc, char *argv[])
{
    int             c;
    fout = stdout;

    while ((c = getopt(argc, argv, "so:l")) != -1) {
        switch (c) {
        case 's':
            debug("Output assembly code only");
            sflag = 1;
            break;

        case 'o':
            fout = fopen(optarg, "w+");
            check(fout, "Cannot open the file %s for writing", optarg);
            debug("Output to %s", optarg);
            break;

        case 'l':
            lflag = 1;
            break;

        case 'h':
        default:
            print_help();
            return 1;
        }
    }

    if (optind < argc) {
        // TODO, is there really any point to support multiple input files???
        // Can't the user just run this compiler multiple times?
        do {
            FILE           *f = fopen(argv[optind], "r");
            check(f, "Cannot open the file %s", argv[optind]);
            yyrestart(f);
            yyin = f;

            do {
                yyparse();
            } while (!feof(yyin));

            fclose(f);
        } while (++optind < argc);
    } else {
        yyparse();
    }

    return 0;
error:
    return 1;
}
