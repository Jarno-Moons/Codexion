/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 18:52:57 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/08 16:08:40 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static void	init_fields(t_config *cfg, int **fields)
{
	fields[0] = &cfg->number_of_coders;
	fields[1] = &cfg->time_to_burnout;
	fields[2] = &cfg->time_to_compile;
	fields[3] = &cfg->time_to_debug;
	fields[4] = &cfg->time_to_refactor;
}

static	t_error	parse_scheduler(const char *str, t_scheduler *scheduler)
{
	if (!strcmp(str, "fifo"))
		*scheduler = FIFO;
	else if (!strcmp(str, "edf"))
		*scheduler = EDF;
	else
		return (ERR_SCHEDULER);
	return (ERR_NONE);
}

static	t_error	parse_numeric_fields(char **argv, t_config *cfg)
{
	int		i;
	int		v;
	t_error	err;
	int		*fields[5];

	init_fields(cfg, fields);
	i = 0;
	while (i < 7)
	{
		err = parse_positive_number(argv[i + 1], &v);
		if (err != ERR_NONE)
			return (err);
		if (i < 6 && v == 0)
			return (ERR_RANGE);
		if (i < 5)
			*fields[i] = v;
		else if (i == 5)
			cfg->number_of_compiles_required = v;
		else
			cfg->dongle_cooldown = v;
		i++;
	}
	return (ERR_NONE);
}

t_error	parse_args(char **argv, t_config *cfg)
{
	t_error		err;

	memset(cfg, 0, sizeof(t_config));
	err = parse_numeric_fields(argv, cfg);
	if (err != ERR_NONE)
		return (err);
	return (parse_scheduler(argv[8], &cfg->scheduler));
}

t_error	parse_positive_number(const char *str, int *value)
{
	char		*endptr;
	long long	v;

	v = ft_strtoll(str, &endptr);
	if (str == endptr || *endptr != '\0')
		return (ERR_NUMBER);
	if (v < 0)
		return (ERR_RANGE);
	if (v > C_INT_MAX)
		return (ERR_RANGE);
	*value = (int)v;
	return (ERR_NONE);
}
