/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats.h stats library API */
/* Copyright (C) 2014, 2016, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#ifndef SIMPLE_STATS_H
#define SIMPLE_STATS_H 1

#ifdef __cplusplus
#define Simple_stats_begin_C_functions extern "C" {
#define Simple_stats_end_C_functions }
#else
#define Simple_stats_begin_C_functions
#define Simple_stats_end_C_functions
#endif

Simple_stats_begin_C_functions
#undef Simple_stats_begin_C_functions
/*
 * The simple_stats_std_dev() function makes use of the
 * simple_stats_sqrt function. If SIMPLE_STATS_HOSTED is defined to
 * non-zero, which is the default via __STDC_HOSTED__, then <math.h>
 * is included and the simple_stats_sqrt() function is set to
 * sqrt().  If SIMPLE_STATS_HOSTED is defined to be zero,
 * simple_stats_sqrt is set to a "DIY" implementation which follows
 * Newton's method.
 *
 * In the embedded case where a full libc is not available, it may
 * be enough to look at variance. Or, one could elect to set the
 * function pointer by hand.
 */
extern double (*simple_stats_sqrt)(double x);

/*
 * Context struct used for all functions typdef'ed as
 * "simple_stats", or use "struct simple_stats_s" simply pass in a
 * valid struct pointer, from stack or malloc/free the struct has no
 * pointers, and thus requires no need for constructors or
 * destructors, only the "simple_stats_init" function
 */
struct simple_stats {
	unsigned int cnt;
	double min;
	double max;
	double sum;
	double sum_of_squares;
};

/* functions */
/* init: caller is only required to pass in a valid struct pointer */
void simple_stats_init(struct simple_stats *stats);

void simple_stats_append_val(struct simple_stats *stats, double val);

double simple_stats_average(struct simple_stats *stats);
double simple_stats_variance(struct simple_stats *stats, int bessel_correct);
double simple_stats_std_dev(struct simple_stats *stats, int bessel_correct);

const char *simple_stats_version(void);

Simple_stats_end_C_functions
#undef Simple_stats_end_C_functions
#endif /* SIMPLE_STATS_H */
