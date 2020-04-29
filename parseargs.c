/* parsearg.c
 * Implementation file of libparseargs library
 *
 * Copyright (C) 2020 Alexey V. Cherkaev
 * LICENSE: GPL3
 */
#include <string.h>
#include <stdbool.h>
#include "parseargs.h"

typedef enum arg_type_enum {
    SINGLE_SHORT, COMBINED, LONG_OPTION, LONG_ASSIGN, OTHER
} arg_type;

static arg_type get_arg_type(char **argv, int iarg);
static int process_single_short_option(int argc,
        char **argv, int nopt, opt *options, int *iarg);
static int process_single_short_option_noarg(char arg, int nopt, opt *options);
static int process_combined_options(char *arg, int nopt, opt *options);
static int process_long_option(char *option_name, int nopt, opt *options);
static int process_long_assign_option(char *option_name, int nopt, opt *options);
static bool single_letter(char *str);
static bool multiple_letters(char *str);
static bool has_assignment_and_proper_name(char *str);
static bool proper_name(char *str);
static int find_short_option(char c, int nopt, opt *options);
static int find_long_option(char *option_name, int nopt, opt *options);
static bool long_name_cmp(char* true_opt_name, char* test_opt_name);
static char* get_assign_value(char* str);
static bool alpha(char c);
static bool alphanum(char c);



void make_spec(opt_spec *spec, char short_name, char * long_name,
        bool has_short_name, bool has_long_name, bool has_assign)
{
    if (has_short_name) {
        spec->short_name = short_name;
    }
    else {
        spec->short_name = 0x0;
    }
    if (has_long_name) {
        strncpy(spec->long_name, long_name, PARSEARGS_LONG_NAME_MAX_LENGTH);
    }
    spec->has_short_name = has_short_name;
    spec->has_long_name = has_long_name;
    spec->has_assign = has_assign;
}


int parse_args(int argc, char **argv, int nopt, opt *options, others_ind ind) {
    int iarg, iopt, i, result;
    /* Initialize */
    iarg = 1;
    for (i = 0; i < PARSEARGS_OTHERS_MAX; i++) {
        ind[i] = -1;
    }
    for (i = 0; i < nopt; i++) {
        options[i].present = false;
        options[i].has_assign = false;
    }
    /* Use i as an index of current OTHER */
    i = 0;
    while (iarg < argc) {
        result = PARSEARGS_UNKOWN_OPTION_TYPE;
        switch(get_arg_type(argv, iarg)) {
            case SINGLE_SHORT:
                result = process_single_short_option(argc, argv, nopt, options, &iarg);
                break;
            case COMBINED:
                result = process_combined_options(&argv[iarg][1], nopt, options);
                break;
            case LONG_OPTION:
                result = process_long_option(&argv[iarg][2], nopt, options);
                break;
            case LONG_ASSIGN:
                result = process_long_assign_option(&argv[iarg][2], nopt, options);
                break;
            case OTHER:
                if (i < PARSEARGS_OTHERS_MAX) {
                    ind[i] = iarg;
                    i++;
                    result = PARSEARGS_SUCCESS;
                }
                else {
                    result = PARSEARGS_TOO_MANY_ARGUMENTS;
                }
                break;
        }
        if (result != PARSEARGS_SUCCESS) break;
        iarg++;
    }
    return result;
}


arg_type get_arg_type(char **argv, int iarg)
{

    if (argv[iarg][0] == '-' && single_letter(&argv[iarg][1])) {
        return SINGLE_SHORT;
    }
    else if (argv[iarg][0] == '-' && multiple_letters(&argv[iarg][1])) {
        return COMBINED;
    }
    else if (argv[iarg][0] == '-' && argv[iarg][1] == '-') {
        if (has_assignment_and_proper_name(&argv[iarg][2])) {
            return LONG_ASSIGN;
        }
        else if (proper_name(&argv[iarg][2])) {
            return LONG_OPTION;
        }
        else {
            return OTHER;
        }
    }
    else {
        return OTHER;
    }
}



int process_single_short_option(int argc, char **argv, int nopt, opt *options, int *iarg)
{
    int iopt;
    iopt = find_short_option(argv[*iarg][1], nopt, options);
    if (iopt >= 0) {
        options[iopt].present = true;
        if (options[iopt].spec.has_assign) {
            if ((*iarg)+1 < argc) {
                strncpy(options[iopt].value, argv[(*iarg)+1],
                        PARSEARGS_VALUE_MAX_LENGTH);
                options[iopt].has_assign = true;
                (*iarg)++;
                return PARSEARGS_SUCCESS;
            }
            else {
               return PARSEARGS_NO_ASSIGN_VALUE;
            }
        }
        return PARSEARGS_SUCCESS;
    }
    else {
        return PARSEARGS_UNKNOWN_OPTION;
    }
}


int process_single_short_option_noarg(char arg, int nopt, opt *options)
{
    int iopt;
    iopt = find_short_option(arg, nopt, options);
    if (iopt >= 0) {
        options[iopt].present = true;
        if (options[iopt].spec.has_assign) {
            return PARSEARGS_NO_ASSIGN_VALUE;
        }
        return PARSEARGS_SUCCESS;
    }
    else {
        return PARSEARGS_UNKNOWN_OPTION;
    }
}


int process_combined_options(char *arg, int nopt, opt *options)
{
    int iarg, result;
    char copt;

    iarg = 0;

    while (arg[iarg] != 0x0) {
        result = process_single_short_option_noarg(arg[iarg], nopt, options);
        if (result != PARSEARGS_SUCCESS) return result;
        iarg++;
    }
    return PARSEARGS_SUCCESS;
}


int process_long_option(char *option_name, int nopt, opt *options)
{
    int iopt;
    iopt = find_long_option(option_name, nopt, options);
    if (iopt >= 0) {
        options[iopt].present = true;
        if (options[iopt].spec.has_assign) {
            return PARSEARGS_NO_ASSIGN_VALUE;
        }
        else {
            return PARSEARGS_SUCCESS;
        }
    }
    else {
        return PARSEARGS_UNKNOWN_OPTION;
    }
}


int process_long_assign_option(char *option_name, int nopt, opt *options)
{
    int iopt;
    char *val;
    iopt = find_long_option(option_name, nopt, options);
    if (iopt >= 0) {
        options[iopt].present = true;
        if (options[iopt].spec.has_assign) {
            val = get_assign_value(option_name);
            strncpy(options[iopt].value, val, PARSEARGS_VALUE_MAX_LENGTH);
            options[iopt].has_assign = true;
            return PARSEARGS_SUCCESS;
        }
        else {
            return PARSEARGS_NO_ASSIGN_EXPECTED;
        }
    }
    else {
        return PARSEARGS_UNKNOWN_OPTION;
    }
}


bool single_letter(char *str)
{
    return ((str[0] >= 'a' && str[0] < 'z') ||
        (str[0] >= 'A' && str[0] < 'Z')) &&
        str[1] == 0x0;
}


bool multiple_letters(char *str)
{
    int i = 0;
    if (str[i] == 0x0) return false;
    while (str[i] != 0x0) {
        if (!((str[i] >= 'a' && str[i] < 'z') ||
            (str[i] >= 'A' && str[i] < 'Z'))) return false;
        i++;
    }
    return true;
}

bool has_assignment_and_proper_name(char *str)
{
    int i = 0;
    if (str[i] == 0x0 || !alpha(str[i])) return false;
    while (str[i] != 0x0) {
        if (!(alphanum(str[i]) || str[i] != '-' || str[i] != '_'))
            return false;
        if (str[i+1] == '=') return true;
        i++;
    }
    return false;
}


bool proper_name(char *str)
{
    int i = 0;
    if (str[i] == 0x0 || !alpha(str[i])) return false;
    while (str[i] != 0x0) {
        if (!(alphanum(str[i]) || str[i] != '-' || str[i] != '_'))
            return false;
        i++;
    }
    return true;
}



int find_short_option(char c, int nopt, opt *options)
{
    int i;
    for (i = 0 ; i < nopt; i++) {
        if (options[i].spec.short_name == c) return i;
    }
    return -1;
}


int find_long_option(char *option_name, int nopt, opt *options)
{
    int i;
    for (i = 0; i < nopt; i++) {
        if (long_name_cmp(options[i].spec.long_name, option_name)) return i;
    }
    return -1;
}

bool long_name_cmp(char* true_opt_name, char* test_opt_name)
{
    int i;
    i = 0;
    while (true_opt_name[i] != 0x0 && test_opt_name[i] != 0x0) {
        if (true_opt_name[i] != test_opt_name[i]) return false;
        i++;
    }
    if (true_opt_name[i] == 0x0 &&
            (test_opt_name[i] == 0x0 || test_opt_name[i] == '=')) {
        return true;
    }
    else {
        return false;
    }
}


char* get_assign_value(char* str)
{
    int i = 0;
    while (str[i] != '=' && str[i] != 0x0) i++;
    if (str[i] == '=') return &str[i+1];
    return NULL;
}


bool alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool alphanum(char c)
{
    return alpha(c) || (c>='0' && c<='9');
}

