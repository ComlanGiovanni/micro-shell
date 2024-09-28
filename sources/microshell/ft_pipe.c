/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pipe.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:12:48 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 13:14:17 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void ft_prepare_pipe(t_micro_shell *shell)
{
	if (shell->type == TYPE_PIPE)
	{
		if (pipe(shell->pipes.pipe_fd) == -1)
		{
			ft_exit_with_fatal_error(shell);
		}
	}
}
