/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exec.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:11:09 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:22:44 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

static int ft_execute_cd(char **arguments)
{
	if (!arguments[1] || arguments[2])
	{
		ft_error_message("error: ", "cd: bad arguments", NULL);//macro
		return EXIT_FAILURE;
	}
	if (chdir(arguments[1]) == -1)
	{
		ft_error_message("error: cd: cannot change directory to ", \
		arguments[1], NULL);//macro
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static int ft_execute_external_command(t_micro_shell *shell)
{
	ft_prepare_pipe(shell);
	shell->pid = ft_fork_process(shell);
	if (shell->pid == 0)
		ft_execute_child_process(shell);
	else
		ft_execute_parent_process(shell);
	return EXIT_SUCCESS;
}

int ft_execute_command(t_micro_shell *shell)
{
	int exit_code;

	exit_code = EXIT_FAILURE;
	if (shell->type == TYPE_CD)
		exit_code = ft_execute_cd(shell->arguments);
	else
		exit_code = ft_execute_external_command(shell);
	return (exit_code);
}

void ft_execute_child_process(t_micro_shell *shell)
{
	ft_redirect_fds(shell);
	ft_close_child_fds(shell);
	if (execve(shell->arguments[0], shell->arguments, \
		shell->main_vars.env) == -1)
		ft_cannot_execute_commands(shell);
}

void ft_execute_parent_process(t_micro_shell *shell)
{
	int status;

	if (waitpid(shell->pid, &status, 0) == -1)
		ft_exit_with_fatal_error(shell);
	ft_handle_child_status(shell, status);
	ft_close_fd(shell->pipes.previous_pipe_fd, shell);
	if (shell->type == TYPE_PIPE)
	{
		ft_close_fd(shell->pipes.pipe_fd[PIPE_OUTPUT], shell);
		shell->pipes.previous_pipe_fd = shell->pipes.pipe_fd[PIPE_INPUT];
	}
	else
		shell->pipes.previous_pipe_fd = -1;
}
