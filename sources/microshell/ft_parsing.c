/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_parsing.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:02:37 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:10:23 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"


static void ft_skip_semicolons(t_micro_shell *shell)
{
	while (shell->index < shell->main_vars.argc &&
		   strcmp(shell->main_vars.argv[shell->index], SEMICOLON) == 0)
		shell->index++;
}

static int ft_get_command_end(t_micro_shell *shell)
{
	int index;

	index = shell->index;
	while (index < shell->main_vars.argc &&
		   strcmp(shell->main_vars.argv[index], PIPE) != 0 &&
		   strcmp(shell->main_vars.argv[index], SEMICOLON) != 0)
		index++;
	return (index);
}

// Libère la mémoire allouée pour les arguments
static void ft_free_arguments(char **cmd_arguments, int max_index)
{
	int index;

	index = 0;
	while (index < max_index)
	{
		free(cmd_arguments[index]);
		index++;
	}
	free(cmd_arguments);
}

// Récupère les arguments de la commande
static char **ft_get_command_arguments(t_micro_shell *shell, int start, int end)
{
	char	**cmd_arguments;
	int	 index;
	int	 cmd_length;

	if (start >= end)
		return NULL;
	cmd_length = end - start;
	cmd_arguments = (char **)malloc(sizeof(char *) * (cmd_length + 1));
	if (!cmd_arguments)
		ft_exit_with_fatal_error(shell);
	index = 0;
	while (start < end)
	{
		cmd_arguments[index] = ft_strdup(shell->main_vars.argv[start]);
		if (!cmd_arguments[index])
		{
			ft_error_message("error: ", "Malloc arguments fail", NULL);
			ft_free_arguments(cmd_arguments, index);
			ft_exit_with_fatal_error(shell);
		}
		index++;
		start++;
	}
	cmd_arguments[index] = NULL;
	return cmd_arguments;
}

void ft_parse_arguments(t_micro_shell *shell)
{
	int start;
	int end;

	ft_skip_semicolons(shell);
	if (shell->index >= shell->main_vars.argc)
		return;
	start = shell->index;
	end = ft_get_command_end(shell);
	shell->arguments = ft_get_command_arguments(shell, start, end);
	shell->index = end;
	if (ft_is_pipe(shell))
	{
		shell->type = TYPE_PIPE;
		shell->index++;
	}
	else if (ft_is_semicolon(shell))
	{
		shell->type = TYPE_SEMICOLON;
		shell->index++;
	}
	else if (ft_is_cd(shell))
		shell->type = TYPE_CD;
	else
		shell->type = TYPE_NONE;
	//ft_print_micro_shell(shell); // Debug
}
