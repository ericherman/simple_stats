/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats.c stats library */
/* Copyright (C) 2014, 2016, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#include "simple_stats.h"

#include <float.h>
#include <stddef.h>

#ifndef SIMPLE_STATS_HOSTED
#ifdef ARDUINO
#define SIMPLE_STATS_HOSTED 0
#endif
#endif

#ifndef SIMPLE_STATS_HOSTED
#ifdef __STDC_HOSTED__
#define SIMPLE_STATS_HOSTED __STDC_HOSTED__
#endif
#endif

#if SIMPLE_STATS_HOSTED
#include <math.h>
#endif

#ifndef SIMPLE_STATS_NAN
#ifdef NAN
#define SIMPLE_STATS_NAN NAN
#else
#define SIMPLE_STATS_NAN (0.f / 0.f)
#endif
#endif

double simple_stats_sqrt_newton(double x);

#if SIMPLE_STATS_HOSTED
double (*simple_stats_sqrt)(double x) = sqrt;
#else
double simple_stats_sqrt_newton(double x);
double (*simple_stats_sqrt)(double x) = simple_stats_sqrt_newton;
#endif

const char *Simple_stats_version = "2.0.0";

const char *simple_stats_version(void)
{
	return Simple_stats_version;
}

void simple_stats_init(struct simple_stats *stats)
{
	stats->cnt = 0;
	stats->min = DBL_MAX;
	stats->max = -DBL_MAX;
	stats->sum = 0.0;
	stats->sum_of_squares = 0.0;
}

void simple_stats_append_val(struct simple_stats *stats, double val)
{
	stats->cnt++;
	if (stats->min > val) {
		stats->min = val;
	}
	if (stats->max < val) {
		stats->max = val;
	}
	stats->sum += val;
	stats->sum_of_squares += (val * val);
}

double simple_stats_average(struct simple_stats *stats)
{
	return stats->sum / stats->cnt;
}

static double simple_stats_fabs(double x)
{
	if (x < 0.0) {
		return -x;
	}
	return x;
}

double simple_stats_variance(struct simple_stats *stats, int bessel_correct)
{
	double avg_sum_squared, avg_diff_sum_sq, variance;
	size_t bassel_cnt;

	/*   avoid division by zero */
	if (stats->cnt == 0) {
		return SIMPLE_STATS_NAN;
	}
	if (stats->cnt == 1) {
		return 0.0;
	}

	/* https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance */

	/*  Because SumSq and (Sum * Sum)/n can be very similar
	 *  numbers, cancellation can lead to the precision of the result to
	 *  be much less than the inherent precision of the floating-point
	 *  arithmetic used to perform the computation. Thus this algorithm
	 *  should not be used in practice. */

	bassel_cnt = bessel_correct ? (stats->cnt - 1) : stats->cnt;
	avg_sum_squared = (stats->sum * stats->sum) / stats->cnt;
	avg_diff_sum_sq = stats->sum_of_squares - avg_sum_squared;
	variance = avg_diff_sum_sq / bassel_cnt;
	return simple_stats_fabs(variance);
}

double simple_stats_std_dev(struct simple_stats *stats, int bessel_correct)
{
	return simple_stats_sqrt(simple_stats_variance(stats, bessel_correct));
}

double simple_stats_sqrt_newton(double x)
{
	size_t max_iteratons = 50;
	size_t i = 0;
	double divisor = x;
	double guess = 0.0;
	double last_guess = 0.0;
	double distance = 0.0;
	double our_epsilon = x * DBL_EPSILON;

	/* if negative or NaN */
	if (!(x >= 0.0)) {
		SIMPLE_STATS_NAN;
	}

	/* INFINITY */
	if ((x / 2) == x) {
		return x;
	}

	if (x == 0.0) {
		return x;
	}

	if (x > 0.0) {
		our_epsilon = (x / 4.0) * DBL_EPSILON;
	} else {
		our_epsilon = (x * 4.0) * DBL_EPSILON;
	}

	if (x < 0.0) {
		guess = (x * 2.0);
	} else {
		guess = (x / 2.0);
	}

	for (i = 0; i < max_iteratons; ++i) {
		last_guess = guess;
		guess = (guess + divisor / guess) * 0.5;
		distance = simple_stats_fabs(guess - last_guess);
		if (distance < our_epsilon) {
			return guess;
		}
	}
	return guess;
}
