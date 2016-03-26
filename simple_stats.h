/* simple_stats.h - Eric Herman 2014 */
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
double simple_stats_variance(simple_stats *stats);
double simple_stats_std_dev(simple_stats *stats);

void simple_stats_to_string(simple_stats *stats, char *buf, size_t buflen);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_STATS_H */
