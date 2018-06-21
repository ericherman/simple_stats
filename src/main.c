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
	const char *mode = "r";

	*from_file = (strcmp("-", file_name) != 0);

	if (*from_file) {
		fp = fopen(file_name, mode);
		if (fp == NULL) {
			fprintf(stderr, "can not open '%s'\n", file_name);
			exit(-1);
		}
	} else {
		fp = stdin;
	}
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

void _run(ss_options *options)
{
	FILE *ifp;
	simple_stats stats[options->channels];
	unsigned short from_file;
	char line_buf[MAX_LINE_LEN];
	char val_buf[MAX_VALUE_LEN];
	unsigned int rows_skipped = 0;
	unsigned int cols_skipped = 0;
	unsigned int columns = options->channels + options->skip_cols;
	unsigned int channel;
	size_t i, lex_pos = 0;
	double d;

	for (i = 0; i < options->channels; i++) {
		simple_stats_init(&stats[i]);
	}

	ifp = _ifp_open(options->file, &from_file);

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
			simple_stats_append_val(&stats[channel], d);
		}
	}

	if (from_file) {
		fclose(ifp);
	}

	for (i = 0; i < options->channels; i++) {
		simple_stats_to_string(&stats[i], line_buf, MAX_LINE_LEN, NULL);
		printf("%lu: %s\n", (unsigned long)i, line_buf);
	}
}

int main(int argc, char *argv[])
{
	ss_options options;

	parse_cmdline_args(&options, argc, argv);

	_run(&options);

	return 0;
}
