/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:18:49 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/02 19:07:15 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

void	add_request(t_coder *c)
{
	t_request	req;
	t_sim		*sim;

	sim = c->sim;
	req.coder_id = c->id;
	req.arrival_time = get_time();
	if (c->last_compile_time == 0)
		req.deadline = sim->start_time + sim->config.time_to_burnout;
	else
		req.deadline = c->last_compile_time + sim->config.time_to_burnout;
	heap_push(&sim->heap, req, sim->config.scheduler);
}

static int	has_resource_conflict(t_coder *c, int higher_coder_id)
{
	t_coder	*higher;

	higher = &c->sim->coders[higher_coder_id - 1];
	if (c->left_dongle == higher->left_dongle
		|| c->left_dongle == higher->right_dongle
		|| c->right_dongle == higher->left_dongle
		|| c->right_dongle == higher->right_dongle)
		return (1);
	return (0);
}

int	has_higher_conflict(t_coder *c, t_request *my_request)
{
	t_sim	*sim;
	int		i;

	sim = c->sim;
	i = 0;
	while (i < sim->heap.size)
	{
		if (has_higher_priority(sim->heap.data[i], *my_request,
				sim->config.scheduler))
		{
			if (has_resource_conflict(c, sim->heap.data[i].coder_id))
				return (0);
		}
		i++;
	}
	return (1);
}
