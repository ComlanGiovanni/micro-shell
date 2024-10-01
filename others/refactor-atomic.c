/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   refactor-atomic.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:44:56 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/30 20:32:32 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>		// write,fork,execve,dup2,close,chdir,STDERR_FILENO
#include <sys/wait.h>	// waitpid pid_t
#include <stdlib.h>		// malloc, free, exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>		// strcmp
#include <stdbool.h>	// bool, true, false

typedef enum e_pipe_fd
{
	PIPE_INPUT = 0,
	PIPE_OUTPUT = 1
} t_pipe_fd;

typedef struct s_micro
{
	char	**env;
	char	**argv;
	int		argc;
	int		index;
	int		exit_code;
	pid_t 	pid;
	int		pipe_fd[2];
	int		is_pipe;
} t_micro;

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

static int	ft_execute_cd(t_micro *shell)
{
	if (shell->index != 0x2)
	{
		ft_error_message("error: ", "cd: bad arguments", NULL);
		return (EXIT_FAILURE);
	}
	if (chdir(shell->argv[0x1]) == -1)
	{
		ft_error_message("error: cd: cannot change directory to ", \
			shell->argv[1], NULL);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static void	ft_redirect_pipe_streams(t_micro *shell, int pipe_direction)
{
	if (shell->is_pipe)
	{
		if (dup2(shell->pipe_fd[pipe_direction], pipe_direction) == -1)
			ft_exit_with_error("error: ", "fatal");
		if (close(shell->pipe_fd[0]) == -1 || close(shell->pipe_fd[1]) == -1)
			ft_exit_with_error("error: ", "fatal");
	}
}
//shell->argv[shell->index] = NULL;
static int	ft_execute_command(t_micro *shell)
{
	shell->is_pipe = shell->argv[shell->index] && \
		!strcmp(shell->argv[shell->index], "|");
	if (!shell->is_pipe && !strcmp(*shell->argv, "cd"))
		return (ft_execute_cd(shell));
	if (shell->is_pipe && pipe(shell->pipe_fd) == -1)
		ft_exit_with_error("error: ", "fatal");
	shell->pid = fork();
	if (shell->pid == -1)
		ft_exit_with_error("error: ", "fatal");
	if (shell->pid == 0x0)
	{
		shell->argv[shell->index] = NULL;
		ft_redirect_pipe_streams(shell, STDOUT_FILENO);
		if (!strcmp(*shell->argv, "cd"))
			exit(ft_execute_cd(shell));
		execve(shell->argv[0], shell->argv, shell->env);
		ft_exit_with_error("error: cannot execute ", shell->argv[0]);
	}
	waitpid(shell->pid, &shell->exit_code, 0x0);
	ft_redirect_pipe_streams(shell, STDIN_FILENO);
	return (WIFEXITED(shell->exit_code) && WEXITSTATUS(shell->exit_code));
}

static void	ft_init_shell(t_micro *shell, int argc, char **argv, char **envp)
{
	shell->env = envp;
	shell->argv = argv;
	shell->argc = argc;
	shell->index = 0x0;
	shell->exit_code = EXIT_SUCCESS;
	shell->pipe_fd[PIPE_INPUT] = -1;
	shell->pipe_fd[PIPE_OUTPUT] = -1;
	shell->is_pipe = false;
}

int main(int argc, char **argv, char **envp)
{
    t_micro shell;
    int i = 1;
    ft_init_shell(&shell, argc, argv, envp);
    while (i < argc)
    {
        shell.argv = &argv[i];
        shell.index = 0;
        while (i + shell.index < argc &&
               strcmp(argv[i + shell.index], ";") != 0 &&
               strcmp(argv[i + shell.index], "|") != 0)
            shell.index++;
        if (i + shell.index < argc)
            shell.argv[shell.index] = argv[i + shell.index];
        else
            shell.argv[shell.index] = NULL;
        shell.exit_code = ft_execute_command(&shell);
        i = i + shell.index + 1;
    }
    return (shell.exit_code);
}


