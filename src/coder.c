/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 15:05:06 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/03 16:53:20 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

void	log_state(t_coder *c, const char *msg)
{
	pthread_mutex_lock(&c->sim->state_lock);
	if (c->sim->running)
	{
		pthread_mutex_lock(&c->sim->print_lock);
		printf("%lld %d %s\n", get_time() - c->sim->start_time, c->id, msg);
		pthread_mutex_unlock(&c->sim->print_lock);
	}
	pthread_mutex_unlock(&c->sim->state_lock);
}

static int	perform_activity(t_coder *c, const char *msg, int duration_ms)
{
	pthread_mutex_lock(&c->sim->state_lock);
	if (!c->sim->running || (c->sim->config.number_of_compiles_required > 0
			&& c->compile_count >= c->sim->config.number_of_compiles_required))
	{
		pthread_mutex_unlock(&c->sim->state_lock);
		return (0);
	}
	pthread_mutex_unlock(&c->sim->state_lock);
	if (msg)
		log_state(c, msg);
	if (duration_ms > 0)
		usleep(duration_ms * 1000);
	return (1);
}

static int	should_continue(t_coder *c)
{
	int	res;

	pthread_mutex_lock(&c->sim->state_lock);
	res = c->sim->running;
	if (c->sim->config.number_of_compiles_required > 0
		&& c->compile_count >= c->sim->config.number_of_compiles_required)
		res = 0;
	pthread_mutex_unlock(&c->sim->state_lock);
	return (res);
}

void	*coder_routine(void *arg)
{
	t_coder	*c ;

	c = (t_coder *)arg;
	pthread_mutex_lock(&c->sim->state_lock);
	while (c->sim->start_time == 0 && c->sim->running)
		pthread_cond_wait(&c->sim->state_cond, &c->sim->state_lock);
	pthread_mutex_unlock(&c->sim->state_lock);
	while (should_continue(c))
	{
		if (!acquire_dongles(c))
			break ;
		log_state(c, "has taken a dongle");
		log_state(c, "has taken a dongle");
		perform_activity(c, "is compiling", c->sim->config.time_to_compile);
		release_dongles(c);
		perform_activity(c, "is debugging", c->sim->config.time_to_debug);
		perform_activity(c, "is refactoring", c->sim->config.time_to_refactor);
		pthread_mutex_lock(&c->sim->state_lock);
		c->compile_count++;
		pthread_mutex_unlock(&c->sim->state_lock);
	}
	return (NULL);
}
