/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_microshell_structs.h                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 12:47:49 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 12:55:59 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MICRO_SHELL_STRUCTS_H
# define FT_MICRO_SHELL_STRUCTS_H

typedef struct s_main_variable
{
	char	**argv;
	char	**env;
	int	 argc;
}   t_main_variable;

typedef struct s_pipe
{
	int	 pipe_fd[2];
	int	 previous_pipe_fd;
}   t_pipe;


typedef struct s_micro_shell
{
	t_main_variable main_vars;
	t_pipe		  pipes;
	char			**arguments;
	t_command_type  type;
	pid_t		   pid;
	int			 index;
	int			 exit_code;
}   t_micro_shell;

#endif //!FT_MICRO_SHELL_STRUCTS_H
