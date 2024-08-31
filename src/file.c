#include "file.h"

char *read_file(char *file_name) {
	FILE* file = NULL;
	char *output = NULL;
	long size;

	file = fopen(file_name, "r");
	if (!file)
		return NULL;

	// get length of file as size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	// read file into calloc'd buffer and terminate with null
	output = calloc(size + 1, sizeof(char));
	fread(output, size, 1, file);
	output[size] = '\0';
			
	fclose(file);
	return output;
}
