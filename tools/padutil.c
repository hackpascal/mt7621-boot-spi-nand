// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2020 Weijie Gao <hackpascal@gmail.com>
 *
 * Binary file padding utility
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <getopt.h>

static uint32_t align_size;
static uint32_t max_size;
static uint8_t fill_char = 0xff;
static bool show_verbose;
static const char *input_file;
static const char *output_file;

static uint32_t input_file_size;
static uint32_t padding_size;

static void usage(FILE *con, const char *progname, int exitcode)
{
	size_t proglen = strlen(progname);
	const char *prog = progname + proglen - 1;

	while (prog > progname) {
		if (*prog == '/' || *prog == '\\') {
			prog++;
			break;
		}

		prog--;
	}

	fprintf(con,
		"Binary pad utility\n"
		"\n"
		"Usage: %s [options] <input file> <output file>\n"
		"\n"
		"Options:\n"
		"    -a <size>   File size aligned to <size>\n"
		"    -m <size>   Set max output file size\n"
		"    -f <val>    Fill padding space with character <val>\n"
		"    -v          Display file information\n"
		"    -h          Show this help\n",
		prog
	);

	exit(exitcode);
}

static uint32_t parse_value(const char *optarg, const char optchar)
{
	char *end;
	uint32_t val;

	val = strtoul(optarg, &end, 0);
	if ((end == optarg) || *end) {
		fprintf(stderr, "Error: -%c: '%s' is not a valid number\n", optchar, optarg);
		exit(1);
	}

	return val;
}

static void parse_arg(int argc, char *argv[])
{
	static const char optstr[] = "a:m:f:vh";
	int opt;

	if (argc <= 1)
		usage(stdout, argv[0], 0);

	while ((opt = getopt(argc, argv, optstr)) != -1) {
		switch (opt) {
		case 'a':
			align_size = parse_value(optarg, opt);
			if (align_size <= 1) {
				fprintf(stderr, "Error: alignment size must be larger than 1\n");
				exit(1);
			}
			break;
		case 'm':
			max_size = parse_value(optarg, opt);
			if (!max_size) {
				fprintf(stderr, "Error: max file size can't be zero\n");
				exit(1);
			}
			break;
		case 'f':
			fill_char = parse_value(optarg, opt) & 0xff;
			break;
		case 'v':
			show_verbose = true;
			break;
		case 'h':
			usage(stdout, argv[0], 0);
			break;
		default:
			usage(stderr, argv[0], 1);
			break;
		}
	}

	if (argc <= optind) {
		fprintf(stderr, "Error: input file not specified\n");
		exit(1);
	}

	if (argc - optind == 1) {
		fprintf(stderr, "Error: output file not specified\n");
		exit(1);
	}

	input_file = argv[optind];
	output_file = argv[optind + 1];
}

static void process_file(void)
{
	FILE *fin, *fout;
	uint8_t buff[64 << 10];
	size_t rdsize, wrsize, sz2pad, padsize = 0;

	if (!strcmp(input_file, "-")) {
		if (freopen(NULL, "rb", stdin) != stdin) {
			fprintf(stderr, "Error: Failed to change mode of <stdin>\n");
			exit(1);
		}
		fin = stdin;
		input_file = "<stdin>";
	} else {
		fin = fopen(input_file, "rb");
		if (!fin) {
			fprintf(stderr, "Error: Failed to open input file '%s'\n", input_file);
			exit(1);
		}
	}

	if (!strcmp(output_file, "-")) {
		if (freopen(NULL, "wb", stdout) != stdout) {
			fprintf(stderr, "Error: Failed to change mode of <stdout>\n");
			exit(1);
		}
		fout = stdout;
		output_file = "<stdout>";
	} else {
		fout = fopen(output_file, "wb");
		if (!fout) {
			fprintf(stderr, "Error: failed to open output file '%s'\n", output_file);
			exit(1);
		}
	}

	while (true) {
		rdsize = fread(buff, 1, sizeof(buff), fin);
		if (!rdsize) {
			if (feof(fin))
				break;

			fprintf(stderr, "Error occured while reading file '%s', error code %d\n",
				input_file, ferror(fin));
			exit(1);
		}

		wrsize = fwrite(buff, 1, rdsize, fout);
		if (wrsize != rdsize) {
			fprintf(stderr, "Error occured while writing file '%s', error code %d\n",
				output_file, ferror(fout));

			if (fout != stdout) {
				fclose(fout);
				remove(output_file);
			}

			exit(1);
		}

		input_file_size += wrsize;
	}

	if (fin != stdin)
		fclose(fin);

	if (align_size) {
		padding_size = input_file_size % align_size;
		if (padding_size)
			padding_size = align_size - padding_size;
	}

	if (max_size) {
		if (input_file_size + padding_size > max_size) {
			fprintf(stderr, "Error: output file is larger than %u bytes\n", max_size);

			if (fout != stdout) {
				fclose(fout);
				remove(output_file);
			}

			exit(1);
		}
	}

	if (padding_size) {
		memset(buff, fill_char, sizeof(buff));

		while (padsize < padding_size) {
			sz2pad = padding_size - padsize;
			if (sz2pad > sizeof(buff))
				sz2pad = sizeof(buff);

			wrsize = fwrite(buff, 1, sz2pad, fout);
			if (wrsize != sz2pad) {
				fprintf(stderr, "Error occured while padding file '%s', error code %d\n",
					output_file, ferror(fout));

				if (fout != stdout) {
					fclose(fout);
					remove(output_file);
				}

				exit(1);
			}

			padsize += sz2pad;
		}
	}

	if (fout != stdout)
		fclose(fout);
}

static void display_info(void)
{
	if (!show_verbose)
		return;

	printf(
		"Input file:     %s\n"
		"Input size:     %u (0x%x)\n",
		input_file,
		input_file_size, input_file_size
	);

	printf("----------------------------------------\n");

	printf(
		"Output file:    %s\n"
		"Output size:    %u (0x%x)\n",
		output_file,
		input_file_size + padding_size, input_file_size + padding_size
	);

	if (padding_size) {
		printf(
			"Padding size:   %u (0x%x)\n"
			"Fill char:      0x%02x\n",
			padding_size, padding_size,
			fill_char
		);
	}

	if (max_size) {
		printf(
			"Maximum size:   %u (0x%x)\n"
			"Space usage:    %u%%",
			max_size, max_size,
			((input_file_size + padding_size) * 100) / max_size
		);
	}
}

int main(int argc, char *argv[])
{
	parse_arg(argc, argv);

	process_file();

	display_info();

	return 0;
}
