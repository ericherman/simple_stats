/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats_options.h stats library options structure */
/* Copyright (C) 2014, 2016, 2019 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#ifndef SIMPLE_STATS_OPTIONS_H
#define SIMPLE_STATS_OPTIONS_H

#include <stdio.h>
#include <stdint.h>

typedef struct simple_stats_options_s {
	char file[FILENAME_MAX];
	uint8_t help;
	uint8_t version;
	unsigned int channels;
	unsigned int skip_cols;
	unsigned int skip_rows;
} simple_stats_options;

void parse_cmdline_args(simple_stats_options *options, int argc, char *argv[]);

int print_help(const char *argv0, const char *version, FILE *out, int *error);

#endif /* SIMPLE_STATS_OPTIONS_H */
