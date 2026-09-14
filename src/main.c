/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 16:33:20 by jamoons           #+#    #+#             */
/*   Updated: 2026/07/09 16:33:53 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config	cfg;
	t_error		err;
	t_sim		sim;

	if (argc != 9)
		return (print_error(ERR_ARGC));
	err = parse_args(argv, &cfg);
	if (err != ERR_NONE)
		return (print_error(err));
	err = init_sim(&sim, &cfg);
	if (err != ERR_NONE)
		return (print_error(err));
	err = init_coders(&sim);
	if (err != ERR_NONE)
	{
		cleanup_sim(&sim);
		return (print_error(err));
	}
	err = start_sim(&sim);
	cleanup_sim(&sim);
	if (err != ERR_NONE)
		return (print_error(err));
	return (0);
}
