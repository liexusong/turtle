#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "lexer.h"
#include "dbg.h"

FILE *out_fd;

int main(int argc, char *argv[])
{
    int c;
    char *oopt = NULL;
    out_fd = stdout;
    while ((c = getopt(argc, argv, "so:")) != -1) {
        switch (c) {
            case 's':
                printf("Output assembly code only\n");
                break;

            case 'o':
                out_fd = fopen(optarg, "w+");
                check(out_fd, "Cannot open the file %s for writing", optarg);

                printf("Output to %s\n", optarg);
                oopt = optarg;
                break;

            default:
                printf("Unknown\n");
                break;
        }
    }
    if (optind < argc) {
        do {
            FILE *f = fopen(argv[optind], "r");
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
