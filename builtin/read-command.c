/*
 * GIT - The information manager from hell
 *
 * Copyright (C) Linus Torvalds, 2005
 */

#include "builtin.h"
#include "pkt-line.h"
#include "argv-array.h"

int cmd_read_command(int argc, const char **argv, const char *prefix)
{
	struct argv_array new_argv = ARGV_ARRAY_INIT;
	char buf[LARGE_PACKET_MAX];

	int r = packet_read(0, NULL, NULL, buf, sizeof(buf), 0);

	if (r <= 0)
		die("zero length argument length packet");

	int new_argc = atoi(buf);

	if (new_argc < 0)
		die("invalid argument length packet");

	argv_array_push(&new_argv, "git");

	for(int i = 0; i < new_argc; i++) {
		r = packet_read(0, NULL, NULL, buf, sizeof(buf), 0);

		if (strlen(buf) < r)
			die("invalid argument at %d, arguments can't contain \\0", i);

		argv_array_push(&new_argv, buf);
	}

	return cmd_main(new_argv.argc, new_argv.argv);
}
