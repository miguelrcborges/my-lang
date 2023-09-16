#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "token.h"
#include "scanner.h"

int main(int argc, char **argv) {
	if (argc > 3) {
		printf("Usage:\n\tepic [FILE]\n");
		return 64;
	}

	if (argc == 2) {
		FILE *f = fopen(argv[1], "r");
		if (f == NULL) {
			fprintf(stderr, "Failed to open the file %s.\n", argv[1]);
			return 64;
		}

		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		char *src = malloc(size);
		if (size <= 0 || fseek(f, 0, SEEK_SET) != 0) {
			fprintf(stderr, "Failed to store the file %s.\n", argv[1]);
			fclose(f);
			return 64;
		}
		fread(src, 1, size, f);
		fclose(f);

		Token *tokens = Scanner_tokenize(src);

		free(tokens);
		free(src);
	}
}
