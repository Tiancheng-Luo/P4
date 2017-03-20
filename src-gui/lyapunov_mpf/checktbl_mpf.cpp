#include "lyapunov_mpf.h"
#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------------------
//                      CHECK_SUM
// --------------------------------------------------------------------------
//
// check if the table exist

void check_sum(char *s, int k)
{
    FILE *fp;

    if (env_windows) {
        if (win_sumtablepath == nullptr)
            sprintf(s, "sum%d.tab", k);
        else
            sprintf(s, "%ssum%d.tab", win_sumtablepath, k);
    } else {
        if (getenv("P4_DIR") == nullptr)
            sprintf(s, "sum%d.tab", k);
        else
            sprintf(s, "%s/sum_tables/sum%d.tab", getenv("P4_DIR"), k);
    }

    fp = fopen(s, "r");
    if (fp == nullptr) {
        printf("table sum%d.tab does not exist\n", k);
        printf("creating the table\n");
        printf("please wait \n");
        create_sum(k);
    } else {
        printf("table sum%d.tab exist\n", k);
        fclose(fp);
    }
}
