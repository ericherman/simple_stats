/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats.h stats library API */
/* Copyright (C) 2014, 2016, 2019 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#ifndef SIMPLE_STATS_H
#define SIMPLE_STATS_H

#ifdef __cplusplus
#define Simple_stats_begin_C_functions extern "C" {
#define Simple_stats_end_C_functions }
#else
#define Simple_stats_begin_C_functions
#define Simple_stats_end_C_functions
#endif

#include <stdio.h>

/*
 * Context struct used for all functions
 * typdef'ed as "simple_stats", or use "struct simple_stats_s"
 * simply pass in a valid struct pointer, from stack or malloc/free
 * the struct has no pointers, and thus requires no need for constructors
 * or destructors, only the "simple_stats_init" function
 */
struct simple_stats_s {
	unsigned int cnt;
	double min;
	double max;
	double sum;
	double sum_of_squares;
};
typedef struct simple_stats_s simple_stats;

Simple_stats_begin_C_functions
#undef Simple_stats_begin_C_functions
/* functions */
/* init: caller is only required to pass in a valid struct pointer */
void simple_stats_init(simple_stats *stats);

void simple_stats_append_val(simple_stats *stats, double val);

double simple_stats_average(simple_stats *stats);
double simple_stats_variance(simple_stats *stats, int bessel_correct);
double simple_stats_std_dev(simple_stats *stats, int bessel_correct);

simple_stats **simple_stats_from_file(const char *file_name,
				      unsigned int channels,
				      unsigned int skip_cols,
				      unsigned int skip_rows,
				      char *line_buf, size_t line_buf_len,
				      char *val_buf, size_t val_buf_len,
				      FILE *err, size_t *len);

char *simple_stats_to_string(simple_stats *stats, char *buf, size_t buflen,
			     int *written);

const char *simple_stats_version(void);

Simple_stats_end_C_functions
#undef Simple_stats_end_C_functions
#endif /* SIMPLE_STATS_H */
