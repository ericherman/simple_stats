/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats.c stats library */
/* Copyright (C) 2014, 2016, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#include "simple_stats.h"
#include <float.h>
#include <stddef.h>
#include <math.h>
#include <assert.h>

const char *Simple_stats_version = "2.0.0";

const char *simple_stats_version(void)
{
	return Simple_stats_version;
}

void simple_stats_init(struct simple_stats *stats)
{
	assert(stats);
	stats->cnt = 0;
	stats->min = DBL_MAX;
	stats->max = -DBL_MAX;
	stats->sum = 0.0;
	stats->sum_of_squares = 0.0;
}

void simple_stats_append_val(struct simple_stats *stats, double val)
{
	assert(stats);
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
	assert(stats);
	return stats->sum / stats->cnt;
}

double simple_stats_variance(struct simple_stats *stats, int bessel_correct)
{
	double avg_sum_squared, avg_diff_sum_sq, variance;
	size_t bassel_cnt;

	assert(stats);

	/*   avoid division by zero */
	if (stats->cnt == 0) {
		return NAN;
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
	return fabs(variance);
}

double simple_stats_std_dev(struct simple_stats *stats, int bessel_correct)
{
	return sqrt(simple_stats_variance(stats, bessel_correct));
}
