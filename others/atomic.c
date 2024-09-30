/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atomic.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/29 21:06:15 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/30 08:15:03 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>		// write,fork,execve,dup2,close,chdir,STDERR_FILENO
#include <sys/wait.h>	// waitpid pid_t
#include <stdlib.h>		// malloc, free, exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>		// strcmp

size_t	ft_strlen(char *string)
{
	const char	*last_char_in_string;

	if (!string)
		return (0x0);
	last_char_in_string = string;
	while (*last_char_in_string)
		last_char_in_string++;
	return (last_char_in_string - string);
}

static void	ft_putstr_fd(char *string, int fd)
{
	if (!string)
		string = "(null)";
	if (fd >= 0x0)
		write(fd, string, ft_strlen(string));
}

static void	ft_error_message(char *prefix, char *message, char *suffix)
{
	if (prefix)
		ft_putstr_fd(prefix, STDERR_FILENO);
	if (message)
		ft_putstr_fd(message, STDERR_FILENO);
	if (suffix)
		ft_putstr_fd(suffix, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

static void	ft_exit_with_error(char *message, char *arg)
{
	ft_error_message(message, arg, NULL);
	exit(EXIT_FAILURE);
}

static int	ft_execute_cd(char **arg, int arg_count)
{
	if (arg_count != 0x2)
	{
		ft_error_message("error: ", "cd: bad arguments", NULL);
		return (EXIT_FAILURE);
	}
	if (chdir(arg[0x1]) == -1)
	{
		ft_error_message("error: cd: cannot change directory to ", \
			arg[1], NULL);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static void	ft_configure_pipe(int has_pipe, int *pipe_fds, int end)
{
	if (has_pipe)
	{
		if (dup2(pipe_fds[end], end) == -1)
			ft_exit_with_error("error: ", "fatal");
		if (close(pipe_fds[0]) == -1 || close(pipe_fds[1]) == -1)
			ft_exit_with_error("error: ", "fatal");
	}
}

static int	ft_execute_command(char **arguments, int arg_count, char **env)
{
	int			has_pipe;
	int			pipe_fds[0x2];
	int			code;
	pid_t		pid;

	has_pipe = arguments[arg_count] && !strcmp(arguments[arg_count], "|");
	if (!has_pipe && !strcmp(*arguments, "cd"))
		return (ft_execute_cd(arguments, arg_count));
	if (has_pipe && pipe(pipe_fds) == -1)
		ft_exit_with_error("error: ", "fatal");
	pid = fork();
	if (pid == -1)
		ft_exit_with_error("error: ", "fatal");
	if (pid == 0x0)
	{
		arguments[arg_count] = NULL;
		ft_configure_pipe(has_pipe, pipe_fds, STDOUT_FILENO);
		if (!strcmp(*arguments, "cd"))
			exit(ft_execute_cd(arguments, arg_count));
		execve(arguments[0], arguments, env);
		ft_exit_with_error("error: cannot execute ", arguments[0]);
	}
	waitpid(pid, &code, 0x0);
	ft_configure_pipe(has_pipe, pipe_fds, STDIN_FILENO);
	return (WIFEXITED(code) && WEXITSTATUS(code));
}

int	main(int argc, char **argv, char **env)
{
	int	index;
	int	code;

	(void)argc;
	index = 0x0;
	code = 0x0;
	while (argv[index])
	{
		argv += index + 0x1;
		index = 0x0;
		while (argv[index] && \
			strcmp(argv[index], "|") && strcmp(argv[index], ";"))
			index++;
		if (index)
			code = ft_execute_command(argv, index, env);
	}
	return (code);
}
