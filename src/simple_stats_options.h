/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats_options.h stats library options structure */
/* Copyright (C) 2014, 2016, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#ifndef SIMPLE_STATS_OPTIONS_H
#define SIMPLE_STATS_OPTIONS_H 1

#include <stdio.h>
#include <stdint.h>

struct simple_stats_options {
	char *filename_buf;
	size_t filename_buf_len;
	uint8_t help;
	uint8_t version;
	unsigned int channels;
	unsigned int skip_cols;
	unsigned int skip_rows;
};

void simple_stats_parse_args(struct simple_stats_options *options, int argc,
			     char *argv[]);

int simple_stats_print_help(const char *argv0, FILE *out, int *error);

#endif /* SIMPLE_STATS_OPTIONS_H */
