/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ss_options.h stats library options structure */
/* Copyright (C) 2014, 2016, 2019 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#ifndef SS_OPTIONS_H
#define SS_OPTIONS_H

#include <stdio.h>
#include <stdint.h>

typedef struct ss_options_s {
	char file[FILENAME_MAX];
	uint8_t help;
	uint8_t version;
	unsigned int channels;
	unsigned int skip_cols;
	unsigned int skip_rows;
} ss_options;

void parse_cmdline_args(ss_options *options, int argc, char *argv[]);

int print_help(const char *argv0, const char *version, FILE *out, int *error);

#endif /* SS_OPTIONS_H */
