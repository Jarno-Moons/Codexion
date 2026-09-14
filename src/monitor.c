/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 19:15:32 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/03 15:52:17 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static int	is_burned_out(t_coder *c, long long now)
{
	long long	last;

	pthread_mutex_lock(&c->sim->state_lock);
	if (!c->sim->running)
	{
		pthread_mutex_unlock(&c->sim->state_lock);
		return (1);
	}
	last = c->last_compile_time;
	if (last == 0)
		last = c->sim->start_time;
	if (now - last > c->sim->config.time_to_burnout)
	{
		c->sim->running = 0;
		pthread_cond_broadcast(&c->sim->state_cond);
		pthread_mutex_lock(&c->sim->print_lock);
		printf("%lld %d burned out\n", now - c->sim->start_time, c->id);
		pthread_mutex_unlock(&c->sim->print_lock);
		pthread_mutex_unlock(&c->sim->state_lock);
		return (1);
	}
	pthread_mutex_unlock(&c->sim->state_lock);
	return (0);
}

static int	check_burnout(t_sim *sim)
{
	int			i;
	long long	now;

	i = 0;
	now = get_time();
	while (i < sim->config.number_of_coders)
	{
		if (is_burned_out(&sim->coders[i], now))
			return (1);
		i++;
	}
	return (0);
}

static int	check_completion(t_sim *sim)
{
	int	i;

	if (sim->config.number_of_compiles_required <= 0)
		return (0);
	pthread_mutex_lock(&sim->state_lock);
	if (!sim->running)
	{
		pthread_mutex_unlock(&sim->state_lock);
		return (1);
	}
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (sim->coders[i].compile_count
			< sim->config.number_of_compiles_required)
		{
			pthread_mutex_unlock(&sim->state_lock);
			return (0);
		}
		i++;
	}
	sim->running = 0;
	pthread_cond_broadcast(&sim->state_cond);
	pthread_mutex_unlock(&sim->state_lock);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	pthread_mutex_lock(&sim->state_lock);
	while (sim->start_time == 0 && sim->running)
		pthread_cond_wait(&sim->state_cond, &sim->state_lock);
	pthread_mutex_unlock(&sim->state_lock);
	while (1)
	{
		if (check_burnout(sim) || check_completion(sim))
			break ;
		pthread_mutex_lock(&sim->state_lock);
		pthread_cond_broadcast(&sim->state_cond);
		pthread_mutex_unlock(&sim->state_lock);
		usleep(1000);
	}
	return (NULL);
}
