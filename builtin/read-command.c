/*
 * GIT - The information manager from hell
 *
 * Copyright (C) Linus Torvalds, 2005
 */

#include "builtin.h"
#include "pkt-line.h"
#include "argv-array.h"

int read_arg(char *buffer, unsigned size) {
	int r = packet_read(0, NULL, NULL, buffer, size, 0);

	if (r < 0)
		die("unexpected end of input");

	if (strlen(buffer) < r)
		die("arguments can't contain \\0");

	return r;
}

int read_int_arg(char *buffer, unsigned size) {
	if (!read_arg(buffer, size))
		die("expected integer");

	int val = atoi(buffer);

	if (val < 0)
		die("expected positive integer");

	return val;
}

int cmd_read_command(int argc, const char **argv, const char *prefix)
{
	int i;
	struct argv_array new_argv = ARGV_ARRAY_INIT;
	char buf[LARGE_PACKET_MAX];

	int envc = read_int_arg(buf, sizeof(buf));

	for (i = 0; i < envc; i++) {
		if (!read_arg(buf, sizeof(buf)))
			die("expected environment variable, got empty packet");

		char *value = strchr(buf, '=');

		if (!value) {
			value = "";
		} else {
			*value++ = '\0';
		}

		setenv(buf, value, 1);
	}

	int new_argc = read_int_arg(buf, sizeof(buf));

	argv_array_push(&new_argv, "git");

	for (i = 0; i < new_argc; i++) {
		read_arg(buf, sizeof(buf));
		argv_array_push(&new_argv, buf);
	}

	return cmd_main(new_argv.argc, new_argv.argv);
}
