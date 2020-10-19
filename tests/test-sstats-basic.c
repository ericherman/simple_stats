/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-sstats-basic.c */
/* Copyright (C) 2018, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#include "simple_stats.h"
#include "echeck.h"

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

void echeck_log_append_simple_stats(struct echeck_log *log,
				    struct simple_stats *stats)
{
	int bessel_correct = 1;

	log->append_s(log, "{ cnt: ");
	log->append_ul(log, stats->cnt);
	log->append_s(log, ", min: ");
	log->append_f(log, stats->min);
	log->append_s(log, ", max: ");
	log->append_f(log, stats->max);
	log->append_s(log, ", avg: ");
	log->append_f(log, simple_stats_average(stats));
	log->append_s(log, ", avg: ");
	log->append_f(log, simple_stats_std_dev(stats, bessel_correct));
	log->append_s(log, " }");
}

int check_stats(double *samples, size_t sample_len, int bessel_correct,
		double expect_min, double expect_max, double expect_mean,
		double expect_variance, double expect_stddev)
{
	struct echeck_log *log = echeck_default_log;
	size_t i;
	int errs;
	struct simple_stats stats;
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
		log->append_ul(log, errs);
		log->append_s(log, " errors found with: ");
		log->append_eol(log);
		echeck_log_append_simple_stats(log, &stats);
		log->append_eol(log);
	}

	return errs;
}

/* this test is a "friend" in the C++ sense of the word */
extern double simple_stats_sqrt_newton(double x);

#define check_simple_stats_sqrt_newton_to_known(x, expected, epsilon) \
	check_double(simple_stats_sqrt_newton(x), expected, epsilon)

#define square_and_check(root, epsilon) \
	check_simple_stats_sqrt_newton_to_known((root * root), root, epsilon)

int test_simple_stats_sqrt_newton(void)
{
	int errs = 0;
	double d1 = 1.0;
	double d2 = 2.0;
	double d3 = 3.0;
	double sqrt_2 = 1.41421356237309504880;
	double sqrt_3 = 1.73205080756887719000;
	double epsilon = 0.00001;
	double epsilon2 = 0.00001;

	errs += check_double((sqrt_2 * sqrt_2), 2.0, epsilon);
	errs += check_double((sqrt_3 * sqrt_3), 3.0, epsilon);

	errs += check_simple_stats_sqrt_newton_to_known(d1, d1, epsilon);
	errs += check_simple_stats_sqrt_newton_to_known(d2, sqrt_2, epsilon);
	errs += check_simple_stats_sqrt_newton_to_known(d3, sqrt_3, epsilon);
	errs += check_simple_stats_sqrt_newton_to_known(100.0, 10.0, epsilon);
	errs += check_simple_stats_sqrt_newton_to_known(1024.0, 32.0, epsilon);

	for (int i = 0; i < 10; ++i) {
		d1 = d1 * 2.0;
		errs += square_and_check(d1, epsilon);
		errs += square_and_check(1.0 / d1, epsilon2);

		d2 = d2 * 10;
		errs += square_and_check(d2, epsilon);
		errs += square_and_check(1.0 / d2, epsilon2);

		d3 = d3 + d2 + d1;
		errs += square_and_check(d3, epsilon);
		errs += square_and_check(1.0 / d3, epsilon2);

		epsilon = (epsilon * 10);
		epsilon2 = (epsilon / 10);
	}
	return errs;
}

int test_sstats_basic(void)
{
	int errs = 0;
	double samples[] = { 10.0, 8.0, 10.0, 8.0, 8.0, 4.0 };
	size_t sample_len = 6;
	double expect_min = 4.0;
	double expect_max = 10.0;
	double expect_mean = 8.0;

	int bessel_correct = 1;
	double expect_variance = 4.8;
	double epsilon = 0.000001;
	double expect_stddev = 2.19089023002066445382;

	/* sanity check that our expect_stddev is the sqrt(expect_variance) */
	errs +=
	    check_double((expect_stddev * expect_stddev), expect_variance,
			 epsilon);

	errs +=
	    check_stats(samples, sample_len, bessel_correct, expect_min,
			expect_max, expect_mean, expect_variance,
			expect_stddev);

	bessel_correct = 0;
	expect_variance = 4;
	expect_stddev = 2;
	errs +=
	    check_stats(samples, sample_len, bessel_correct, expect_min,
			expect_max, expect_mean, expect_variance,
			expect_stddev);

	errs += test_simple_stats_sqrt_newton();

	return errs;
}

#if SIMPLE_STATS_HOSTED
int main(void)
{
	int errs = test_sstats_basic();
	return errs ? 1 : 0;
}
#endif
