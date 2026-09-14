/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sync.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 16:24:31 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/08 15:28:07 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static void	get_dongle_order(t_coder *c, int *first, int *second)
{
	if (c->left_dongle < c->right_dongle)
	{
		*first = c->left_dongle;
		*second = c->right_dongle;
	}
	else
	{
		*first = c->right_dongle;
		*second = c->left_dongle;
	}
}

int	acquire_dongles(t_coder *c)
{
	t_sim		*sim;
	int			first;
	int			second;

	sim = c->sim;
	pthread_mutex_lock(&sim->state_lock);
	get_dongle_order(c, &first, &second);
	add_request(c);
	while (!can_compile(c, get_time()) && sim->running)
		pthread_cond_wait(&sim->state_cond, &sim->state_lock);
	if (!sim->running)
	{
		pthread_mutex_unlock(&sim->state_lock);
		return (0);
	}
	heap_pop(&sim->heap, sim->config.scheduler, c->id);
	pthread_mutex_lock(&sim->dongles[first].mutex);
	pthread_mutex_lock(&sim->dongles[second].mutex);
	sim->dongles[first].in_use = 1;
	sim->dongles[second].in_use = 1;
	pthread_mutex_unlock(&sim->dongles[second].mutex);
	pthread_mutex_unlock(&sim->dongles[first].mutex);
	c->last_compile_time = get_time();
	pthread_mutex_unlock(&sim->state_lock);
	return (1);
}

void	release_dongles(t_coder *c)
{
	t_sim		*sim;
	long long	cooldown_expiry;
	int			first;
	int			second;

	sim = c->sim;
	get_dongle_order(c, &first, &second);
	cooldown_expiry = get_time() + sim->config.dongle_cooldown;
	pthread_mutex_lock(&sim->state_lock);
	pthread_mutex_lock(&sim->dongles[first].mutex);
	pthread_mutex_lock(&sim->dongles[second].mutex);
	sim->dongles[first].in_use = 0;
	sim->dongles[first].available_at = cooldown_expiry;
	sim->dongles[second].in_use = 0;
	sim->dongles[second].available_at = cooldown_expiry;
	pthread_mutex_unlock(&sim->dongles[second].mutex);
	pthread_mutex_unlock(&sim->dongles[first].mutex);
	pthread_cond_broadcast(&sim->state_cond);
	pthread_mutex_unlock(&sim->state_lock);
}

static int	dongles_available(t_coder *c, long long now)
{
	int	first;
	int	second;
	int	available;

	get_dongle_order(c, &first, &second);
	pthread_mutex_lock(&c->sim->dongles[first].mutex);
	pthread_mutex_lock(&c->sim->dongles[second].mutex);
	available = 1;
	if (c->sim->dongles[first].in_use
		|| c->sim->dongles[second].in_use)
		available = 0;
	if (now < c->sim->dongles[first].available_at
		|| now < c->sim->dongles[second].available_at)
		available = 0;
	pthread_mutex_unlock(&c->sim->dongles[second].mutex);
	pthread_mutex_unlock(&c->sim->dongles[first].mutex);
	return (available);
}

int	can_compile(t_coder *c, long long now)
{
	t_sim		*sim;
	t_request	*my_request;

	sim = c->sim;
	if (c->left_dongle == c->right_dongle)
		return (0);
	if (!dongles_available(c, now))
		return (0);
	my_request = heap_find_request(&sim->heap, c->id);
	if (!my_request)
		return (0);
	if (now > my_request->deadline)
		return (0);
	if (!has_higher_conflict(c, my_request))
		return (0);
	return (1);
}
