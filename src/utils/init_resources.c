/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_resources.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 15:23:18 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/01 15:43:38 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

int	init_dongles(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < sim->config.number_of_coders)
	{
		if (!pthread_mutex_init(&sim->dongles[i].mutex, NULL))
		{
			sim->dongles[i].in_use = 0;
			sim->dongles[i].available_at = 0;
		}
		else
		{
			i--;
			while (i >= 0)
			{
				pthread_mutex_destroy(&sim->dongles[i].mutex);
				i--;
			}
			return (0);
		}
	}
	return (1);
}
