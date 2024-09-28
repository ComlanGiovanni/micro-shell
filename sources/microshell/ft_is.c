/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_is.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:04:50 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:09:36 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

bool ft_is_pipe(t_micro_shell *shell)
{
	if (shell->index < shell->main_vars.argc &&
		strcmp(shell->main_vars.argv[shell->index], PIPE) == 0)
		return (true);
	return (false);
}

bool ft_is_semicolon(t_micro_shell *shell)
{
	if (shell->index < shell->main_vars.argc &&
		strcmp(shell->main_vars.argv[shell->index], SEMICOLON) == 0)
		return (true);
	return (false);
}

bool ft_is_cd(t_micro_shell *shell)
{
	if (shell->arguments[0] && strcmp(shell->arguments[0], CHANGE_DIRECTORY) == 0)
		return (true);
	return (false);
}
