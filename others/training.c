/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   training.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 23:43:22 by gicomlan          #+#    #+#             */
/*   Updated: 2024/10/03 00:32:27 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE "|"
#define SEMICOLON ";"
#define CHANGE_DIRECTORY "cd"
#define CD "cd: "
#define ERROR "error: "
#define FATAL "fatal: "
#define CD_BAD_ARG "bad arguments"
#define CD_FAIL "cannot change directory to"
#define EXECVE_FAIL "cannot execute"


typedef enum e_pipes_fd t_pipes_fd;
typedef struct s_micro 	t_micro;
typedef struct s_pipe	t_pipe;
typedef struct s_main	t_main;

enum e_pipes_fd
{
	INVALID_FD = -1,
	PIPE_INPUT,
	PIPE_OUTPUT,
	FD_NUMBER
};

struct s_pipe
{
	bool	is_pipe;
	int		pipes_fd[FD_NUMBER];
};

struct s_main
{
	int		argc;
	char	**argv;
	char	**envp;
};

struct s_micro
{
	t_main	main;
	t_pipe	pipe;
	pid_t	pid;
	int		index;
	int		exit_code;
};

static size_t ft_strlen(char *str)
{
	const	char *end_str;

	if (!str)
		return (0x0);
	end_str = str;
	while (*end_str)
		end_str++;
	return (end_str - str);
}

static void ft_putstr_fd(char *str, int fd)
{
	if (str)
		if (fd >= 0x0)
			write(fd, str, ft_strlen(str));
}

static void ft_error_mssg(char *pref, char *radi, char *suff, char *mssg)//
{
	if (pref)
		ft_putstr_fd(pref, STDERR_FILENO);
	if (radi)//
		ft_putstr_fd(radi, STDERR_FILENO);
	if (suff)//
		ft_putstr_fd(suff, STDERR_FILENO);
	if (mssg)
		ft_putstr_fd(mssg, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

static void ft_exit_error(char *pref, char *radi, char *suff)
{
	ft_error_mssg(pref, radi, suff, NULL);
	exit(EXIT_FAILURE);
}

static inline bool ft_is_pipe(char *str)
{
	return(!(strcmp(str, PIPE)));
}


static inline bool ft_is_cd(char *str)
{
	return(!(strcmp(str, CHANGE_DIRECTORY)));
}

static inline bool ft_is_semicolon(char *str)
{
	return(!(strcmp(str, SEMICOLON)));
}

static void ft_init_main(t_main *main, int argc, char **argv, char **envp)
{
	main->argc = argc;
	main->argv = argv;
	main->envp = envp;
}

static void	ft_init_pipe(t_pipe *pipe)//
{
	pipe->is_pipe = false;//
	pipe->pipes_fd[PIPE_INPUT] = INVALID_FD;//
	pipe->pipes_fd[PIPE_OUTPUT] = INVALID_FD;//
}

static void ft_init_shell(t_micro *shell, int argc, char **argv, char **envp)
{
	shell->index = 0x0;
	shell->pid	= -(0x1);
	shell->exit_code = EXIT_SUCCESS;
	ft_init_pipe(&shell->pipe);//
	ft_init_main(&shell->main, argc, argv, envp);
}

static void ft_check_is_pipe(t_micro *shell)
{
	shell->pipe.is_pipe = (shell->main.argv[shell->index] && \
		ft_is_pipe(shell->main.argv[shell->index]));//
}

static int ft_exec_cd(t_micro *shell)
{
	if (shell->index != 0x2)
	{
		ft_error_mssg(ERROR, CD, CD_BAD_ARG, NULL);
		return (EXIT_FAILURE);//
	}
	if ((chdir(shell->main.argv[0x1])) == -(0x1))
	{
		ft_error_mssg(ERROR, CD, CD_FAIL, shell->main.argv[0x1]);
		return (EXIT_FAILURE);//
	}
	return (EXIT_SUCCESS);
}

static void ft_redirect_pipe(t_micro *shell, int direction)
{
	if (shell->pipe.is_pipe)
	{
		if (dup2(shell->pipe.pipes_fd[direction], direction) == -(0x1))//
			ft_exit_error(ERROR, FATAL, NULL);//
		if ((close(shell->pipe.pipes_fd[PIPE_INPUT]) == -(0x1)) || \
			close(shell->pipe.pipes_fd[PIPE_OUTPUT]) == -(0x1))//
			ft_exit_error(ERROR, FATAL, NULL);
	}
}

static	void ft_exec_child(t_micro *shell)
{
	if (shell->pid == 0x0)
	{
		shell->main.argv[shell->index] = NULL;//
		ft_redirect_pipe(shell, STDOUT_FILENO);//
		if (ft_is_cd(*shell->main.argv))//
			exit(ft_exec_cd(shell));//
		execve(shell->main.argv[0x0],shell->main.argv,shell->main.envp);
		ft_exit_error(ERROR, EXECVE_FAIL, shell->main.argv[0x0]);
	}
}

static int ft_exec_cmd(t_micro *shell)
{
	ft_check_is_pipe(shell);
	if (!shell->pipe.is_pipe && ft_is_cd(*shell->main.argv))//
		return(ft_exec_cd(shell));
	if (shell->pipe.is_pipe && (pipe(shell->pipe.pipes_fd) == -(0x1)))
		ft_exit_error(ERROR, FATAL, NULL);
		//
	shell->pid = fork();
	if (shell->pid == -(0x1))
		ft_exit_error(ERROR, FATAL, NULL);
	ft_exec_child(shell);
	waitpid(shell->pid,&shell->exit_code, 0);
	ft_redirect_pipe(shell, STDIN_FILENO);
	return (WIFEXITED(shell->exit_code) && WEXITSTATUS(shell->exit_code));
}

int main(int argc, char **argv, char **envp)
{
	t_micro shell;

	ft_init_shell(&shell, argc, argv, envp);
	while (shell.main.argv[shell.index] != NULL)//
	{
		shell.main.argv += shell.index + (0x1);
		shell.index = 0x0;
		while (shell.main.argv[shell.index] && \
			!ft_is_pipe(shell.main.argv[shell.index]) && \
				!ft_is_semicolon(shell.main.argv[shell.index]))
			shell.index++;
		if (shell.index)
			shell.exit_code = ft_exec_cmd(&shell);
	}
	return (shell.exit_code);
}
