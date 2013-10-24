#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "lexer.h"
#include "dbg.h"
#include <unistd.h>
#include <getopt.h>
#include "global.h"

FILE           *fout;
int             sflag = 0;
int             dflag = 0;

void
panic(void)
{
    if (fout == stdout) {
        log_info("Panic!");
    } else {
        fclose(fout);
    }

    exit(1);
}

int
main(int argc, char *argv[])
{
    int             c;
    fout = stdout;

    while ((c = getopt(argc, argv, "so:d")) != -1) {
        switch (c) {
        case 's':
            printf("Output assembly code only\n");
            sflag = 1;
            break;

        case 'o':
            fout = fopen(optarg, "w+");
            check(fout, "Cannot open the file %s for writing", optarg);
            printf("Output to %s\n", optarg);
            break;

        case 'd':
            dflag = 1;
            break;

        default:
            printf("Unknown\n");
            break;
        }
    }

    if (optind < argc) {
        do {
            FILE           *f = fopen(argv[optind], "r");
            check(f, "Cannot open the file %s", argv[optind]);
            yyrestart(f);
            yyin = f;

            do {
                yyparse();
            } while (!feof(yyin));
        } while (++optind < argc);
    } else {
        yyparse();
    }

    return 0;
error:
    return 1;
}
