/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atomic.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/29 21:06:15 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/29 21:44:52 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>     // write, chdir, dup2, close, execve
#include <sys/wait.h>   // waitpid
#include <stdlib.h>     // exit
#include <string.h>     // strcmp

typedef struct s_pipe
{
	int	has_pipe;
	int	pipe_fds[2];
}				t_pipe;

void	ft_print_error(char *msg)
{
	while (*msg)
		write(STDERR_FILENO, msg++, sizeof(char));
}

int	ft_execute_cd(char **arg, int arg_count)
{
	if (arg_count != 2)
	{
		ft_print_error("error: cd: bad arg\n");
		return (EXIT_FAILURE);
	}
	if (chdir(arg[1]) == -1)
	{
		ft_print_error("error: cd: cannot change directory to ");
		ft_print_error(arg[1]);
		ft_print_error("\n");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

void	ft_configure_pipe(int has_pipe, int *pipe_fds, int end)
{
	if (has_pipe)
	{
		if (dup2(pipe_fds[end], end) == -1)
		{
			ft_print_error("error: fatal\n");
			exit(EXIT_FAILURE);
		}
		if (close(pipe_fds[0]) == -1 || close(pipe_fds[1]) == -1)
		{
			ft_print_error("error: fatal\n");
			exit(EXIT_FAILURE);
		}
	}
}

void	ft_handle_pipe(t_pipe *pipe_info)
{
	if (pipe_info->has_pipe && pipe(pipe_info->pipe_fds) == -1)
	{
		ft_print_error("error: fatal\n");
		exit(EXIT_FAILURE);
	}
}

void	ft_execute_child_process(char **arg, int arg_count, char **env, \
t_pipe *pipe_info)
{
	arg[arg_count] = NULL;
	ft_configure_pipe(pipe_info->has_pipe, pipe_info->pipe_fds, STDOUT_FILENO);
	if (!strcmp(*arg, "cd"))
		exit(ft_execute_cd(arg, arg_count));
	execve(arg[0], arg, env);
	ft_print_error("error: cannot execute ");
	ft_print_error(arg[0]);
	ft_print_error("\n");
	exit(EXIT_FAILURE);
}

int	ft_execute_command(char **arg, int arg_count, char **env)
{
	t_pipe		pipe;
	int			code;
	pid_t		pid;

	pipe.has_pipe = arg[arg	count] && !strcmp(arg[arg_count], "|");
	if (!pipe.has_pipe && !strcmp(*arg, "cd"))
		return (ft_execute_cd(arg, arg_count));
	ft_handle_pipe(&pipe);
	pid = fork();
	if (pid == -1)
	{
		ft_print_error("error: fatal\n");
		exit(EXIT_FAILURE);
	}
	if (pid == 0)
		ft_execute_child_process(arg, arg_count, env, &pipe);
	waitpid(pid, &code, 0);
	ft_configure_pipe(pipe.has_pipe, pipe.pipe_fds, STDIN_FILENO);
	return (WIFEXITED(code) && WEXITSTATUS(code));
}

int	main(int argc, char **argv, char **env)
{
	int	index;
	int	code;

	(void)argc;
	index = 0;
	code = 0;
	while (argv[index])
	{
		argv += index + 1;
		index = 0;
		while (argv[index] && strcmp(argv[index], "|") && \
		strcmp(argv[index], ";"))
			index++;
		if (index)
			code = ft_execute_command(argv, index, env);
	}
	return (code);
}
