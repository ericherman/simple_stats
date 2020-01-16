/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats.c stats library */
/* Copyright (C) 2014, 2016, 2019 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#include "simple_stats.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *Simple_stats_version = "1.0.1";

const char *simple_stats_version(void)
{
	return Simple_stats_version;
}

void simple_stats_init(simple_stats *stats)
{
	assert(stats);
	stats->cnt = 0;
	stats->min = DBL_MAX;
	stats->max = -DBL_MAX;
	stats->sum = 0.0;
	stats->sum_of_squares = 0.0;
}

void simple_stats_append_val(simple_stats *stats, double val)
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

double simple_stats_average(simple_stats *stats)
{
	assert(stats);
	return stats->sum / stats->cnt;
}

double simple_stats_variance(simple_stats *stats, int bessel_correct)
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

double simple_stats_std_dev(simple_stats *stats, int bessel_correct)
{
	return sqrt(simple_stats_variance(stats, bessel_correct));
}

char *simple_stats_to_string(simple_stats *stats, char *buf, size_t buflen,
			     int *written)
{
	int rv = -1;
	int bessel_correct = 1;

	assert(stats);

	if (buf) {
		rv = snprintf(buf, buflen,
			      "{ cnt: %u, min: %f, max: %f, avg: %f, std-dev: %f }",
			      stats->cnt, stats->min, stats->max,
			      simple_stats_average(stats),
			      simple_stats_std_dev(stats, bessel_correct));
	}
	if (written) {
		*written = rv;
	}
	return buf;
}

static FILE *_ifp_open(const char *file_name, unsigned short *from_file)
{
	FILE *fp;
	unsigned short _from_file;
	const char *mode = "r";

	_from_file = (strcmp("-", file_name) != 0);
	if (from_file) {
		*from_file = _from_file;
	}

	fp = (_from_file) ? fopen(file_name, mode) : stdin;

	return fp;
}

static void _lex_col_val(char *line_buf, size_t line_buf_len, size_t *lex_pos,
			 char *val_buf, size_t val_buf_len)
{
	size_t maxlen, len = 0;
	unsigned char done = 0;
	char *c;

	while (!done && len < line_buf_len) {
		c = (line_buf + *lex_pos + len++);
		switch (*c) {
		case '\0':
		case '\n':
		case ',':
			{
				done = 1;
			}
		default:
			break;
		}
	}
	if (len > val_buf_len) {
		fprintf(stderr, "len > val_buf_len (%lu > %lu)\n",
			(unsigned long)len, (unsigned long)val_buf_len);
		maxlen = val_buf_len;
	} else {
		maxlen = len;
	}
	strncpy(val_buf, line_buf + *lex_pos, maxlen);
	val_buf[(!done || !maxlen) ? maxlen : maxlen - 1] = '\0';
	*lex_pos += len;
}

simple_stats **simple_stats_from_file(const char *file_name,
				      unsigned int channels,
				      unsigned int skip_cols,
				      unsigned int skip_rows,
				      char *line_buf, size_t line_buf_len,
				      char *val_buf, size_t val_buf_len,
				      FILE *err, size_t *len)
{
	simple_stats **stats;
	FILE *ifp;
	unsigned short from_file;
	unsigned int rows_skipped = 0;
	unsigned int cols_skipped = 0;
	unsigned int channel;
	unsigned int columns;
	size_t i, local_len, size, lex_pos = 0;
	double d;

	if (!len) {
		len = &local_len;
	}
	*len = 0;

	size = (sizeof(simple_stats *) * (1 + channels));
	stats = calloc(1, size);
	if (!stats) {
		fprintf(err, "%s:%d: ", __FILE__, __LINE__);
		fprintf(err, "failed to alloc %lu bytes?\n",
			(unsigned long)size);
		return NULL;
	}

	for (i = 0; i < channels; i++) {
		size = sizeof(simple_stats);
		stats[i] = malloc(size);
		if (!stats[i]) {
			fprintf(err, "%s:%d: ", __FILE__, __LINE__);
			fprintf(err, "alloc %lu bytes failed? (stats[%lu])\n",
				(unsigned long)size, (unsigned long)i);
		} else {
			++(*len);
			simple_stats_init(stats[i]);
		}
	}
	if (!(*len)) {
		free(stats);
		return NULL;
	}

	ifp = _ifp_open(file_name, &from_file);
	if (ifp == NULL) {
		fprintf(stderr, "can not open '%s'\n", file_name);
		return stats;
	}

	columns = skip_cols + *len;
	while (fgets(line_buf, line_buf_len, ifp) != NULL) {
		if (rows_skipped++ < skip_rows) {
			continue;
		}
		cols_skipped = 0;
		lex_pos = 0;
		for (i = 0; i < columns; i++) {
			_lex_col_val(line_buf, line_buf_len, &lex_pos, val_buf,
				     val_buf_len);
			if (cols_skipped++ < skip_cols) {
				continue;
			}
			channel = i - skip_cols;
			sscanf(val_buf, "%lf%*s", &d);
			simple_stats_append_val(stats[channel], d);
		}
	}

	if (from_file) {
		fclose(ifp);
	}

	return stats;
}
