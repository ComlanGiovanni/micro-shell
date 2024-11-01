/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   training.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 08:34:19 by gicomlan          #+#    #+#             */
/*   Updated: 2024/11/01 10:43:28 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>		// strcmp
#include <stdbool.h>	// bool
#include <sys/wait.h>	// waitpid pid_t WIFEXITED WEXITSTATUS
#include <stdlib.h>		// malloc, free, exit, EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h>		// write,fork,execve,dup2,close,chdir,STDERR_FILENO

// #define FT_IS_PIPE(str) (strcmp((str), PIPE) == 0)
// #define FT_IS_CD(str) (strcmp((str), CHANGE_DIRECTORY) == 0)
// #define FT_IS_SEMICOLON(str) (strcmp((str), SEMICOLON) == 0)


#define PIPE				"|"
#define SEMICOLON			";"
#define CHANGE_DIRECTORY	"cd"
#define CD					"cd: "
#define FATAL				"fatal"
#define ERROR				"error: "
#define CD_BAD_ARG			"bad arguments"
#define EXECVE_FAIL			"cannot execute "
#define CD_FAIL				"cannot change directory to "

#define ZERO				(0x0)
#define NEXT_CMD			(0x1)
#define SYSCALL_ERROR		-0x1
#define INDEX_FIRST_ARG		(0x0)
#define INDEX_SECOND_ARG	(0x1)
#define CD_EXPECTED_ARGS	(0x2)

typedef struct s_main	t_main;
typedef struct s_micro	t_micro;
typedef struct s_pipe	t_pipe;

typedef enum e_pid		t_pid;
typedef enum e_pipes_fd	t_pipes_fds;

enum e_pid
{
	INVALID_PID = -(0x1),
	CHILD_PID = ZERO
};

enum e_pipes_fd
{
	INVALID_FD = -(0x1),
	PIPE_INPUT,
	PIPE_OUTPUT,
	FD_NUMBER
};

struct s_main
{
	int		argc;
	char	**argv;
	char	**envp;
};

struct s_pipe
{
	bool	is_pipe;
	int		pipes_fds[FD_NUMBER];
};

struct s_micro
{
	pid_t	pid;
	t_main	main;
	t_pipe	pipe;
	int		index;
	int		exit_code;
};


static inline bool	ft_is_pipe(char *str)
{
	return (!(strcmp(str, PIPE)));
}

static inline bool	ft_is_cd(char *str)
{
	return (!(strcmp(str, CHANGE_DIRECTORY)));
}

static inline bool	ft_is_semicolon(char *str)
{
	return (!(strcmp(str, SEMICOLON)));
}

static size_t	ft_strlen(char *str)
{
	const char	*end_str;

	if (!str)
		return (ZERO);
	end_str = str;
	while (*end_str)
		end_str++;
	return (end_str - str);
}

static void	ft_putstr_fd(char *str, int fd)
{
	if (str)
		if (fd >= STDIN_FILENO)
			write(fd, str, ft_strlen(str));
}

static void	ft_error_mssg(char *pref, char *suff, char *radi, char *mssg)
{
	if (pref)
		ft_putstr_fd(pref, STDERR_FILENO);
	if (suff)
		ft_putstr_fd(suff, STDERR_FILENO);
	if (radi)
		ft_putstr_fd(radi, STDERR_FILENO);
	if (mssg)
		ft_putstr_fd(mssg, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

static void	ft_exit_error(char *pref, char *suff, char *radi)
{
	ft_error_mssg(pref, suff, radi, NULL);
	exit(EXIT_FAILURE);
}

static void	ft_init_main(t_main *main, int argc, char **argv, char **envp)
{
	main->argc = argc;
	main->argv = argv;
	main->envp = envp;
}

static void	ft_init_pipe(t_pipe *pipe)
{
	pipe->is_pipe = false;
	pipe->pipes_fds[PIPE_INPUT] = INVALID_FD;
	pipe->pipes_fds[PIPE_OUTPUT] = INVALID_FD;
}

static void	ft_init_shell(t_micro *shell, int argc, char **argv, char **envp)
{
	shell->index = ZERO;
	shell->pid = INVALID_PID;
	ft_init_pipe(&shell->pipe);
	shell->exit_code = EXIT_SUCCESS;
	ft_init_main(&shell->main, argc, argv, envp);
}

static int	ft_exec_cd(t_micro *shell)
{
	if (shell->index != CD_EXPECTED_ARGS)
	{
		ft_error_mssg(ERROR, CD, CD_BAD_ARG, NULL);
		return (EXIT_FAILURE);
	}
	if (chdir(shell->main.argv[INDEX_SECOND_ARG]) == SYSCALL_ERROR)
	{
		ft_error_mssg(ERROR, CD, CD_FAIL, shell->main.argv[INDEX_SECOND_ARG]);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static void	ft_redirect_pipe(t_micro *shell, int direction)
{
	if (shell->pipe.is_pipe)
	{
		if (dup2(shell->pipe.pipes_fds[direction], direction) == INVALID_FD)
			ft_exit_error(ERROR, FATAL, NULL);
		if ((close(shell->pipe.pipes_fds[PIPE_INPUT]) == SYSCALL_ERROR) || \
			close(shell->pipe.pipes_fds[PIPE_OUTPUT]) == SYSCALL_ERROR)
			ft_exit_error(ERROR, FATAL, NULL);
	}
}

static void	ft_check_if_pipe(t_micro *shell)
{
	shell->pipe.is_pipe = (shell->main.argv[shell->index] && \
		ft_is_pipe(shell->main.argv[shell->index]));
}

static void	ft_exec_child(t_micro *shell)
{
	if (shell->pid == CHILD_PID)
	{
		shell->main.argv[shell->index] = NULL;
		ft_redirect_pipe(shell, STDOUT_FILENO);
		if (ft_is_cd(*shell->main.argv))
			exit(ft_exec_cd(shell));
		execve(shell->main.argv[INDEX_FIRST_ARG], \
			shell->main.argv, shell->main.envp);
		ft_exit_error(ERROR, EXECVE_FAIL, shell->main.argv[INDEX_FIRST_ARG]);
	}
}

static int	ft_exec_cmd(t_micro *shell)
{
	ft_check_if_pipe(shell);
	if (!shell->pipe.is_pipe && ft_is_cd(*shell->main.argv))
		return (ft_exec_cd(shell));
	if (shell->pipe.is_pipe && pipe(shell->pipe.pipes_fds) == INVALID_FD)
		ft_exit_error(ERROR, FATAL, NULL);
	shell->pid = fork();
	if (shell->pid == INVALID_PID)
		ft_exit_error(ERROR, FATAL, NULL);
	ft_exec_child(shell);
	waitpid(shell->pid, &shell->exit_code, ZERO);
	ft_redirect_pipe(shell, STDIN_FILENO);
	return (WIFEXITED(shell->exit_code) && WEXITSTATUS(shell->exit_code));
}

int	main(int argc, char **argv, char **envp)
{
	t_micro	shell;

	ft_init_shell(&shell, argc, argv, envp);
	while (shell.main.argv[shell.index] != NULL)
	{
		shell.main.argv += shell.index + NEXT_CMD;
		shell.index = ZERO;
		while (shell.main.argv[shell.index] && \
			!ft_is_pipe(shell.main.argv[shell.index]) && \
				!ft_is_semicolon(shell.main.argv[shell.index]))
			shell.index++;
		if (shell.index)
			shell.exit_code = ft_exec_cmd(&shell);
	}
	return (shell.exit_code);
}
