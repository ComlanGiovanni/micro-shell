/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfischof <pfischof@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 10:15:32 by gicomlan          #+#    #+#             */
/*   Updated: 2024/10/01 17:05:22 by pfischof         ###   ########.fr       */
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

#define INVALID_FD			-1
#define PIPES_NUMBER		2

typedef enum e_pipes_fd
{
	PIPE_INPUT,
	PIPE_OUTPUT,
}	t_pipes_fds;

typedef struct s_main
{
	char	**argv;
	char	**envp;
	int		 argc;
}	t_main;

typedef struct s_micro
{
	t_main			main;
	pid_t			pid;
	int				index;
	bool			is_pipe;
	int				exit_code;
	int				pipes_fds[PIPES_NUMBER];
}	t_micro;

void	ft_init_main(t_main *main, int argc, char **argv, char **envp)
{
	main->argc = argc;
	main->argv = argv;
	main->envp = envp;
}

void	ft_init_micro(int argc, char **argv, char **envp, t_micro *shell)
{
	ft_init_main(&shell->main, argc, argv, envp);
	shell->pid = getpid();
	shell->index = 0;
	shell->is_pipe = false;
	shell->exit_code = EXIT_SUCCESS;
	shell->pipes_fds[PIPE_INPUT] = INVALID_FD;
	shell->pipes_fds[PIPE_OUTPUT] = INVALID_FD;
}

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

static void	ft_redirect_pipe(t_micro *shell, int direction)
{
	if (shell->is_pipe)
	{
		if (dup2(shell->pipes_fds[direction], direction) == -1)
			ft_exit_error(ERROR, FATAL, NULL);
		if ((close(shell->pipes_fds[PIPE_INPUT]) == -(0x1)) || \
			(close(shell->pipes_fds[PIPE_OUTPUT]) == -(0x1)))
			ft_exit_error(ERROR, FATAL, NULL);
	}
}

static int	ft_exec_cd(t_micro *shell)
{
	if (shell->index != 0x2)
	{
		ft_error_msg(ERROR, CD, CD_BAD_ARG, NULL);
		return (EXIT_FAILURE);
	}
	if (chdir(shell->main.argv[0x1]) == -0x1)
	{
		ft_error_msg(ERROR, CD, CD_FAIL, shell->main.argv[0x1]);
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

static int	ft_exec_cmd(t_micro *shell)
{
	shell->is_pipe = (shell->main.argv[shell->index] && !ft_is_pipe(shell->main.argv[shell->index]));
	if (!shell->is_pipe && !ft_is_cd(*shell->main.argv))
		return (ft_exec_cd(shell));
	if (shell->is_pipe && pipe(shell->pipes_fds) == -1)
		ft_exit_error(ERROR, FATAL, NULL);
	shell->pid = fork();
	if (shell->pid == -1)
		ft_exit_error(ERROR, FATAL, NULL);
	if (shell->pid == 0x0)
	{
		shell->main.argv[shell->index] = NULL;
		ft_redirect_pipe(shell, STDOUT_FILENO);
		if (!ft_is_cd(*shell->main.argv))
			exit(ft_exec_cd(shell));
		execve(shell->main.argv[0x0], shell->main.argv, shell->main.envp);
		ft_exit_error(ERROR, EXECVE_FAIL, shell->main.argv[0x0]);
	}
	waitpid(shell->pid, &shell->exit_code, 0x0);
	ft_redirect_pipe(shell, STDIN_FILENO);
	return (WIFEXITED(shell->exit_code) && WEXITSTATUS(shell->exit_code));
}

int	main(int argc, char **argv, char **envp)
{
	t_micro	shell;

	ft_init_micro(argc, argv, envp, &shell);
	while (shell.main.argv[shell.index] != NULL)
	{
		shell.main.argv += shell.index + (0x1);
		shell.index = (0x0);
		while (shell.main.argv[shell.index] && \
			ft_is_pipe(shell.main.argv[shell.index]) && \
				ft_is_semicolon(shell.main.argv[shell.index]))
			shell.index++;
		if (shell.index)
			shell.exit_code = ft_exec_cmd(&shell);
	}
	return (shell.exit_code);
}
