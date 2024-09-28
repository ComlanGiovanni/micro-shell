/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_debug.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:16:36 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:20:40 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

// // Fonction de debug pour afficher la structure t_micro_shell
// void ft_print_micro_shell(t_micro_shell *shell)
// {
// 	 int index = 0;

// 	 printf("Micro Shell Structure:\n");
// 	 printf("PID: %d\n", shell->pid);
// 	 printf("Argc: %d\n", shell->main_vars.argc);
// 	 printf("Index: %d\n", shell->index);
// 	 printf("Exit Code: %d\n", shell->exit_code);
// 	 printf("Pipe FD: [%d, %d]\n", shell->pipes.pipe_fd[0], shell->pipes.pipe_fd[1]);
// 	 printf("Prev Pipe FD: %d\n", shell->pipes.previous_pipe_fd);
// 	 printf("Type: ");
// 	 if (shell->type == TYPE_NONE)
// 		 printf("TYPE_NONE\n");
// 	 else if (shell->type == TYPE_PIPE)
// 		 printf("TYPE_PIPE\n");
// 	 else if (shell->type == TYPE_SEMICOLON)
// 		 printf("TYPE_SEMICOLON\n");
// 	 else if (shell->type == TYPE_CD)
// 		 printf("TYPE_CD\n");
// 	 else
// 		 printf("UNKNOWN\n");
// 	 printf("Arguments:\n");
// 	 if (shell->arguments)
// 	 {
// 		 while (shell->arguments[index])
// 		 {
// 			 printf("  arguments[%d]: %s\n", index, shell->arguments[index]);
// 			 index++;
// 		 }
// 	 }
// 	 else
// 		 printf("  No arguments\n");
// }
