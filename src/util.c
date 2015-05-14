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
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <uuid.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>



extern char *argv0;



char *
generate_uuid(void)
{
	uuid_t raw;
	char *str;

	str = malloc(37 * sizeof(char));
	if (!str)
		return NULL;

	uuid_generate_time(raw);
	uuid_unparse_lower(raw, str);
	return str;
}


char *
spawn_read(char *const argv[], int *success)
{
	int pipe_rw[] = { -1, -1 };
	pid_t pid, reaped = -1;
	int r, saved_errno, status;
	ssize_t got;
	size_t size = 0, ptr = 0;
	char *buf = NULL;
	char *new;

	*success = 0;

	r = pipe(pipe_rw);
	if (r)
		goto fail;

	pid = fork();
	if (pid < 0)
		goto fail;

	if (!pid) {
		close(pipe_rw[0]);
		if (pipe_rw[1] != STDOUT_FILENO) {
			close(STDOUT_FILENO);
			dup2(pipe_rw[1], STDOUT_FILENO);
			close(pipe_rw[1]);
		}
		execvp(*argv, argv);
		perror(argv0);
		exit(EXIT_FAILURE);
	}

	close(pipe_rw[1]);
	for (;;) {
		if (size - ptr < 1024) {
			size = (size ? size : 2048) << 1;
			new = realloc(buf, size * sizeof(char));
			if (!new)
				goto fail;
			buf = new;
		}
		got = read(pipe_rw[0], buf + ptr, (size - ptr) * sizeof(char));
		if (got < 0) {
			if (errno == EINTR)
				continue;
			else
				goto fail;
		} else if (!got) {
			break;
		} else {
			ptr += (size_t)got;
		}
	}

	while (reaped != pid) {
		reaped = waitpid(pid, &status, 0);
		if (reaped < 0) {
			if (errno == EINTR)
				continue;
			else
				goto fail;
		}
	}

	new = realloc(buf, (ptr + 1) * sizeof(char));
	if (!new)
		goto fail;
	buf = new;
	buf[ptr] = 0;

	*success = (status == 0);
	return buf;
fail:
	saved_errno = errno;
	if (pipe_rw[0] >= 0)
		close(pipe_rw[0]);
	if (pipe_rw[1] >= 0)
		close(pipe_rw[1]);
	free(buf);
	errno = saved_errno;
	return NULL;
}


int test_help(int argc, char *argv[])
{
	int i;
	for (i = 1; i < argc; i++)
		if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "help"))
			return 1;
	return 0;
}


void usage(char *source_file, int open_manpage)
{
	char command[128];
	snprintf(command, strlen(source_file) - 1, "%s", source_file);

	if (open_manpage) {
		execlp("man", "man", strrchr(command, '/') + 1, NULL);
		perror(argv0);
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr,
			"%s: Invalid usage, see `man 1 %s` for more information.\n",
			argv0, strrchr(command, '/') + 1);
	}
}

