/**
 * MIT/X Consortium License
 * 
 * Copyright © 2015  Mattias Andrée <m@maandree.se>
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
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>



#define VERSION  "1.0"


static int find_commands(const char *directory);
static int cmpstrp(const void *a, const void *b);
static int synchronous_spawn(const char *file, char *argv[]);
static void usage(int exit_value);
static void run(const char *action, int argc, char *argv[]);


char *argv0;


static char **commands = NULL;
static size_t commands_size = 16;
static size_t commands_ptr = 0;


static int
find_commands(const char *directory)
{
	DIR *dir;
	struct dirent *file;
	char *command;
	char **new;
	int saved_errno;
	char *cmd = NULL;

	if (!*directory)
		return 0;

	dir = opendir(directory);
	if (dir == NULL)
		return 0;

	while ((file = readdir(dir))) {
		command = file->d_name;
		if (strstr(command, "sit-") == command && command[4]) {
			if (!commands || commands_ptr == commands_size) {
				commands_size <<= 1;
				new = realloc(commands, commands_size * sizeof(char *));
				if (!new)
					goto fail;
				commands = new;
			}
			cmd = malloc((strlen(command) + 1) * sizeof(char));
			if (!cmd)
				goto fail;
			strcpy(cmd, command);
			commands[commands_ptr++] = cmd;
			cmd = NULL;
		}
	}

	closedir(dir);
	return 0;
fail:
	saved_errno = errno;
	closedir(dir);
	errno = saved_errno;
	return -1;
}


static int
cmpstrp(const void *a, const void *b)
{
	return strcmp(*(char *const *)a, *(char *const *)b);
}


static int
synchronous_spawn(const char *file, char *argv[])
{
	pid_t pid, reaped;
	int status;

	pid = fork();
	if (!pid) {
		execvp(file, argv);
		_exit(1);
	}

	for (;;) {
		reaped = waitpid(pid, &status, 0);
		if (reaped < 0 && errno != EINTR) {
			perror(argv0);
			return -1;
		} else if (reaped == pid) {
			break;
		}
	}

	return status == 0 ? 0 : -1;
}


static void
usage(int exit_value)
{
	char *path = NULL;
	char *begin;
	char *end;
	size_t i, maxlen = 0, len;
	char *prefix = NULL;
	char *last_success = NULL;
	char *argv[3];

	path = getenv("PATH");
	if (!path || !*path) {
		fprintf(stderr, "%s: $PATH is missing", argv0);
		exit(EXIT_FAILURE);
	}

	begin = malloc((strlen(path) + 1) * sizeof(char));
	if (!begin)
		goto fail;
	strcpy(begin, path);
	path = begin;

	for (;;) {
		end = strchr(begin, ':');
		if (end) {
			*end = 0;
			if (find_commands(begin) < 0)
				goto fail;
			begin = end + 1;
		} else {
			if (find_commands(begin) < 0)
				goto fail;
			break;
		}
	}

	qsort(commands, commands_ptr, sizeof(char *), cmpstrp);
	/* TODO change to a stable sort, like merge sort */

	fprintf(stderr, "usage: %s [--version] [--help] <command> [<args>]\n\n", argv0);
	fprintf(stderr, "available sit commands:\n");

	for (i = 0; i < commands_ptr; i++) {
		len = strlen(commands[i]);
		if (maxlen < len)
			maxlen = len;
	}

	prefix = malloc((3 + maxlen) * sizeof(char));
	if (!prefix)
		goto fail;

	argv[0] = "sit";
	argv[2] = NULL;
	for (i = 0; i < commands_ptr; i++) {
		if (!last_success || strcmp(commands[i], last_success)) {
			sprintf(prefix, "  %-*s", (int)maxlen, commands[i] + 4);
			argv[1] = prefix;
			if (!synchronous_spawn(commands[i], argv))
				last_success = commands[i];
		}
	}

done:
	free(prefix);
	free(path);
	while (commands_ptr--)
	  free(commands[commands_ptr]);
	free(commands);
	exit(exit_value);

fail:
	perror(argv0);
	exit_value = EXIT_FAILURE;
	goto done;
}


static void
run(const char *action, int argc, char *argv[])
{
	char *command;

	command = malloc((5 + strlen(action)) * sizeof(char));
	if (!command)
		goto fail;

	sprintf(command, "sit-%s", action);
	argv[1] = command;

	memmove(argv, argv + 1, (argc - 1) * sizeof(char *));
	argv[argc - 1] = NULL;

	execvp(command, argv);

fail:
	perror(argv0);
	free(command);
	exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
	char *action;

	argv0 = *argv++;
	argc--;

	if (argc)
		action = *argv;
	else
		usage(EXIT_FAILURE);

	if (!strcmp(action, "--version")) {
		printf("sit-" VERSION " © 2015 sit engineers, see LICENSE for details\n");
	} else if (!strcmp(action, "--help") && argc == 1) {
		usage(EXIT_SUCCESS);
	} else {
		if (!strcmp(action, "--help")) {
			action = argv[1];
			argv[1] = "--help";
		}
		if (*action == '-' || !*action)
			usage(EXIT_FAILURE);
		run(action, argc + 1, argv - 1);
	}

	return EXIT_SUCCESS;
}

