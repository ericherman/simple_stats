/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ss_options.c stats library options parsing */
/* Copyright (C) 2014, 2016, 2019 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#include "ss_options.h"
#include <getopt.h>		/* if you don't hate getopt, you should */
#include <stdlib.h>

void _init_options(ss_options *options)
{
	/* initialize to stdin */
	snprintf(options->file, FILENAME_MAX, "-");
	options->help = 0;
	options->version = 0;
	options->channels = 1;
	options->skip_cols = 0;
	options->skip_rows = 0;
}

void parse_cmdline_args(ss_options *options, int argc, char *argv[])
{
	int opt_char;
	int option_index;

	/* yes, optstirng is horrible */
	const char *optstring = "hvf::c::x::y::";

	struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
		{ "file", optional_argument, 0, 'f' },
		{ "channels", optional_argument, 0, 'c' },
		{ "skip_cols", optional_argument, 0, 'x' },
		{ "skip_rows", optional_argument, 0, 'y' },
		{ 0, 0, 0, 0 }
	};

	_init_options(options);

	while (1) {
		option_index = 0;

		opt_char =
		    getopt_long(argc, argv, optstring, long_options,
				&option_index);

		/* Detect the end of the options. */
		if (opt_char == -1)
			break;

		switch (opt_char) {
		case 0:
			break;
		case 'h':	/* --help | -h */
			options->help = 1;
			break;
		case 'v':	/* --version | -v */
			options->version = 1;
			break;
		case 'f':	/* --file | -f */
			{
				/* basically, optarg is all wrong */
				snprintf(options->file, FILENAME_MAX, "%s",
					 optarg);
				break;
			}
		case 'c':	/* --channels | -opt_char */
			options->channels = atoi(optarg);
			break;
		case 'x':	/* --skip_cols | -x */
			options->skip_cols = atoi(optarg);
			break;
		case 'y':	/* --skip_rows | -y */
			options->skip_rows = atoi(optarg);
			break;
		}
	}
}

#define _safe_print(rv, written, err, end_target, out, format) \
	do { \
		rv = fprintf(out, format); \
		if (rv < 0) { \
			if (err) { \
				*err = rv; \
			} \
			goto end_target; \
		} else { \
			written += rv; \
		} \
	} while (0)

#define _safe_printf(rv, written, err, end_target, out, format, ...) \
	do { \
		rv = fprintf(out, format, __VA_ARGS__); \
		if (rv < 0) { \
			if (err) { \
				*err = rv; \
			} \
			goto end_target; \
		} else { \
			written += rv; \
		} \
	} while (0)

int print_help(const char *argv0, const char *version, FILE *out, int *error)
{
	int rv, written;
	written = 0;
	if (!out) {
		return written;
	}

	_safe_printf(rv, written, error, end_print_help, out, "%s version %s\n",
		     argv0, version);
	_safe_print(rv, written, error, end_print_help, out, "OPTIONS:\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "-f --file=name\t\tPath to input file or '-' for stdin\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "-c --channels=n\t\tNumber of columns to collect data from\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "-x --skip_cols=n\tExclude 'n' columns from data\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "\t\t\t(useful for ignoring a label column)\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "-y --skip_rows=n\tExclude 'n' rows from input data\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "\t\t\t(useful for ignoring header row)\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "-h --help\t\tThis message\n");
	_safe_print(rv, written, error, end_print_help, out,
		    "-v --version\t\tPrint version\n");

end_print_help:
	return written;
}
