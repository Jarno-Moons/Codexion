/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtoll.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:23:01 by jamoons           #+#    #+#             */
/*   Updated: 2026/06/15 15:22:59 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

static const char	*skip_and_sign(const char *str, int *sign)
{
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			*sign = -1;
		str++;
	}
	return (str);
}

static long long	parse_digits(const char *str, const char **end)
{
	long long	res;

	res = 0;
	while (*str >= '0' && *str <= '9')
	{
		if (res > C_LLONG_MAX)
			break ;
		res = res * 10 + (*str - '0');
		str++;
	}
	*end = str;
	return (res);
}

long long	ft_strtoll(const char *nptr, char **endptr)
{
	const char	*start;
	const char	*digits;
	int			sign;
	long long	result;

	sign = 1;
	start = skip_and_sign(nptr, &sign);
	digits = start;
	result = parse_digits(start, &start);
	if (endptr)
	{
		if (digits == start)
			*endptr = (char *)nptr;
		else
			*endptr = (char *)start;
	}
	return (result * sign);
}
