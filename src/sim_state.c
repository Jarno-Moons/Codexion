/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 15:58:06 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/03 15:59:56 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

void	stop_startup(t_sim *sim)
{
	pthread_mutex_lock(&sim->state_lock);
	sim->running = 0;
	pthread_cond_broadcast(&sim->state_cond);
	pthread_mutex_unlock(&sim->state_lock);
}
