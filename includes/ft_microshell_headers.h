/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_microshell_headers.h                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 12:47:44 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 12:53:24 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MICRO_SHELL_HEADERS_H
# define FT_MICRO_SHELL_HEADERS_H

#include <unistd.h>	 // write, fork, execve, dup2, close, chdir, STDERR_FILENO
#include <sys/wait.h>   // waitpid pid_t inside types
//#include <sys/types.h> //pid_t
#include <stdlib.h>	 // malloc, free, exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>	 // strcmp
#include <stdbool.h>	// bool, true, false
#include <stdio.h>	  // printf


#endif //!FT_MICRO_SHELL_HEADERS_H
