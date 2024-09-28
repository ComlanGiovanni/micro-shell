/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_error.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:06:20 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:23:08 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

static void ft_exit_with_fatal_error(t_micro_shell *shell)
{
	ft_error_message("error: ", "fatal", NULL);
	ft_cleanup(shell);
	exit(EXIT_FAILURE);
}

void ft_error_message(char *prefix, char *message, char *suffix)
{
	if (prefix)
		ft_putstr_fd(prefix, STDERR_FILENO);
	if (message)
		ft_putstr_fd(message, STDERR_FILENO);
	if (suffix)
		ft_putstr_fd(suffix, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

static void ft_cleanup(t_micro_shell *shell)
{
	int index_arguments;

	if (shell->arguments)
	{
		index_arguments = 0;
		while (shell->arguments[index_arguments])
		{
			free(shell->arguments[index_arguments]);
			index_arguments++;
		}
		free(shell->arguments);
		shell->arguments = NULL;
	}
	ft_close_fd(shell->pipes.previous_pipe_fd, shell);
	ft_close_fd(shell->pipes.pipe_fd[PIPE_INPUT], shell);
	ft_close_fd(shell->pipes.pipe_fd[PIPE_OUTPUT], shell);
}

static void ft_cannot_execute_commands(t_micro_shell *shell)
{
	ft_error_message("error: cannot execute ", shell->arguments[0], NULL);
	ft_cleanup(shell);
	exit(EXIT_FAILURE);
}

static void ft_close_fd(int fd, t_micro_shell *shell)
{
	if (fd != -1 && close(fd) == -1)
		ft_exit_with_fatal_error(shell);
}
