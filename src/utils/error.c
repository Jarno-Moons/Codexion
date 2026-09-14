/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 16:26:40 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/08 16:02:55 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

int	print_error(t_error err)
{
	if (err == ERR_ARGC)
		fprintf(stderr, "ERROR: Wrong number of arguments\n");
	else if (err == ERR_NUMBER)
		fprintf(stderr, "ERROR: Invalid number format\n");
	else if (err == ERR_RANGE)
		fprintf(stderr, "ERROR: Invalid or out-of-range value\n");
	else if (err == ERR_SCHEDULER)
		fprintf(stderr, "ERROR: Scheduler must be fifo or edf\n");
	else if (err == ERR_ALLOC)
		fprintf(stderr, "ERROR: Memory allocation failed\n");
	return (1);
}
