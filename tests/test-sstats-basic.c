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

void eembed_log_append_simple_stats(struct eembed_log *log,
				    struct simple_stats *stats,
				    int bessel_correct)
{
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
		double expect_variance, double expect_stddev, int verbose)
{
	struct eembed_log *log = eembed_err_log;
	size_t i;
	int errs;
	struct simple_stats stats;
	double mean, variance, stddev;

	if (verbose) {
		log->append_s(log, "simple_stats_init(&stats)");
		log->append_eol(log);
	}
	simple_stats_init(&stats);

	for (i = 0; i < sample_len; ++i) {
		if (verbose) {
			log->append_s(log, "simple_stats_append_val(");
			log->append_f(log, samples[i]);
			log->append_s(log, ")");
			log->append_eol(log);
		}
		simple_stats_append_val(&stats, samples[i]);
	}
	errs = 0;
	if (verbose) {
		log->append_s(log, "check_double_scaled_epsilon(stats.min, ");
		log->append_f(log, expect_min);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	errs += check_double_scaled_epsilon(stats.min, expect_min);

	if (verbose) {
		log->append_s(log, "check_double_scaled_epsilon(stats.max, ");
		log->append_f(log, expect_max);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	errs += check_double_scaled_epsilon(stats.max, expect_max);

	if (verbose) {
		log->append_s(log, "check_double_scaled_epsilon(");
		log->append_s(log, "simple_stats_average(&stats), ");
		log->append_f(log, expect_mean);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	mean = simple_stats_average(&stats);
	errs += check_double_scaled_epsilon(mean, expect_mean);

	if (verbose) {
		log->append_s(log, "bessel_correct == ");
		log->append_ul(log, bessel_correct);
		log->append_eol(log);
		log->append_s(log, "check_double_scaled_epsilon(");
		log->append_s(log, "simple_stats_variance(&stats, ");
		log->append_ul(log, bessel_correct);
		log->append_s(log, "), ");
		log->append_f(log, expect_variance);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	variance = simple_stats_variance(&stats, bessel_correct);
	errs += check_double_scaled_epsilon(variance, expect_variance);

	if (verbose) {
		log->append_s(log, "check_double_scaled_epsilon(");
		log->append_s(log, "simple_stats_std_dev(&stats, ");
		log->append_ul(log, bessel_correct);
		log->append_s(log, "), ");
		log->append_f(log, expect_stddev);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	stddev = simple_stats_std_dev(&stats, bessel_correct);
	errs += check_double_scaled_epsilon(stddev, expect_stddev);

	if (errs) {
		log->append_ul(log, errs);
		log->append_s(log, " errors found with: ");
		log->append_eol(log);
		eembed_log_append_simple_stats(log, &stats, bessel_correct);
		log->append_eol(log);
	}

	if (verbose) {
		log->append_eol(log);
	}
	return errs;
}

/* this test is a "friend" in the C++ sense of the word */
extern double simple_stats_sqrt_newton(double x);

int test_simple_stats_sqrt_newton(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int errs = 0;
	double d1 = 1.0;
	double d2 = 2.0;
	double d3 = 3.0;
	double sqrt_2 = 1.41421356237309504880;
	double sqrt_3 = 1.73205080756887719000;
	double epsilon = 0.00001;
	double epsilon2 = 0.00001;

	if (verbose) {
		log->append_s(log, "test_simple_stats_sqrt_newton");
		log->append_eol(log);
	}

	errs += check_double((sqrt_2 * sqrt_2), 2.0, epsilon);
	errs += check_double((sqrt_3 * sqrt_3), 3.0, epsilon);

	errs += check_double(simple_stats_sqrt_newton(d1), d1, epsilon);
	errs += check_double(simple_stats_sqrt_newton(d2), sqrt_2, epsilon);
	errs += check_double(simple_stats_sqrt_newton(d3), sqrt_3, epsilon);
	errs += check_double(simple_stats_sqrt_newton(100.0), 10.0, epsilon);
	errs += check_double(simple_stats_sqrt_newton(1024.0), 32.0, epsilon);

	int max_iterations = (sizeof(double) < 8) ? 5 : 10;
	double square = 0.0;
	double root = 0.0;

	for (int i = 0; i < max_iterations; ++i) {
		if (verbose) {
			log->append_s(log, "simple_stats_sqrt_newton test ");
			log->append_ul(log, i);
			log->append_eol(log);
		}

		d1 = d1 * 2.0;
		root = d1;
		square = (root * root);
		if (verbose) {
			log->append_s(log, "\t(square: ");
			log->append_f(log, square);
			log->append_s(log, "\troot: ");
			log->append_f(log, root);
			log->append_s(log, ")");
		}
		errs +=
		    check_double(simple_stats_sqrt_newton(square), root,
				 epsilon);
		if (verbose) {
			log->append_eol(log);
		}

		root = (1.0 / d1);
		square = (root * root);
		if (verbose) {
			log->append_s(log, "\t(square: ");
			log->append_f(log, square);
			log->append_s(log, "\troot: ");
			log->append_f(log, root);
			log->append_s(log, ")");
		}
		errs +=
		    check_double(simple_stats_sqrt_newton(square), root,
				 epsilon2);
		if (verbose) {
			log->append_eol(log);
		}

		d2 = d2 * 10;
		root = d2;
		square = (root * root);
		if (verbose) {
			log->append_s(log, "\t(square: ");
			log->append_f(log, square);
			log->append_s(log, "\troot: ");
			log->append_f(log, root);
			log->append_s(log, ")");
		}
		errs +=
		    check_double(simple_stats_sqrt_newton(square), root,
				 epsilon);
		if (verbose) {
			log->append_eol(log);
		}

		root = (1.0 / d2);
		square = (root * root);
		if (verbose) {
			log->append_s(log, "\t(square: ");
			log->append_f(log, square);
			log->append_s(log, "\troot: ");
			log->append_f(log, root);
			log->append_s(log, ")");
		}
		errs +=
		    check_double(simple_stats_sqrt_newton(square), root,
				 epsilon2);
		if (verbose) {
			log->append_eol(log);
		}

		d3 = d3 + d2 + d1;
		root = d3;
		square = (root * root);
		if (verbose) {
			log->append_s(log, "\t(square: ");
			log->append_f(log, square);
			log->append_s(log, "\troot: ");
			log->append_f(log, root);
			log->append_s(log, ")");
		}
		errs +=
		    check_double(simple_stats_sqrt_newton(square), root,
				 epsilon);
		if (verbose) {
			log->append_eol(log);
		}

		root = (1.0 / d3);
		square = (root * root);
		if (verbose) {
			log->append_s(log, "\t(square: ");
			log->append_f(log, square);
			log->append_s(log, "\troot: ");
			log->append_f(log, root);
			log->append_s(log, ")");
		}
		errs +=
		    check_double(simple_stats_sqrt_newton(square), root,
				 epsilon2);
		if (verbose) {
			log->append_eol(log);
		}

		epsilon = (epsilon * 10);
		epsilon2 = (epsilon / 10);
		if (verbose) {
			log->append_eol(log);
		}
	}
	return errs;
}

int test_sstats_basic(int verbose)
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
			expect_max, expect_mean, expect_variance, expect_stddev,
			verbose);

	bessel_correct = 0;
	expect_variance = 4;
	expect_stddev = 2;
	errs +=
	    check_stats(samples, sample_len, bessel_correct, expect_min,
			expect_max, expect_mean, expect_variance, expect_stddev,
			verbose);

	errs += test_simple_stats_sqrt_newton(verbose);

	return errs;
}

#if SIMPLE_STATS_HOSTED
#include <stdlib.h>
int main(int argc, char **argv)
{
	int verbose = argc > 1 ? atoi(argv[1]) : 0;
	int errs = test_sstats_basic(verbose);
	return errs ? 1 : 0;
}
#endif
