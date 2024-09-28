/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_microshell_enums.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 12:47:42 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 12:55:34 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MICRO_SHELL_ENUMS_H
# define FT_MICRO_SHELL_ENUMS_H

typedef enum e_command_type
{
	TYPE_NONE,
	TYPE_PIPE,
	TYPE_SEMICOLON,
	TYPE_CD
}   t_command_type;

typedef enum e_pipe_fd
{
	PIPE_INPUT = 0,
	PIPE_OUTPUT = 1
}   t_pipe_fd;

#endif //!FT_MICRO_SHELL_ENUMS_H
