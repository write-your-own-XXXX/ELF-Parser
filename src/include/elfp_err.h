/*
 * File: elfp_err.h
 *
 * Description: elfparse's error handling infra.
 *
 * License: 
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                  Version 2, December 2004
 *  
 * Copyright (C) 2019 Adwaith Gautham <adwait.gautham@gmail.com>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *          DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#ifndef _ELFP_ERR_H
#define _ELFP_ERR_H

#include <stdio.h>
#include <stdlib.h>

static void
elfp_err_warn(const char *function_name, const char *err_msg)
{
	fprintf(stderr, "%s: %s\n", function_name, err_msg);
}

static void
elfp_err_exit(const char *function_name, const char *err_msg)
{
	elfp_err_warn(function_name, err_msg);
	fprintf(stderr, "Exiting...\n");
    exit(-1);
}

#endif /* _ELFP_ERR_H */
