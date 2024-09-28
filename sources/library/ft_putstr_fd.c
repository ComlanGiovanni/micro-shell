/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 12:58:38 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 12:59:28 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void ft_putstr_fd(char *string, int fd)
{
	if (!string)
		string = "(null)";
	if (fd >= 0)
		write(fd, string, ft_strlen(string));
}
