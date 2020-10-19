/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-sstats-basic.c */
/* Copyright (C) 2018, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#include <stdio.h>
#include "simple_stats.h"
#include "simple_stats.c"
#include "simple_stats_report.c"
#include "echeck.h"
#include "echeck.c"

int check_stats(double *samples, size_t sample_len, int bessel_correct,
		double expect_min, double expect_max, double expect_mean,
		double expect_variance, double expect_stddev)
{
	size_t i;
	int errs;
	struct simple_stats stats;
	char buf[80];
	double mean, variance, stddev;

	simple_stats_init(&stats);

	for (i = 0; i < sample_len; ++i) {
		simple_stats_append_val(&stats, samples[i]);
	}
	errs = 0;
	errs += check_double_scaled_epsilon(stats.min, expect_min);
	errs += check_double_scaled_epsilon(stats.max, expect_max);

	mean = simple_stats_average(&stats);
	errs += check_double_scaled_epsilon(mean, expect_mean);

	variance = simple_stats_variance(&stats, bessel_correct);
	errs += check_double_scaled_epsilon(variance, expect_variance);

	stddev = simple_stats_std_dev(&stats, bessel_correct);
	errs += check_double_scaled_epsilon(stddev, expect_stddev);

	if (errs) {
		fprintf(stderr, "%s\n",
			simple_stats_to_string(&stats, buf, 80, NULL));
	}

	return errs;
}

int compare_simple_stats_sqrt_newton_to_libc(double x)
{
	double s = sqrt(x);
	double n = simple_stats_sqrt_newton(x);
	double t = 2 * s * DBL_EPSILON;
	double d = fabs(s - n);
	int err = d < t ? 0 : 1;

	if (err) {
		printf("simple_stats_sqrt_newton(%g) = %g\n", x, s);
		printf("                    sqrt(%g) = %g\n", x, n);
		printf("                           t = %g\n", t);
		printf("                           d = %g\n", t);
		printf("                close enough = %s\n",
		       err ? "no" : "yes");
		printf("\n");
	}

	return err;
}

int test_simple_stats_sqrt_newton(void)
{
	int err = 0;
	double d1 = 1.0;
	double d2 = 1.0;
	double d3 = 3.0;
	for (int i = 0; i < 20; ++i) {
		d1 = d1 * 2.0;
		err += compare_simple_stats_sqrt_newton_to_libc(d1);
		err += compare_simple_stats_sqrt_newton_to_libc(1.0 / d1);

		d2 = d2 * 10;
		err += compare_simple_stats_sqrt_newton_to_libc(d2);
		err += compare_simple_stats_sqrt_newton_to_libc(1.0 / d2);

		d3 = d3 + d2 + d1;
		err += compare_simple_stats_sqrt_newton_to_libc(d3);
		err += compare_simple_stats_sqrt_newton_to_libc(1.0 / d3);
	}
	return err;
}

int main(void)
{
	int errs = 0;
	double samples[] = { 10.0, 8.0, 10.0, 8.0, 8.0, 4.0 };
	size_t sample_len = 6;
	double expect_min = 4.0;
	double expect_max = 10.0;
	double expect_mean = 8.0;

	int bessel_correct = 1;
	double expect_variance = 4.8;
	double expect_stddev = sqrt(expect_variance);

	errs +=
	    check_stats(samples, sample_len, bessel_correct, expect_min,
			expect_max, expect_mean, expect_variance,
			expect_stddev);

	bessel_correct = 0;
	expect_variance = 4;
	expect_stddev = sqrt(expect_variance);
	errs +=
	    check_stats(samples, sample_len, bessel_correct, expect_min,
			expect_max, expect_mean, expect_variance,
			expect_stddev);

	errs += test_simple_stats_sqrt_newton();

	return errs ? EXIT_FAILURE : EXIT_SUCCESS;
}
