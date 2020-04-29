/* parseargs.h
 * Header file for libparseargs library
 *
 * Copyright (C) 2020 Alexey V. Cherkaev
 * LICENSE: GPL3
 */
#ifndef __PARSEARGS_H__
#define __PARSEARGS_H__


#define PARSEARGS_LONG_NAME_MAX_LENGTH  28
#define PARSEARGS_VALUE_MAX_LENGTH      94
#define PARSEARGS_OTHERS_MAX            16

#define PARSEARGS_SUCCESS               0
#define PARSEARGS_TOO_MANY_ARGUMENTS    -1
#define PARSEARGS_UNKOWN_OPTION_TYPE    -2
#define PARSEARGS_NO_ASSIGN_VALUE       -3
#define PARSEARGS_UNKNOWN_OPTION        -4
#define PARSEARGS_NO_ASSIGN_EXPECTED    -5


typedef struct opt_spec_tag {
    char short_name;
    char long_name[PARSEARGS_LONG_NAME_MAX_LENGTH];
    bool has_long_name;
    bool has_short_name;
    bool has_assign;
} opt_spec;

typedef struct opt_tag {
    opt_spec spec;
    bool present;
    bool has_assign;
    char value[PARSEARGS_VALUE_MAX_LENGTH];
} opt;


typedef int others_ind[PARSEARGS_OTHERS_MAX];

void make_spec(opt_spec *spec, char short_name, char * long_name,
        bool has_short_name, bool has_long_name, bool has_assign);

int parse_args(int argc, char **argv, int nopt, opt *options, others_ind ind);



#endif
