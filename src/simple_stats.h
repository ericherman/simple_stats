/* simple_stats.h stats library API
   Copyright (C) 2014, 2016 Eric Herman <eric@freesa.org>

   This work is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This work is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public
   License (COPYING) and the GNU General Public License (COPYING.GPL3).
   If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SIMPLE_STATS_H
#define SIMPLE_STATS_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct simple_stats_s {
	unsigned int cnt;
	double min;
	double max;
	double sum;
	double sum_of_squares;
} simple_stats;

void simple_stats_init(simple_stats *stats);

void simple_stats_append_val(simple_stats *stats, double val);

double simple_stats_average(simple_stats *stats);
double simple_stats_variance(simple_stats *stats, int bessel_correct);
double simple_stats_std_dev(simple_stats *stats, int bessel_correct);

char *simple_stats_to_string(simple_stats *stats, char *buf, size_t buflen,
			     int *written);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_STATS_H */
