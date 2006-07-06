#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <strings.h>

#define MAXLINE		500
#define cell_to_int(c) ((cells[(c)]).tc.b)

#define QUEUESIZE 16384

typedef unsigned short ushort;
typedef union {
	long long i;
	struct {
		int b,c;
	} tc;
} tcell;

tcell *cells;
ushort *infos, s[MAXLINE], letters[65536], info[MAXLINE], s1[MAXLINE]; 
FILE *fo, *fi;
int codes[65536], letter_count;

int fgetchar(FILE *fi) {
	ushort c2;
	
	if (fread(&c2,2,1,fi)==1) return (int) c2 ; else return EOF;
}

int wstrlen(ushort *s) {
	int i;
	for (i=0; s[i]!=0; i++) ;
	return i;
}

int buff[QUEUESIZE];
int charsread = 0;

int testin(int i) {
	if ( i >= charsread) {
		if (i!=charsread) {
			fprintf(stderr,"Symbols not processed\n");
			exit(1);
		}
		buff[i % QUEUESIZE] = fgetchar(fi);
		if (buff[i % QUEUESIZE]==-1) buff[i % QUEUESIZE]=0;
		charsread++;
	}
	return buff[i % QUEUESIZE];
}

void main (int argc, char **argv) {
	struct stat f_stat;
	int c, c1, state, i, j, subst, subst1;
	ushort c2;

	fprintf(stderr, "rewrite: Utility for dictionary rewriting the standard input to the standard output\n");
	if (argc!=4) {
		printf("Usage:\nrewrite <alphabet> <info_dic> <comp_dic>");
		exit(1);
	}
	
	fi = fopen(argv[1],"rb");
	memset(codes, 0, 65536*sizeof(int));
	for (letter_count=1; (c=fgetchar(fi))>=' '; letter_count++) {
		if (codes[c] != 0) {
			printf("Duplicated letter in the alphabet! \n");
			exit(1);
		}
		codes[c] = letter_count;
		letters[letter_count]=c;
	}
	fclose(fi);
	

	stat(argv[2],&f_stat);
	fi = fopen(argv[2],"rb");
	infos = malloc(f_stat.st_size);
	fread(infos, 1, f_stat.st_size, fi);
	fclose(fi);
	
	stat(argv[3],&f_stat);
	fi = fopen(argv[3],"rb");
	cells = malloc(f_stat.st_size);
	fread(cells, 1, f_stat.st_size, fi);
	fclose(fi);
	
	
	fi = stdin;  //fopen(argv[4],"rb");
	fo = stdout; //fopen(argv[5],"wb");
		
	state = cell_to_int(0);
	i=0; j=0; subst=-1; subst1=0;
	while (testin(i)!=0) {
		c=testin(j);
		c1 = codes[c];
		if (c1!=0 && cells[state+c1].tc.c==c1) {
			state = cell_to_int(state+c1);
			if (cells[state].tc.c==0) {
				subst = j;
				subst1 = cell_to_int(state);
			}
			j++;
		} else {
			if (subst!=-1) {
				i=subst+1; j=i; state = cell_to_int(0);
				fwrite(infos+subst1,sizeof(ushort),wstrlen(infos+subst1),fo);
				subst=-1;
			} else {
				c2 = testin(i);
				fwrite(&c2,sizeof(ushort),1,fo);
				i++; j=i; state = cell_to_int(0);
			}
		}
	}

		
//	fclose(fi);
//	fclose(fo);				

}	
