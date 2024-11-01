/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   refactor-atomic.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 08:34:19 by gicomlan          #+#    #+#             */
/*   Updated: 2024/11/01 10:43:31 by gicomlan         ###   ########.fr       */
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

/**
 * @file micro-shell.c
 * @brief
 *              Check if the string is a pipe
 *
 * 	The function compares the input string to the predefined PIPE symbol
 *  and returns true if they match, false otherwise.
 *  		Déroulement de la fonction :
 * 		1. Use strcmp to compare the input string with PIPE.
 * 		2. Return true if the strings are identical.
 *
 *  Exemple :
 *      ft_is_pipe("|"); // returns true
 *
 * @param str String to be compared
 * @return bool Returns true if str is PIPE, otherwise false
 * @note ----------------------------
 * @todo ----------------------------
 */
static inline bool	ft_is_pipe(char *str)
{
	return (!(strcmp(str, PIPE)));
}

/**
 * @file micro-shell.c
 * @brief
 *              Check if the string is the 'cd' command
 *
 * 	This function checks if the input string matches the CHANGE_DIRECTORY
 *  constant ("cd") and returns true if they match.
 *  		Déroulement de la fonction :
 * 		1. Compare input string to "cd" using strcmp.
 * 		2. Return true if the strings match.
 *
 *  Exemple :
 *      ft_is_cd("cd"); // returns true
 *
 * @param str String to be checked
 * @return bool Returns true if the string is 'cd'
 * @note ----------------------------
 * @todo ----------------------------
 */
static inline bool	ft_is_cd(char *str)
{
	return (!(strcmp(str, CHANGE_DIRECTORY)));
}

/**
 * @file micro-shell.c
 * @brief
 *              Check if the string is a semicolon
 *
 * 	The function checks if the input string matches the SEMICOLON constant
 *  and returns true if they match.
 *  		Déroulement de la fonction :
 * 		1. Compare input string to ";" using strcmp.
 * 		2. Return true if the strings match.
 *
 *  Exemple :
 *      ft_is_semicolon(";"); // returns true
 *
 * @param str String to be checked
 * @return bool Returns true if the string is a semicolon
 * @note ----------------------------
 * @todo ----------------------------
 */
static inline bool	ft_is_semicolon(char *str)
{
	return (!(strcmp(str, SEMICOLON)));
}

/**
 * @file micro-shell.c
 * @brief
 *              Calculate the length of a string
 *
 * 	This function calculates and returns the length of a given string.
 *  		Déroulement de la fonction :
 * 		1. Check if the string is NULL.
 * 		2. If not, loop through each character to find the null terminator.
 * 		3. Return the difference between the start and end pointers.
 *
 *  Exemple :
 *      ft_strlen("hello"); // returns 5
 *
 * @param str Pointer to the input string
 * @return size_t Length of the string
 * @note ----------------------------
 * @todo ----------------------------
 */
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

/**
 * @file micro-shell.c
 * @brief
 *              Write a string to a file descriptor
 *
 * 	This function writes a given string to a specified file descriptor.
 *  		Déroulement de la fonction :
 * 		1. Check if the string is non-null.
 * 		2. Ensure the file descriptor is valid (>= 0).
 * 		3. Write the string using the write() system call.
 *
 *  Exemple :
 *      ft_putstr_fd("hello", 1); // prints "hello" to stdout
 *
 * @param str The string to write
 * @param fd File descriptor to which the string will be written
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
static void	ft_putstr_fd(char *str, int fd)
{
	if (str)
		if (fd >= STDIN_FILENO)
			write(fd, str, ft_strlen(str));
}

/**
 * @file micro-shell.c
 * @brief
 *              Print an error message
 *
 * 	This function prints an error message to stderr, composed of a prefix,
 *  suffix, optional additional message, and a newline.
 *  		Déroulement de la fonction :
 * 		1. Print the prefix, suffix, and additional message (if non-null).
 * 		2. Always print a newline at the end.
 *
 *  Exemple :
 *      ft_error_mssg("Error: ", "File not found", NULL, NULL); // prints error
 *
 * @param pref The prefix of the message
 * @param suff The suffix of the message
 * @param radi Additional message to print
 * @param mssg Optional message, prints if non-null
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
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

/**
 * @file micro-shell.c
 * @brief
 *              Exit the program with an error
 *
 * 	This function prints an error message and exits the program
 * 			with a failure status.
 *  		Déroulement de la fonction :
 * 		1. Print the error message using ft_error_mssg().
 * 		2. Call exit(EXIT_FAILURE) to terminate the program.
 *
 *  Exemple :
 *      ft_exit_error("Error: ", "File not found", NULL);
 *
 * @param pref The prefix of the message
 * @param suff The suffix of the message
 * @param radi Additional message
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
static void	ft_exit_error(char *pref, char *suff, char *radi)
{
	ft_error_mssg(pref, suff, radi, NULL);
	exit(EXIT_FAILURE);
}

/**
 * @file micro-shell.c
 * @brief
 *              Initialize the main structure
 *
 * 	This function initializes the t_main structure with the provided
 *  arguments (argc, argv, envp).
 *  		Déroulement de la fonction :
 * 	1. Assign argc, argv, and envp to the respective fields in the structure.
 *
 *  Exemple :
 *      ft_init_main(&main, argc, argv, envp);
 *
 * @param main Pointer to the main structure to initialize
 * @param argc Argument count
 * @param argv Argument vector
 * @param envp Environment variables
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
static void	ft_init_main(t_main *main, int argc, char **argv, char **envp)
{
	main->argc = argc;
	main->argv = argv;
	main->envp = envp;
}

/**
 * @file micro-shell.c
 * @brief
 *              Initialize the pipe structure
 *
 * 	This function initializes the t_pipe structure, setting its fields
 *  to default values (false and INVALID_FD).
 *  		Déroulement de la fonction :
 * 		1. Set is_pipe to false.
 * 		2. Set both pipe file descriptors to INVALID_FD.
 *
 *  Exemple :
 *      ft_init_pipe(&pipe);
 *
 * @param pipe Pointer to the pipe structure to initialize
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
static void	ft_init_pipe(t_pipe *pipe)
{
	pipe->is_pipe = false;
	pipe->pipes_fds[PIPE_INPUT] = INVALID_FD;
	pipe->pipes_fds[PIPE_OUTPUT] = INVALID_FD;
}

/**
 * @file micro-shell.c
 * @brief
 *              Initialize the shell structure
 *
 * 	This function initializes the t_micro structure, setting up default values for
 *  its fields and initializing the t_main and t_pipe substructures.
 *  		Déroulement de la fonction :
 * 		1. Initialize index, pid, exit_code.
 * 		2. Initialize t_pipe and t_main using their respective init functions.
 *
 *  Exemple :
 *      ft_init_shell(&shell, argc, argv, envp);
 *
 * @param shell Pointer to the shell structure to initialize
 * @param argc Argument count
 * @param argv Argument vector
 * @param envp Environment variables
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
static void	ft_init_shell(t_micro *shell, int argc, char **argv, char **envp)
{
	shell->index = ZERO;
	shell->pid = INVALID_PID;
	ft_init_pipe(&shell->pipe);
	shell->exit_code = EXIT_SUCCESS;
	ft_init_main(&shell->main, argc, argv, envp);
}

/**
 * @file micro-shell.c
 * @brief
 *              Execute the 'cd' command
 *
 * 	This function handles the execution of the 'cd' (change directory) command.
 *  		Déroulement de la fonction :
 * 		1. Check if there are the correct number of arguments for 'cd'.
 * 		2. Call chdir to change the directory.
 * 		3. Print an error message and return failure if it fails.
 *
 *  Exemple :
 *      ft_exec_cd(&shell); // attempts to change directory
 *
 * @param shell Pointer to the shell structure
 * @return int Returns EXIT_SUCCESS on success, EXIT_FAILURE on failure
 * @note ----------------------------
 * @todo ----------------------------
 */
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

/**
 * @file micro-shell.c
 * @brief
 *              Redirect pipe input/output
 *
 * 	This function handles the redirection of pipes,
 * 		duplicating file descriptors
 *  	as needed for input/output.
 * 	It also closes the original pipe file descriptors.
 *
 *  		Déroulement de la fonction :
 * 		1. Check if there is a pipe in the shell structure.
 * 		2. Use dup2 to redirect the file descriptor.
 * 		3. Close the original pipe file descriptors.
 *
 *  Exemple :
 *      ft_redirect_pipe(&shell, STDOUT_FILENO); // redirect pipe output
 *
 * @param shell Pointer to the shell structure
 * @param direction Direction of redirection (STDIN_FILENO or STDOUT_FILENO)
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
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

/**
 * @file micro-shell.c
 * @brief
 *              Check if current argument is a pipe
 *
 * 	This function checks if the current argument
 * 		in the shell structure is a pipe.
 *  		Déroulement de la fonction :
 * 	1. Set the pipe flag in the structure to true if the argument is a pipe.
 *
 *  Exemple :
 *      ft_check_if_pipe(&shell);
 *
 * @param shell Pointer to the shell structure
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
static void	ft_check_if_pipe(t_micro *shell)
{
	shell->pipe.is_pipe = (shell->main.argv[shell->index] && \
		ft_is_pipe(shell->main.argv[shell->index]));
}

/**
 * @file micro-shell.c
 * @brief
 *              Execute a child process
 *
 * 	This function executes a command in a child process,
 * 		handling pipes, 'cd', and execve.
 *
 *  		Déroulement de la fonction :
 * 		1. Check if the process is a child (pid == 0).
 * 		2. Redirect pipes if necessary.
 * 		3. Handle 'cd' command or call execve to execute the command.
 *
 *  Exemple :
 *      ft_exec_child(&shell); // execute child process
 *
 * @param shell Pointer to the shell structure
 * @return void
 * @note ----------------------------
 * @todo ----------------------------
 */
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

/**
 * @file microshell.c
 * @brief
 *              Execute a command
 *
 * 	This function executes a command, handling pipes and forks.
 * 					It waits for the
 *  child process to complete before returning the exit code.
 *
 *  		Déroulement de la fonction :
 * 		1. Check if the current argument is a pipe.
 * 		2. Handle 'cd' command separately if no pipes.
 * 		3. Create pipes and fork a child process.
 * 		4. Execute the child process and wait for its completion.
 *
 *  Exemple :
 *      ft_exec_cmd(&shell); // execute command with or without pipes
 *
 * @param shell Pointer to the shell structure
 * @return int Returns the exit status of the command
 * @note ----------------------------
 * @todo ----------------------------
 */
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

/**
 * @file microshell.c
 * @brief
 *              Main function of the microshell program
 *
 * 	This is the main function that initializes the shell structure and loops
 *  through the arguments to execute commands.
 *  		Déroulement de la fonction :
 * 		1. Initialize the shell structure.
 * 		2. Loop through the arguments and execute commands.
 * 		3. Handle pipes and semicolons.
 *
 *  Exemple :
 *      main(argc, argv, envp);
 *
 * @param argc Number of arguments
 * @param argv Argument array
 * @param envp Environment variables
 * @return int Returns the exit code of the last command executed
 * @note ----------------------------
 * @todo ----------------------------
 */
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
