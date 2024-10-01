/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   refactor-atomic.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 10:15:32 by gicomlan          #+#    #+#             */
/*   Updated: 2024/10/01 16:22:22 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stddef.h>	// NULL
#include <string.h>		// strcmp
#include <stdbool.h>	// bool
#include <sys/wait.h>	// waitpid pid_t WIFEXITED WEXITSTATUS
#include <stdlib.h>		// malloc, free, exit, EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h>		// write,fork,execve,dup2,close,chdir,STDERR_FILENO

#define PIPE				"|"
#define SEMICOLON			";"
#define CHANGE_DIRECTORY	"cd"
#define FATAL				"fatal"
#define ERROR				"error: "
#define CD					"cd: "
#define CD_BAD_ARG			"bad arguments"
#define EXECVE_FAIL			"cannot execute"
#define CD_FAIL				"cannot change directory to "

typedef enum e_pipes_fd
{
	PIPE_INPUT = 0x0,
	PIPE_OUTPUT = 0x1,
	PIPE_FD	= 0x2
}	t_pipes_fds;

// typedef struct s_main
// {
// 	char	**argv;
// 	char	**envp;
// 	int	 argc;
// }	t_main;

// typedef struct s_micro_shell
// {
// 	t_main			main;
// 	pid_t			pid;
// 	int				index;
// 	bool			is_pipe;
// 	int				exit_code;
// 	int				pipes_fds[PIPE_FD];
// }	t_micro_shell;

size_t	ft_strlen(char *str)
{
	const char	*end_str;

	if (!str)
		return (0x0);
	end_str = str;
	while (*end_str)
		end_str++;
	return (end_str - str);
}

static void	ft_putstr_fd(char *str, int fd)
{
	if (str)
		if (fd >= 0x0)
			write(fd, str, ft_strlen(str));
}

static void	ft_error_msg(char *pref, char *suff, char *radi, char *msg)
{
	if (pref)
		ft_putstr_fd(pref, STDERR_FILENO);
	if (suff)
		ft_putstr_fd(suff, STDERR_FILENO);
	if (radi)
		ft_putstr_fd(radi, STDERR_FILENO);
	if (msg)
		ft_putstr_fd(msg, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

static void	ft_exit_error(char *pref, char *suff, char *radi)
{
	ft_error_msg(pref, suff, radi, NULL);
	exit(EXIT_FAILURE);
}

static void	ft_redirect_pipe(bool is_pipe, int *pipes_fd, int direction)
{
	if (is_pipe)
	{
		if (dup2(pipes_fd[direction], direction) == -1)
			ft_exit_error(ERROR, FATAL, NULL);
		if ((close(pipes_fd[PIPE_INPUT]) == -(0x1)) || \
			(close(pipes_fd[PIPE_OUTPUT]) == -(0x1)))
			ft_exit_error(ERROR, FATAL, NULL);
	}
}

static int	ft_exec_cd(char **argv, int index)
{
	if (index != 0x2)
	{
		ft_error_msg(ERROR, CD, CD_BAD_ARG, NULL);
		return (EXIT_FAILURE);
	}
	if (chdir(argv[0x1]) == -0x1)
	{
		ft_error_msg(ERROR, CD, CD_FAIL, argv[0x1]);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static bool ft_is_cd(char *str)
{
	return ((strcmp(str, CHANGE_DIRECTORY)));
}

static bool ft_is_pipe(char *str)
{
	return ((strcmp(str, PIPE)));
}

static bool ft_is_semicolon(char *str)
{
	return ((strcmp(str, SEMICOLON)));
}

static int	ft_exec_cmd(char **argv, int index, char **envp)
{
	pid_t	pid;
	bool	is_pipe;
	int		exit_code;
	int		pipes_fds[PIPE_FD];

	is_pipe = (argv[index] && !ft_is_pipe(argv[index]));
	if (!is_pipe && !ft_is_cd(*argv))
		return (ft_exec_cd(argv, index));
	if (is_pipe && pipe(pipes_fds) == -1)
		ft_exit_error(ERROR, FATAL, NULL);
	pid = fork();
	if (pid == -1)
		ft_exit_error(ERROR, FATAL, NULL);
	if (pid == 0x0)
	{
		argv[index] = NULL;
		ft_redirect_pipe(is_pipe, pipes_fds, STDOUT_FILENO);
		if (!ft_is_cd(*argv))
			exit(ft_exec_cd(argv, index));
		execve(argv[0x0], argv, envp);
		ft_exit_error(ERROR, EXECVE_FAIL, argv[0x0]);
	}
	waitpid(pid, &exit_code, 0x0);
	ft_redirect_pipe(is_pipe, pipes_fds, STDIN_FILENO);
	return (WIFEXITED(exit_code) && WEXITSTATUS(exit_code));
}

int	main(int argc, char **argv, char **envp)
{
	int	index;
	int	exit_code;

	(void)argc;
	index = 0x0;
	exit_code = EXIT_SUCCESS;
	while (argv[index] != NULL)
	{
		argv += index + (0x1);
		index = (0x0);
		while (argv[index] && \
			ft_is_pipe(argv[index]) && \
				ft_is_semicolon(argv[index]))
			index++;
		if (index)
			exit_code = ft_exec_cmd(argv, index, envp);
	}
	return (exit_code);
}
