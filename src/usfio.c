#include "usfio.h"

char **usf_ftot(char *file, char *options, uint64_t *l) {
	FILE *f = fopen(file, options);

	usf_dynarr *text;
	usf_dynarr *line;

	char fbuffer[BUFSIZ], *s, **txt; //File buffer
	uint64_t i, lines = 0;
	int64_t j;
	usf_data chr;
	size_t loaded; //Loaded bytes to buffer

	if (f == NULL) return NULL; //Failed to open file

	text = usf_arrtodyn(NULL, 0); //All lines
	line = usf_arrtodyn(NULL, 0); //Current line

	j = 0; //Address within current line

	for (;;) {
		loaded = fread(fbuffer, 1, BUFSIZ, f);

		if (ferror(f)) {
			//Free and close
			usf_freeda(line); //Free current line

			for (i = 0; i < lines; i++)
				free(usf_daget(text, i).p); //Free parsed lines

			usf_freeda(text); //Free text array
			fclose(f);
			return NULL; //Error when reading from file
		}

		for (i = 0; i < loaded; i++, j++) {
			chr = USFDATAU(fbuffer[i]);
			usf_daset(line, j, chr);

			if (chr.u == '\n') { //Normally, all files should end with \n
				usf_daset(line, j + 1, USFDATAU('\0'));
				s = malloc(j + 2); //Char size is 1. Also, include \0

				//Copy and reset (j = -1, next j = 0)
				for (j++; j >= 0; j--)
					s[j] = (char) line -> array[j].u;

				usf_daset(text, lines++, USFDATAP(s)); //Add line
			}
		}

		if (feof(f)) break; //End of file
	}

	if (j > 0) { //If file does not end with \n
		usf_daset(line, j, USFDATAU('\0'));
		s = malloc(j + 1);
		memcpy(s, line->array, j + 1);
		usf_daset(text, lines++, USFDATAP(s));
	}

	*l = lines; //Set total line count
	txt = malloc(sizeof(char *) * lines); //Prepare string array

	usf_freeda(line); //Free current line

	for (i = 0; i < lines; i++) {
		txt[i] = usf_daget(text, i).p; //Transfer parsed lines
	}

	usf_freeda(text); //Free text array
	fclose(f);

	return txt;
}

void usf_printtxt(char **text, uint64_t len, FILE *stream) {
	uint64_t i;
	char *c;

	for (i = 0; i < len; i++)
		for (c = text[i]; *c != '\0'; c++)
			fprintf(stream, "%c", *c);
}
