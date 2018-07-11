/* main.c stats library commandline utility
   Copyright (C) 2014, 2016 Eric Herman <eric@freesa.org>

   This work is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This work is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public
   License (COPYING) and the GNU General Public License (COPYING.GPL3).
   If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simple_stats.h"

#include "ss_options.h"

#define MAX_LINE_LEN 2000
#define MAX_VALUE_LEN MAX_LINE_LEN

FILE *_ifp_open(const char *file_name, unsigned short *from_file)
{
	FILE *fp;
	unsigned short _from_file;
	const char *mode = "r";

	_from_file = (strcmp("-", file_name) != 0);
	if (from_file) {
		*from_file = _from_file;
	}

	fp = (_from_file) ? fopen(file_name, mode) : stdin;

	return fp;
}

void _lex_col_val(char *line_buf, size_t *lex_pos, char *val_buf)
{
	size_t len = 0;
	unsigned char done = 0;
	char *c;

	while (!done) {
		c = (line_buf + *lex_pos + len++);
		switch (*c) {
		case '\0':
		case '\n':
		case ',':
			{
				done = 1;
			}
		default:
			break;
		}
	}
	strncpy(val_buf, line_buf + *lex_pos, len);
	val_buf[(!done || !len) ? len : len - 1] = '\0';
	*lex_pos += len;
}

simple_stats **_run(ss_options *options)
{
	FILE *ifp;
	simple_stats **stats;
	unsigned short from_file;
	char line_buf[MAX_LINE_LEN];
	char val_buf[MAX_VALUE_LEN];
	unsigned int rows_skipped = 0;
	unsigned int cols_skipped = 0;
	unsigned int columns = options->channels + options->skip_cols;
	unsigned int channel;
	size_t i, lex_pos = 0;
	double d;

	stats = malloc(sizeof(simple_stats *) * options->channels);
	if (!stats) {
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < options->channels; i++) {
		stats[i] = malloc(sizeof(simple_stats));
		if (!stats[i]) {
			exit(EXIT_FAILURE);
		}
		simple_stats_init(stats[i]);
	}

	ifp = _ifp_open(options->file, &from_file);
	if (ifp == NULL) {
		fprintf(stderr, "can not open '%s'\n", options->file);
		exit(-1);
	}

	while (fgets(line_buf, MAX_LINE_LEN, ifp) != NULL) {
		if (rows_skipped++ < options->skip_rows) {
			continue;
		}
		cols_skipped = 0;
		lex_pos = 0;
		for (i = 0; i < columns; i++) {
			_lex_col_val(line_buf, &lex_pos, val_buf);
			if (cols_skipped++ < options->skip_cols) {
				continue;
			}
			channel = i - options->skip_cols;
			sscanf(val_buf, "%lf%*s", &d);
			simple_stats_append_val(stats[channel], d);
		}
	}

	if (from_file) {
		fclose(ifp);
	}
	return stats;
}

void _display_stats(simple_stats **stats, size_t len)
{
	size_t i;
	char line_buf[MAX_LINE_LEN];

	for (i = 0; i < len; i++) {
		simple_stats_to_string(stats[i], line_buf, MAX_LINE_LEN, NULL);
		fprintf(stdout, "%lu: %s\n", (unsigned long)i, line_buf);
	}

}

int main(int argc, char *argv[])
{
	ss_options options;
	simple_stats **stats;
	size_t i;

	parse_cmdline_args(&options, argc, argv);

	if (options.help) {
		print_help(argv[0], simple_stats_version(), stdout, NULL);
		return 0;
	}

	if (options.version) {
		printf("%s\n", simple_stats_version());
		return 0;
	}

	stats = _run(&options);

	_display_stats(stats, options.channels);

	for (i = options.channels; i > 0; --i) {
		free(stats[i - 1]);
		stats[i - 1] = NULL;
	}
	free(stats);
	stats = NULL;

	return 0;
}
