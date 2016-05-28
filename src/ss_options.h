
/* ss_options.h - Eric Herman 2014 */
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

#endif /* SS_OPTIONS_H */
