/* testparse.c
 * Test/demo program for libparseargs library
 *
 * Copyright (C) 2020 Alexey V. Cherkaev
 * LICENSE: GPL3
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parseargs.h"

static void print_spec(opt_spec *spec);
static void print_option(opt *option);

int main(int argc, char **argv)
{
    opt options[4];
    others_ind ind;
    int result, i;
    make_spec(&options[0].spec, 'h', "help", true, true, false);
    make_spec(&options[1].spec, 'i', "info", true, true, false);
    make_spec(&options[2].spec, 'I', "input", true, true, true);
    make_spec(&options[3].spec, 'v', "", true, false, false);
    result = parse_args(argc, argv, 4, options, ind);
    if (result != PARSEARGS_SUCCESS) {
        printf("Unsuccessfull options parsing: %d\n", result);
        return -1;
    }
    for (i = 0; i < 4; i++) {
        print_spec(&options[i].spec);
        print_option(&options[i]);
    }
    printf("Other arguments:\n");
    for (i = 0; i < PARSEARGS_OTHERS_MAX; i++) {
        if (ind[i] >= 0) {
            printf("\t%s\n", argv[ind[i]]);
        }
    }
    return 0;
}

void print_spec(opt_spec *spec)
{
    printf("[");
    if (spec->has_short_name) {
        printf("-%c ", spec->short_name);
    }
    if (spec->has_long_name) {
        printf("--%s", spec->long_name);
    }
    if (spec->has_assign) {
        printf("=<VAL>");
    }
    printf("]\n");
}

void print_option(opt *option)
{
    if (!option->present) {
        printf("\tNOT PRESENT\n");
        return;
    }
    printf("\tPRESENT");
    if (option->spec.has_assign) {
        printf(" WITH VALUE: %s", option->value);
    }
    printf("\n");
}
/*
 * -hi --input=SOMEINPUT -v Hello
 * */
