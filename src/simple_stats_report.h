/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* simple_stats_report.h stats report API */
/* Copyright (C) 2014, 2016, 2019, 2020 Eric Herman <eric@freesa.org> */
/* https://github.com/ericherman/simple_stats */

#ifndef SIMPLE_STATS_REPORT_H
#define SIMPLE_STATS_REPORT_H 1

#ifdef __cplusplus
#define Simple_stats_report_begin_C_functions extern "C" {
#define Simple_stats_report_end_C_functions }
#else
#define Simple_stats_report_begin_C_functions
#define Simple_stats_report_end_C_functions
#endif

Simple_stats_report_begin_C_functions
#undef Simple_stats_report_begin_C_functions
#include <stdio.h>
/*
 * declare the existance of "struct simple_stats" but size is not
 * important for this header.
 */
    struct simple_stats;

struct simple_stats **simple_stats_from_file(const char *file_name,
					     unsigned int channels,
					     unsigned int skip_cols,
					     unsigned int skip_rows,
					     char *line_buf,
					     size_t line_buf_len, char *val_buf,
					     size_t val_buf_len, FILE *err,
					     size_t *len);

char *simple_stats_to_string(struct simple_stats *stats, char *buf,
			     size_t buflen, int *written);

Simple_stats_report_end_C_functions
#undef Simple_stats_report_end_C_functions
#endif /* SIMPLE_STATS_REPORT_H */
