/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atomic.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:44:56 by gicomlan          #+#    #+#             */
/*   Updated: 2024/10/01 10:14:43 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>		// write,fork,execve,dup2,close,chdir,STDERR_FILENO
#include <sys/wait.h>	// waitpid pid_t
#include <stdlib.h>		// malloc, free, exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>		// strcmp

#define PIPE				"|"
#define SEMICOLON			";"
#define CHANGE_DIRECTORY	"cd"

#define ERROR				"error: "
#define FATAL				"fatal"
#define CD_BAD_ARG			"cd: bad arguments"
#define CD_FAIL				"error: cd: cannot change directory to "
#define EXECVE_FAIL			"error: cannot execute "

typedef enum e_pipe_fd
{
	PIPE_INPUT = 0x0,
	PIPE_OUTPUT = 0x1,
	PIPE_FD = 0x2
} t_pipe_fd;


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
		ft_error_message(ERROR, CD_BAD_ARG, NULL);
		return (EXIT_FAILURE);
	}
	if (chdir(arg[0x1]) == -1)
	{
		ft_error_message(CD_FAIL, arg[1], NULL);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static void	ft_redirect_pipe_streams(int is_pipe, int *pipes_fds, int pipe_direction)
{
	if (is_pipe)
	{
		if (dup2(pipes_fds[pipe_direction], pipe_direction) == -1)
			ft_exit_with_error(ERROR, FATAL);
		if (close(pipes_fds[0]) == -1 || close(pipes_fds[1]) == -1)
			ft_exit_with_error(ERROR, FATAL);
	}
}

static int	ft_execute_command(char **arguments, int index, char **env)
{
	int			is_pipe;
	int			pipes_fds[PIPE_FD];
	int			code;
	pid_t		pid;

	is_pipe = arguments[index] && !strcmp(arguments[index], PIPE);
	if (!is_pipe && !strcmp(*arguments, CHANGE_DIRECTORY))
		return (ft_execute_cd(arguments, index));
	if (is_pipe && pipe(pipes_fds) == -1)
		ft_exit_with_error(ERROR, FATAL);
	pid = fork();
	if (pid == -1)
		ft_exit_with_error(ERROR, FATAL);
	if (pid == 0x0)
	{
		arguments[index] = NULL;
		ft_redirect_pipe_streams(is_pipe, pipes_fds, STDOUT_FILENO);
		if (!strcmp(*arguments, CHANGE_DIRECTORY))
			exit(ft_execute_cd(arguments, index));
		execve(arguments[0], arguments, env);
		ft_exit_with_error(EXECVE_FAIL, arguments[0x0]);
	}
	waitpid(pid, &code, 0x0);
	ft_redirect_pipe_streams(is_pipe, pipes_fds, STDIN_FILENO);
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
			strcmp(argv[index], PIPE) && strcmp(argv[index], SEMICOLON))
			index++;
		if (index)
			code = ft_execute_command(argv, index, env);
	}
	return (code);
}
