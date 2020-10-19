/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats_cli_main.c stats library commandline utility */
/* Copyright (C) 2014, 2016, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simple_stats.h"
#include "simple_stats_report.h"

#include "simple_stats_options.h"

#define MAX_LINE_LEN 2000
#define MAX_VALUE_LEN MAX_LINE_LEN

void _display_stats(FILE *out, struct simple_stats **stats, size_t len,
		    char *line_buf, size_t line_buf_len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		simple_stats_to_string(stats[i], line_buf, line_buf_len, NULL);
		fprintf(out, "%lu: %s\n", (unsigned long)i, line_buf);
	}

}

int main(int argc, char *argv[])
{
	char filename_buf[FILENAME_MAX + 1];
	struct simple_stats_options options;
	struct simple_stats **stats;
	char line_buf[MAX_LINE_LEN];
	char val_buf[MAX_VALUE_LEN];
	size_t len, i;

	options.filename_buf = filename_buf;
	options.filename_buf_len = FILENAME_MAX + 1;

	simple_stats_parse_args(&options, argc, argv);

	if (options.help) {
		simple_stats_print_help(argv[0], stdout, NULL);
		return 0;
	}

	if (options.version) {
		printf("%s\n", simple_stats_version());
		return 0;
	}

	stats =
	    simple_stats_from_file(options.filename_buf, options.channels,
				   options.skip_cols, options.skip_rows,
				   line_buf, MAX_LINE_LEN, val_buf,
				   MAX_VALUE_LEN, stderr, &len);
	if (!stats) {
		return EXIT_FAILURE;
	}

	_display_stats(stdout, stats, len, line_buf, MAX_LINE_LEN);

	for (i = len; i > 0; --i) {
		free(stats[i - 1]);
		stats[i - 1] = NULL;
	}
	free(stats);
	stats = NULL;

	return EXIT_SUCCESS;
}
