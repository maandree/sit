/**
 * MIT/X Consortium License
 * 
 * Copyright © 2015  Mattias Andrée <maandree@member.fsf.org>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"



char *argv0;


int
main(int argc, char *argv[])
{
	char *command;
	int opt_critical = 0, opt_print_id = 0;
	char *ticket_id = NULL;
	size_t milestone_count = 0;
	char **milestones = NULL;

	HELP("Add, remove or list milestones an issue is listed under");

	ARG_BEGIN(argv0, command) {
		ARG_USAGE;
		ARG_BOOLEAN('c', opt_critical);
		ARG_BOOLEAN('p', opt_print_id);
	ARG_NONOPT:
		if (!ticket_id) {
			ticket_id = *argv;
		} else {
			if (!milestones) {
				milestones = malloc(argc * sizeof(char*));
				if (!milestones) {
					perror(argv0);
					return EXIT_FAILURE;
				}
			}
			milestones[milestone_count++] = *argv;
		}
		break;
	} ARG_END;

	return EXIT_SUCCESS;
}

