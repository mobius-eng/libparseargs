# libparseargs

Simple command-line argument parsing library.

It follows (more or less) Unix & GNU conventions:

* An option can have a short name (e.g. `-h`) or long name (e.g. `--help`)
or both.
* Short options can be combined: `-v -h` can become `-vh`
* Long name option can have an argument specified after `=`,
e.g. `--input=file`. No spaces are allowed around `=`.
* Short name option with an argument has the following restrictions:
    - Such a short option cannot be combined with others.
    - The argument must be separated by a space, i.e. `-I/usr` is not
    allowed. Instead, `-I /usr` must be used.
* If an option requires an argument, not providing it causes an error.
* If an option does not require an argument, providing it causes an error.

## How to link

A few options available:

1. Copy-paste `parseargs.c` and `parseargs.h` into your project.
2. Compile statically linked library (done by `make all`, see Makefile)
and link against it.

The library was tested with GCC 7.5; it uses C99 standard. If C89 is
required, just define `bool`, `true`, and `false` and remove
`#include <stdbool.h>` from `parseargs.c`.

## How to use

The overall workflow is as follows:

- Define option specs: which options you expect, whether they would have
values.
- Parse command-line arguments against the specs.
- Examine the result of parsing.

The acceptable options are specified using an array of type `opt`.
Specs are created using function `make_spec`. Arguments are parsed
against specs with function `parse_args`. This function will modify
`opt` array entries for which the command line arguments are found.

A variable of type `opt` is a structure with the following fields:
- `present` will be set to `true` if option is present in the argument
list.
- `has_assign` will be set to `true` if argument value for the option is
provided.
- `value` is a string that will be filled with the value if the option
takes one.
- `spec` is an option spec filled using `make_spec`.

Function `parse_args` also outputs upto 16 (`PARSEARGS_OTHERS_MAX`)
indices values corresponding to the entries in `argv` that are not
options and that hasn't been used as values for options.

For more details, see `testparse.c`. Use `make all` to build an executable
and test the library.

## License

GPL3
