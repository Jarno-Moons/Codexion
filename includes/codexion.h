/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamoons <jamoons@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 14:39:20 by jamoons           #+#    #+#             */
/*   Updated: 2026/09/03 16:00:53 by jamoons          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# define C_INT_MAX 2147483647
# define C_LLONG_MAX 9223372036854775807LL
# define C_LLONG_MIN -9223372036854775808LL

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>

/* === STRUCTS === */

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef enum e_error
{
	ERR_NONE = 0,
	ERR_ARGC,
	ERR_NUMBER,
	ERR_RANGE,
	ERR_SCHEDULER,
	ERR_ALLOC
}	t_error;

typedef struct s_config
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_request
{
	int			coder_id;
	long long	arrival_time;
	long long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	*data;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	long long		available_at;
	int				in_use;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	long long		last_compile_time;
	int				compile_count;
	pthread_t		thread;
	struct s_sim	*sim;
	int				left_dongle;
	int				right_dongle;
}	t_coder;

typedef struct s_sim
{
	t_config		config;
	t_coder			*coders;
	long long		start_time;
	int				running;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	state_lock;
	pthread_cond_t	state_cond;
	pthread_t		monitor_thread;
	t_dongle		*dongles;
	t_heap			heap;
}	t_sim;

/* === FUNCTIONS === */

/* cleanup.c */
void		cleanup_sim(t_sim *sim);

/* coder.c */
void		log_state(t_coder *c, const char *msg);
void		*coder_routine(void *arg);

/* monitor.c */
void		*monitor_routine(void *arg);

/* parser.c */
t_error		parse_args(char **argv, t_config *cfg);

/* simulation.c */
t_error		init_sim(t_sim *sim, t_config *cfg);
t_error		init_coders(t_sim *sim);
t_error		start_sim(t_sim *sim);

/* sim_state.c */
void		stop_startup(t_sim *sim);

/* utils/error.c */
int			print_error(t_error err);

/* utils/ft_strtoll.c */
long long	ft_strtoll(const char *nptr, char **endptr);
t_error		parse_positive_number(const char *str, int *val);

/* utils/time.c */
long long	get_time(void);

/* utils/init_resources.c */
int			init_dongles(t_sim *sim);

/* scheduler/heap.c */
void		heap_push(t_heap *heap, t_request req, t_scheduler type);
void		heap_pop(t_heap *heap, t_scheduler type, int coder_id);
t_request	*heap_find_request(t_heap *heap, int coder_id);

/* scheduler/priority.c */
int			has_higher_priority(t_request a, t_request b, t_scheduler type);

/* scheduler/sync.c */
int			acquire_dongles(t_coder *c);
void		release_dongles(t_coder	*c);
int			can_compile(t_coder	*c, long long now);

/* scheduler/request.c */
void		add_request(t_coder *c);
int			has_higher_conflict(t_coder *c, t_request *my_request);
#endif
