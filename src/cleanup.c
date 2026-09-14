/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 17:00:57 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/01 16:34:09 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static void	cleanup_dongles(t_sim *sim)
{
	int	i;

	if (sim->dongles)
	{
		i = 0;
		while (i < sim->config.number_of_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			i++;
		}
		free(sim->dongles);
		sim->dongles = NULL;
	}
}

void	cleanup_sim(t_sim *sim)
{
	if (!sim)
		return ;
	cleanup_dongles(sim);
	if (sim->heap.data)
	{
		free(sim->heap.data);
		sim->heap.data = NULL;
	}
	if (sim->coders)
	{
		free(sim->coders);
		sim->coders = NULL;
	}
	pthread_mutex_destroy(&sim->print_lock);
	pthread_mutex_destroy(&sim->state_lock);
	pthread_cond_destroy(&sim->state_cond);
}
