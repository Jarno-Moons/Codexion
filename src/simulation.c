/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 19:13:15 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/03 16:04:37 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static int	init_resources(t_sim *sim)
{
	sim->dongles = malloc(sizeof(t_dongle)
			* sim->config.number_of_coders);
	sim->coders = malloc(sizeof(t_coder)
			* sim->config.number_of_coders);
	sim->heap.data = malloc(sizeof(t_request)
			* sim->config.number_of_coders);
	if (!sim->dongles || !sim->coders || !sim->heap.data)
		return (1);
	sim->heap.size = 0;
	sim->heap.capacity = sim->config.number_of_coders;
	if (!init_dongles(sim))
		return (1);
	return (0);
}

t_error	init_sim(t_sim *sim, t_config *cfg)
{
	sim->config = *cfg;
	sim->running = 1;
	sim->start_time = 0;
	sim->dongles = NULL;
	sim->coders = NULL;
	sim->heap.data = NULL;
	if (pthread_mutex_init(&sim->print_lock, NULL) != 0)
		return (ERR_ALLOC);
	if (pthread_mutex_init(&sim->state_lock, NULL) != 0)
		return (ERR_ALLOC);
	if (pthread_cond_init(&sim->state_cond, NULL) != 0)
		return (ERR_ALLOC);
	if (init_resources(sim) != 0)
		return (cleanup_sim(sim), ERR_ALLOC);
	return (ERR_NONE);
}

t_error	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_time = 0;
		sim->coders[i].left_dongle = i;
		sim->coders[i].right_dongle = (i + 1) % sim->config.number_of_coders;
		i++;
	}
	return (ERR_NONE);
}

static void	join_threads(t_sim *sim, int count, int monitor_created)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	if (monitor_created)
		pthread_join(sim->monitor_thread, NULL);
}

t_error	start_sim(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			stop_startup(sim);
			join_threads(sim, i, 0);
			return (ERR_ALLOC);
		}
		i++;
	}
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		stop_startup(sim);
		return (join_threads(sim, sim->config.number_of_coders, 0), ERR_ALLOC);
	}
	pthread_mutex_lock(&sim->state_lock);
	sim->start_time = get_time();
	pthread_cond_broadcast(&sim->state_cond);
	pthread_mutex_unlock(&sim->state_lock);
	join_threads(sim, sim->config.number_of_coders, 1);
	return (ERR_NONE);
}
