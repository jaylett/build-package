## Process this file with automake to produce Makefile.in

## --- Options ---

AUTOMAKE_OPTIONS = foreign

## --- What to build ---

bin_PROGRAMS = build-package

## --- Things to install ---

## --- How to build the program itself ---

CFLAGS = @LIBCOOPT_CFLAGS@

build_package_SOURCES = main.c memory.c module.c option.c paths.c step.c
build_package_LDADD = @LIBCOOPT_LIBS@
