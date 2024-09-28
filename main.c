/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 12:41:52 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:01:46 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

/*
clang -g3 -Wall -Werror -Wextra -o microshell microshell.c && valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --show-reachable=yes --track-origins=yes ./microshell
*/
int main(int argc, char **argv, char **envp)
{
	t_micro_shell shell;

	//printf("Starting microshell...\n"); // Debug
	ft_initialize_micro_shell(&shell, argc, argv, envp);
	while (shell.index < shell.main_vars.argc)
	{
		shell.arguments = NULL;
		shell.type = TYPE_NONE;
		ft_parse_arguments(&shell);
		if (shell.arguments)
		{
			shell.exit_code = ft_execute_command(&shell);
			free(shell.arguments);
			shell.arguments = NULL;
		}
	}
	//printf("Microshell exited with code %d\n", shell.exit_code); // Debug
	return (shell.exit_code);
}
