/* simple_stats.c stats library
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

#include "simple_stats.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

void simple_stats_init(simple_stats *stats)
{
	stats->cnt = 0;
	stats->min = DBL_MAX;
	stats->max = -DBL_MAX;
	stats->sum = 0.0;
	stats->sum_of_squares = 0.0;
}

void simple_stats_append_val(simple_stats *stats, double val)
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

double simple_stats_average(simple_stats *stats)
{
	return stats->sum / stats->cnt;
}

double simple_stats_variance(simple_stats *stats)
{
	return stats->sum_of_squares / stats->cnt;
}

double simple_stats_std_dev(simple_stats *stats)
{
	return sqrt(simple_stats_variance(stats));
}

void simple_stats_to_string(simple_stats *stats, char *buf, size_t buflen)
{
	snprintf(buf, buflen,
		 "{ cnt: %u, min: %f, max: %f, avg: %f, std-dev: %f }",
		 stats->cnt, stats->min, stats->max,
		 simple_stats_average(stats), simple_stats_std_dev(stats));
}
