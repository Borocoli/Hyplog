#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifndef TLEN
#define TLEN 3
#endif

#ifndef I
#define I 1
#endif

#ifndef LAST
#define LAST 5
#endif

#ifndef PATH
#define PATH "./"
#endif

struct meta {
	off_t poz;
	off_t len[3];
};

struct log {
	char hyp[TLEN];
	char exp[TLEN];
	char rez[TLEN];

};

int check_size(char *s) {
	if (strlen(s) > TLEN) {
		return 1;
	}
	return 0;
}


unsigned long int clear() {
	int c = 0;
	unsigned long int i = 0;
	do {
		c = getchar();
		i++;
	}while(c != '\n' && c != EOF);
	return i;
}


int8_t showLog(struct meta *m, struct log *l) {
	printf("    Hypothesis:\n %.*s\n    Experiment:\n %.*s\n    Result:\n %.*s\n\n",
			m->len[0], l->hyp, 
			m->len[1], l->exp, 
			m->len[2], l->rez);
	return 0;	
}


int8_t print(FILE *metaf, FILE *dataf, off_t index1, off_t index2, int8_t (*func)(struct meta*, struct log*)) {
	struct meta m;
	if(fseeko(metaf, index1*sizeof(struct meta), SEEK_SET) != 0)  {
		printf("Moving in the meta file failed.\n");
		return -1;
	}

	struct log l;

	for(off_t i = index1; i < index2; ++i) {
		if(fread(&m, sizeof(struct meta), 1, metaf) != 1) {
			printf("Reading meta file failed.\n");
			return -1;

		}
		if(
				fseeko(dataf, m.poz, SEEK_SET) != 0 || 
				fread(&l.hyp, sizeof(char), m.len[0], dataf) != m.len[0] ||
				fread(&l.exp, sizeof(char), m.len[1], dataf) != m.len[1] ||
				fread(&l.rez, sizeof(char), m.len[2], dataf) != m.len[2]) {
			printf("Reading data file failed.\n");
			return -1;
		}
		if(func(&m, &l) != 0) {
			printf("Printing failed.\n");
			return -1;
		}

	}
	return 0;

}

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("This program requires a subcommand\n");
		return 2;
	};

	char jour[] = PATH "hypjour.blob";
	char mdata[] = PATH  "hypmeta.blob";
	char data[] =  PATH "hypdata.blob";

	off_t header[2]; //nr, pos

	FILE* jourf = fopen(jour, "rb+");
	FILE* metaf;
	FILE* dataf;
	if(jourf == NULL) {
		printf("No journal file, creating...\n");
		jourf = fopen(jour, "wb+");
		metaf = fopen(mdata, "wb");
		dataf = fopen(data, "wb");
		header[0] = 0;
		header[1] = 0;
		if(fwrite(&header, sizeof(off_t), 2, jourf) != 2) {
			printf("An error has occured, try again\n");
			return -1;
		}
		fclose(metaf);
		fclose(dataf);

	}else {
		if(fread(&header, sizeof(off_t), 2, jourf) != 2) {
			printf("Failed reading metadata, try again\n");
			return -1;
		}
	}
	fclose(jourf);


	const char *sbcmd= argv[1];
	if(strcmp(sbcmd, "create") == 0) {
		jourf = fopen(jour, "rb+");
		metaf = fopen(mdata, "rb+");
		dataf = fopen(data, "rb+");

		printf("There are %lld logs and the last position is %lld.\n", header[0], header[1]);

		struct meta m;
#if I
		if(argc > 2) {
			printf("Create subcommand does not accept any other arguments, ignoring...\n");
		}
#else
		if(argc != 5) {
			printf("The number of arguments does not match. It must be:\nhyplog create {file1} (file2) {file3}\n");
			return -1;
		}
#endif

		if(fseeko(dataf, header[1], SEEK_SET) != 0) {
			printf("Fseeko failed\n");
			return -1;	
		}


#define TMPL TLEN+2
		char tmp[TMPL] ;
		long int l;
		m.poz = header[1];
		char s[][11] = {"hypothesis", "experiment", "result"};
		for( int8_t i = 0; i < 3; ++i) {
#if I

			while (1) {
				printf("Write %s: ", s[i]);
				fgets(tmp, TMPL, stdin);
				l = strlen(tmp);
				if ((l >= TLEN) && (tmp[l-1] != '\n')) {
					unsigned long int ll = clear();

					printf("Max allowed text length is %d, your input has %ld characters. Summarize and try again.\n", TLEN, l+ll-1);
				}else {
					break;
				}
			}
			tmp[l-1] = '\0';
			l = l-1;

#else
			FILE *f = fopen(argv[2+i], "r");
			if(f == NULL) {
				printf("File %.100s could not be opened.\n");
				return -1;				
			}
			fgets(tmp, TMPL, f);
			l = strlen(tmp);
			if(tmp[l-1] == '\n') {
				tmp[l-1] = '\0';
				l -= 1;
			} 

			if(l >= TLEN && (fgetc(f) != EOF && feof(f) == 0)) {
				printf("Max allowed text length is %d, file %.100s is bigger than that. Summarize and try again.\n", TLEN, argv[2+i]);

				return -1;

			}

#endif
			m.len[i] = l;
			fwrite(tmp, sizeof(char), m.len[i], dataf);

		}
		if(fseeko(metaf, 0, SEEK_END) != 0) {
			return -1;
		}
		fwrite(&m, sizeof(m), 1, metaf);
		fclose(metaf);

		header[0]++;
		header[1] = ftell(dataf);
		fclose(dataf);

		if(fseeko(jourf, 0, SEEK_SET) != 0) {
			printf("An error has occured, try again\n");
			return -1;
		}
		size_t test = fwrite(&header, sizeof(off_t), 2, jourf);
		if(test != 2) {
			printf("An error has occured, try again\n");
			return -1;
		}

		fclose(jourf);


	}else if(strcmp(sbcmd, "read") == 0) {
		if(argc < 3) {
			printf("Specify what logs to view. The argument needs to be of form: last - to get the last entries; \n {number} - to get the entry with index {number}; \n {number}-{number} - to get the entries these indexes;\n");
			return -1;

		}

		if(argc > 3) {
			printf("Only one argument accepted. Ignoring the rest.\n");
			return -1;
		}

		if(header[0] == 0) {
			printf("No log found. Write a log before reading.\n");
			return -1;
		}

		char *arg = argv[2];
		off_t interval[2] = {0, 0};
		uint8_t c = 0;
		if(strcmp(arg, "last") == 0) {
			c = 1;
			interval[0] = header[0] - LAST;
			interval[1] = header[0];
		}else {
			for(uint16_t i = 0; arg[i] != '\0'; ++i) {
				if(arg[i] >= '0' && arg[i] <= '9') {
					interval[c] *= 10;
					interval[c] += arg[i] - '0';
				}else if(arg[i] == '-' && c == 0) {
					c = 1;
				}else {
					printf("Invalid argument. The argument needs to be of form: \n {number} \n {number}-{number}\n");
					return -1;
				}
			}
		}
		if(arg[0] == '-') {
			printf("Reverse\n");
			c = 1;
			interval[0] = header[0] - interval[1];
			interval[1] = header[0];
		}
		if(interval[0] < 0) {
			interval[0] = 0;
		}
		if(interval[0] >= header[0]) {
			printf("There are %lld logs, the index must be smaller than it.\n", header[0]);
			return -1;
		} 
		FILE* metaf = fopen(mdata, "rb");
		FILE* dataf = fopen(data, "rb");

		if(c == 1) {
			if(interval[1] > header[0]) {
				interval[1] = header[0];
			}
			if(print(metaf, dataf, interval[0], interval[1], showLog) != 0) {
				return -1;
			}
		}else {
			interval[1] = interval[0] + 1;
			if(print(metaf, dataf, interval[0], interval[1], showLog) != 0) {
				return -1;
			}

		}
		fclose(metaf);
		fclose(dataf);



	}else {
		printf("Not a valid subcommand, valid subcommands are: create, read.\n");
	}

	return 0;
}


