/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 16:51:34 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/02 16:09:45 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

t_request	*heap_find_request(t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->data[i].coder_id == coder_id)
			return (&heap->data[i]);
		i++;
	}
	return (NULL);
}

static int	heap_up(t_heap *heap, t_scheduler type, int i)
{
	t_request	tmp;
	int			moved;

	moved = 0;
	while (i > 0)
	{
		if (has_higher_priority(heap->data[i],
				heap->data[(i - 1) / 2], type))
		{
			tmp = heap->data[i];
			heap->data[i] = heap->data[(i - 1) / 2];
			heap->data[(i - 1) / 2] = tmp;
			i = (i - 1) / 2;
			moved = 1;
		}
		else
			break ;
	}
	return (moved);
}

static void	heap_down(t_heap *heap, t_scheduler type, int i)
{
	int			child;
	t_request	tmp;

	while (i * 2 + 1 < heap->size)
	{
		child = i * 2 + 1;
		if (child + 1 < heap->size && has_higher_priority(
				heap->data[child + 1], heap->data[child], type))
			child++;
		if (has_higher_priority(heap->data[i], heap->data[child], type))
			break ;
		tmp = heap->data[i];
		heap->data[i] = heap->data[child];
		heap->data[child] = tmp;
		i = child;
	}
}

void	heap_push(t_heap *heap, t_request req, t_scheduler type)
{
	int	i;

	i = heap->size;
	heap->data[i] = req;
	heap->size++;
	heap_up(heap, type, i);
}

void	heap_pop(t_heap *heap, t_scheduler type, int coder_id)
{
	int			i;
	int			moved;

	i = 0;
	while (i < heap->size)
	{
		if (heap->data[i].coder_id == coder_id)
			break ;
		i++;
	}
	if (i == heap->size)
		return ;
	heap->size--;
	if (i == heap->size)
		return ;
	heap->data[i] = heap->data[heap->size];
	moved = heap_up(heap, type, i);
	if (!moved)
		heap_down(heap, type, i);
}
