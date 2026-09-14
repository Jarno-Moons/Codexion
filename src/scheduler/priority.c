/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 16:29:13 by jamoons           #+#    #+#             */
/*   Updated: 2026/07/30 16:04:39 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

int	has_higher_priority(t_request a, t_request b, t_scheduler type)
{
	if (type == FIFO)
	{
		if (a.arrival_time != b.arrival_time)
			return (a.arrival_time < b.arrival_time);
	}
	else
	{
		if (a.deadline != b.deadline)
			return (a.deadline < b.deadline);
		if (a.arrival_time != b.arrival_time)
			return (a.arrival_time < b.arrival_time);
	}
	return (a.coder_id < b.coder_id);
}
