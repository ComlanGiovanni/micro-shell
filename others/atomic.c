/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atomic.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 10:15:32 by gicomlan          #+#    #+#             */
/*   Updated: 2024/10/01 15:49:44 by gicomlan         ###   ########.fr       */
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

static int	ft_exec_cmd(char **argv, int index, char **envp)
{
	pid_t	pid;
	bool	is_pipe;
	int		exit_code;
	int		pipes_fds[PIPE_FD];

	is_pipe = (argv[index] && !strcmp(argv[index], PIPE));
	if (!is_pipe && !(strcmp(*argv, CHANGE_DIRECTORY)))
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
		if (!(strcmp(*argv, CHANGE_DIRECTORY)))
			exit(ft_exec_cd(argv, index));
		execve(argv[0x0], argv, envp);
		ft_exit_error(ERROR, EXECVE_FAIL, argv[0x0]);
	}
	waitpid(pid, &exit_code, 0x0);
	ft_redirect_pipe(is_pipe, pipes_fds, STDIN_FILENO);
	return (WIFEXITED(exit_code) && WEXITSTATUS(exit_code));
}
/**
 * @brief
 *
 * clang -Wall -Werror -Wextra -o microshell atomic.c &&
 * valgrind --leak-check=full --show-leak-kinds=all
 * --track-fds=yes --show-reachable=yes
 * --track-origins=yes ./microshell /bin/ls "|"
 * /usr/bin/grep microshell ";"
 * /bin/echo i love my microshell && ./microshell
 * /bin/cat atomic.c | /bin/grep a | tr 'a' 'o' |
 * /bin/grep error  && ./microshell /bin/cat atomic.c
 * | /bin/grep return | cd change_directory/ && cd
 * ../ && echo "back to previous folder" &&
 * ./microshell cd change_directory | echo $? &&
 * ./microshell /bin/mkdir change_dir | ls |
 * /bin/rmdir change_directory | pwd | date &&
 * ./microshell /bin/mkdir change_directory && ls
 * && ./microshell cd truct && ./microshell cd lol mdr
 *
 * @param argc
 * @param argv
 * @param envp
 * @return int
 */
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
			strcmp(argv[index], PIPE) && \
				strcmp(argv[index], SEMICOLON))
			index++;
		if (index)
			exit_code = ft_exec_cmd(argv, index, envp);
	}
	return (exit_code);
}
