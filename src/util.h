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
#ifndef SIT_UTIL_H
#define SIT_UTIL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"



#define HELP(TEXT) \
	(strcmp(argv[0], "sit") ? \
	 (test_help(argc, argv) ? (usage(__FILE__), 0) : 0) : \
	 (fprintf(stderr, "%s%s\n", argv[1], TEXT), exit(EXIT_SUCCESS), 0))

#define ARG_USAGE \
	default: usage(__FILE__); break

#define ARG_BOOLEAN(flag, var) \
	case flag: var = 1; break

#define ARG_BEGIN(argv0, command) \
	{ \
		int doubledashed_ = 0, first_ = 1, first_is_opt_; \
		char *argv0_; \
		argv0 = *argv++; \
		argc--; \
		command = NULL; \
		while (argc) { \
			if (!doubledashed_) { \
				first_is_opt_ = !doubledashed_ && **argv == '-'; \
				argc += first_is_opt_; \
				argv -= first_is_opt_; \
				ARGBEGIN(argv0_)

#define ARG_END \
				ARGEND \
				if (first_ && !first_is_opt_) { \
					command = argv0_; \
					first_ = 0; \
				} \
				doubledashed_ = !strcmp(argv[-1], "--"); \
			} else { \
				argv++; \
				argc--; \
			} \
			if (argc && (doubledashed_ || **argv != '-') && command) { \
				goto ARG_NONOPT; \
			} \
		} \
	}



char *generate_uuid(void);
char *spawn_read(char *const argv[], int *success);
int test_help(int argc, char *argv[]);
void usage(char *source_file);


#endif

