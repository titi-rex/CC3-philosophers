/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/04 19:50:05 by tlegrand          #+#    #+#             */
/*   Updated: 2023/04/26 17:29:14 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

void	serial_killer(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->n_philo)
	{
		if (data->philo[i].pid != -1)
			kill(data->philo[i].pid, SIGQUIT);
	}
}

void	*eat_watcher(void *ptr)
{
	t_data	*data;
	int		i;

	data = ptr;
	i = -1;
	while (++i < data->n_philo)
		sem_wait(data->sem_meal);
	sem_wait(data->sem_print);
	sem_post(data->sem_stop);
	return (NULL);
}

int	main(int ac, char **arg)
{
	t_data		data;
	pthread_t	eat_watcher_pid;

	if (parser(ac, arg, &data))
		return (1);
	if (data.n_philo == 0)
		return (0);
	if (philo_init(&data))
		return (clear_all(&data), 1);
	if (philo_launch(&data) == 0)
	{
		if (data.n_meal != -1)
		{
			pthread_create(&eat_watcher_pid, NULL, eat_watcher, &data);
			pthread_detach(eat_watcher_pid);
		}
		sem_wait(data.sem_stop);
	}
	serial_killer(&data);
	if (data.n_meal != -1)
		pthread_join(eat_watcher_pid, NULL);
	clear_all(&data);
	exit(0);
}
