/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/04 19:50:05 by tlegrand          #+#    #+#             */
/*   Updated: 2023/04/28 12:51:48 by tlegrand         ###   ########.fr       */
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
	sem_post(data->sem_stop);
	return (NULL);
}

void	kill_eat_watcher(sem_t *sem_meal, pthread_t watcher_pid, int n)
{
	int	i;

	i = n;
	while (i-- >= 0)
		sem_post(sem_meal);
	pthread_join(watcher_pid, NULL);
}

int	main(int ac, char **arg)
{
	t_data		data;
	pthread_t	eat_watcher_pid;
	int			i;

	if (parser(ac, arg, &data))
		return (1);
	if (philo_init(&data))
		return (clear_all(&data));
	i = data.n_philo;
	if (philo_launch(&data) == 0)
	{
		if (data.n_meal != -1)
			pthread_create(&eat_watcher_pid, NULL, eat_watcher, &data);
		sem_wait(data.sem_stop);
	}
	serial_killer(&data);
	while (i-- > 0)
		waitpid(data.philo[i].pid, NULL, 0);
	if (data.n_meal != -1)
		kill_eat_watcher(data.sem_meal, eat_watcher_pid, data.n_philo);
	clear_all(&data);
	exit(0);
}
