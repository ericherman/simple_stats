/* ss_options.h stats library options structure
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
