/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:01:23 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:01:35 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

static void ft_initialize_micro_shell(t_micro_shell *shell, int argc, char **argv, char **envp)
{
	shell->main_vars.argv = argv;
	shell->main_vars.env = envp;
	shell->main_vars.argc = argc;
	shell->index = 1;
	shell->pipes.pipe_fd[PIPE_INPUT] = -1;
	shell->pipes.pipe_fd[PIPE_OUTPUT] = -1;
	shell->pipes.previous_pipe_fd = -1;
	shell->exit_code = EXIT_SUCCESS;
	shell->arguments = NULL;
	shell->type = TYPE_NONE;
}
