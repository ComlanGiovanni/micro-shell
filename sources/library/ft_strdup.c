/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gicomlan <gicomlan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 12:58:43 by gicomlan          #+#    #+#             */
/*   Updated: 2024/09/28 12:59:37 by gicomlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

char	*ft_strdup(char *source)
{
	size_t	length_source;
	char	*duplicate_string;

	duplicate_string = NULL;
	length_source = ft_strlen(source);
	duplicate_string = (char *)malloc(sizeof(char) * (length_source + 0x1));
	if (duplicate_string == NULL)
		return (NULL);
	ft_strcpy(duplicate_string, source);
	return (duplicate_string);
}
