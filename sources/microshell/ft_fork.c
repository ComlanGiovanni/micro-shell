/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fork.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:18:56 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:20:16 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

pid_t ft_fork_process(t_micro_shell *shell)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
		ft_exit_with_fatal_error(shell);
	return pid;
}

void ft_redirect_fds(t_micro_shell *shell)
{
	if (shell->pipes.previous_pipe_fd != -1)
	{
		if (dup2(shell->pipes.previous_pipe_fd, PIPE_INPUT) == -1)
			ft_exit_with_fatal_error(shell);
	}
	if (shell->type == TYPE_PIPE)
	{
		if (dup2(shell->pipes.pipe_fd[PIPE_OUTPUT], PIPE_OUTPUT) == -1)
			ft_exit_with_fatal_error(shell);
	}
}

void ft_close_child_fds(t_micro_shell *shell)
{
	ft_close_fd(shell->pipes.previous_pipe_fd, shell);
	if (shell->type == TYPE_PIPE)
	{
		ft_close_fd(shell->pipes.pipe_fd[PIPE_INPUT], shell);
		ft_close_fd(shell->pipes.pipe_fd[PIPE_OUTPUT], shell);
	}
}

void ft_handle_child_status(t_micro_shell *shell, int status)
{
	if (WIFEXITED(status))

		shell->exit_code = WEXITSTATUS(status);
	else
		shell->exit_code = EXIT_FAILURE;
}
