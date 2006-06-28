#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <strings.h>


#define STATFREQ	20000
#define COMPWIN		1000
#define MAXLETTERS	5000
#define MAXLINE		5000
#define FALSE 0
#define TRUE  1

#define CELLRELS 30

#define HASHC1 257
#define HASHC2 107

#define state(x,y) states [(x) * (letter_count)  + (y)]
#define clear_state(j) for((c)=0; (c)<(letter_count); state((j), (c)++) = 0 )
#define cell_to_int(c) ((c).tc.b)
#define free_cell(d) cells[(d)].i==0

int HASHSK, HASHS;

typedef unsigned char uchar;
typedef union {
	long long i;
	struct {
		int b,c;
	} tc;
} tcell;

int letter_count, codes[256], line=0;
uchar wf[MAXLINE], owf[MAXLINE], inf[MAXLINE];

int *states_hash_table;
int *hash_codes;

int strings_size, strings_delta, first_string, last_string, strings_count;
uchar *strings;

int cells_size, cells_delta, first_free_cell, last_full_cell, states_count, full_cells;
int *states;
tcell *cells;
FILE *fi,*fo;

void error (uchar *s1) {
	printf("\n%s in line %d\n\n", s1, line);
	exit(1);
}


int state_cmp (int p, int i) {
	int j;

	if ((state(i,0)==0 && cells[p].tc.c != letter_count) ||
		(state(i,0)!=0 && cells[p].tc.c != 0)) return(FALSE);
	for (j=1; j < letter_count; j++)
		if (p+j<=last_full_cell && cells[p+j].tc.c == j) {
			if (cell_to_int(cells[p+j]) != state(i,j)) return(FALSE);
		} else if (state(i,j) != 0) return(FALSE);
	return(TRUE);
}

int state_fits(int i, int j, int k) {
	int c;

	for (c=1; c<=k; c++)
		if (state(i,c)!=0 && !free_cell(j+c)) return(FALSE);
	return(TRUE);
}

void save_cell (int loc, int ch, int p) {
	cells[loc].tc.b = p;
	cells[loc].tc.c = ch;
	full_cells++;
}

int string_set(int i) {
	uchar s[letter_count];
	int j,k;

	for (j=1, k=0; j < letter_count; j++)
		if (state(i,j) != 0) s[k++]=(uchar) j;
	s[k++]=0;

	j=first_string;
	while (j<last_string && strcmp(s, strings+j)!=0) j+=strlen(strings+j)+1;
	if (j>last_string) error("String saving");
	if (j==last_string) {
		if (last_string + k > strings_size)
			if ((strings=realloc(strings,(strings_size+=strings_delta)))==NULL)
				error("Memory full");
		strcpy(strings+j,s);
		last_string+=k;
		strings_count++;
	}
	if(j==1) printf("string_set returns %d\n",j);
	return j;
}

int new_state(int i, int k) {
	int j,c;

	for (j=first_free_cell; ; j++) {
		if (j>=cells_size-k) {
			if ((cells=realloc(cells,(cells_size+=cells_delta)*sizeof(tcell)))==NULL)
				error("Memory full");
			memset(cells+(cells_size-cells_delta),0,cells_delta*sizeof(tcell));
		}
		if (free_cell(j) && state_fits(i,j,k)) {
		    for (c=1; c<=k; c++)
			if (state(i,c)!=0) save_cell(j+c, c, state(i,c));
		    save_cell(j, (state(i,0)==0 ? letter_count : 0), string_set(i));
		    break;
		}
	}
	if (j-COMPWIN > first_free_cell) first_free_cell = j-COMPWIN;
	for ( ; !free_cell(first_free_cell); first_free_cell++) ;
	if (j+k > last_full_cell) last_full_cell = j+k;
	return(j);
}

int hashcode(int i, int k) {
	int h,j;
	for(h=0, j=0; j<=k; h = h*HASHC1 + j + letter_count*state(i,j++) ) ;
	return h;
}

void hashresize() {
	int * sht1;
	int * hc1;
	int i, hashs1, p, h;

	HASHSK++;
	hashs1 = (1 << HASHSK) - 1;
	if ((sht1=malloc(hashs1 * sizeof(int)))==NULL) error("Memory full");
	if ((hc1=malloc(hashs1 * sizeof(int)))==NULL) error("Memory full");
	memset(sht1, 0, hashs1 * sizeof(int));
	for (i=0; i<HASHS; i++) {
		if ((p=states_hash_table[i])!=0) {
			h=abs(hash_codes[i])%hashs1;
			while (sht1[h]!=0) h=(h+HASHC2)%hashs1;
			sht1[h] = p;
			hc1[h] = hash_codes[i];
		}
	}
	free(states_hash_table);
	free(hash_codes);
	states_hash_table=sht1;
	hash_codes=hc1;
	HASHS=hashs1;
}

int save_state (int i) {
	int p;
	int k;
	int h,hc;
	hc=hashcode(i,(k=codes[owf[i]]));
	h=abs(hc)%HASHS;

	while ((p = states_hash_table[h]) != 0 && !state_cmp(p,i)) h=(h+HASHC2)%HASHS;
	if (p==0) {
		if (HASHS - states_count < HASHS / 10 ) {
			hashresize();
			h=abs(hc)%HASHS;
			while (states_hash_table[h] != 0) h=(h+HASHC2)%HASHS;
		}
		states_hash_table[h] = new_state(i,k);
		hash_codes[h] = hc;
		states_count++;
	}
	return states_hash_table[h];
}


void init_mem (int letters, int fsize) {
	if ((states=malloc(MAXLETTERS*letters*sizeof(int)))==NULL) error("Memory full");

	cells_size = fsize / (CELLRELS * sizeof(tcell)); cells_delta = cells_size / 3;
	first_free_cell = 2; last_full_cell = 0; states_count = 0; full_cells = 0;
	if ((cells = malloc(cells_size * sizeof(tcell)))==NULL) error("Memory full");
	memset(cells, 0, cells_size * sizeof(tcell));

	strings_size = fsize / (CELLRELS * CELLRELS); strings_delta = strings_size / 3;
	first_string = 1; last_string = 1; strings_count = 0;
	if ((strings = malloc(strings_size))==NULL) error("Memory full");

	HASHSK=5;
	HASHS = (1 << HASHSK) - 1;
	if ((states_hash_table=malloc(HASHS * sizeof(int)))==NULL) error("Memory full");

	memset(states_hash_table, 0, HASHS * sizeof(int));
	if ((hash_codes=malloc(HASHS * sizeof(int)))==NULL) error("Memory full");
}

void print_statistics() {
	printf("\n\nStatistics\n===========\n");
	printf("Words processed: %d\n", line);
	printf("States created:  %d\n", states_count);
	printf("All cell count:  %d\n", last_full_cell+1);
	printf("Full cells:      %d\n", full_cells);
	printf("First free cell: %d\n", first_free_cell);
	printf("Cells allocated: %d\n", cells_size);
	printf("Strings size:    %d\n", strings_size);
	printf("Last String:     %d\n", last_string);
	printf("String count:    %d\n", strings_count);
}


uchar *c, wf1[MAXLINE], owf1[MAXLINE], s[MAXLINE];
int eof=FALSE;

int get_word_info(char *wf) {
	int i;

	if (eof) return(FALSE);
	if (++line % STATFREQ == 0) print_statistics();
	strcpy(owf1,wf1);

	if (fgets(s,MAXLINE,fi)==NULL) eof=TRUE;
	else {
		for (c=s,i=0; *c>=' '; wf1[i++]=*c++)
			if (codes[*c]==0) error("Illegal symbol");
		wf1[i] = 0;
	}

	if (strcmp(owf1,wf1)>0) {
		printf("\n'%s'>'%s' -- ",owf1,wf1);
		error("Alphabetical order error");
	}

	strcpy(wf,owf1);
	return(TRUE);
}

void main (int argc, uchar **argv) {
	int i, j, c;
	struct stat f_stat;

	printf("compdic: Utility for compilation of word dictionaries\n");
	if (argc!=4) error("Usage:\ncompdic <alphabet> <text_dic> <comp_dic>");

	if ((fi=fopen(argv[1],"rb"))==NULL) error("Error opening alphabet file");
	memset(codes, 0, 256);
	for (letter_count=1; (c=getc(fi))>=' '; codes[c] = letter_count++) ;
	fclose(fi);

	stat(argv[2],&f_stat);
	init_mem(letter_count, f_stat.st_size + 50000);

	if ((fi=fopen(argv[2],"rb"))==NULL) error("Error opening input file");

	clear_state(0);
	owf[0]=0;

	wf1[0]=0; get_word_info(wf); // init of get_word_inf()

	while (get_word_info(wf)) {

		for (i=0; wf[i]==owf[i]; i++) ;		// find difference location

		for (j=strlen(owf)-1; j>=i; j--) state(j, codes[owf[j]]) = save_state(j+1);

		for (j=i+1; j<=strlen(wf); j++) clear_state(j);

		state(--j,0) = 1;

		strcpy(owf, wf);
	}
	fclose(fi);

	for (j=strlen(owf)-1; j>=0; j--) state(j, codes[owf[j]]) = save_state(j+1);

	save_cell(0, 'S', save_state(0));
	save_cell(1, 'T', last_full_cell+1);

	fo = fopen(argv[3], "wb");
	fwrite (cells, sizeof(tcell), last_full_cell+1, fo);
	fwrite (strings, 1, last_string, fo);
	fclose(fo);

	print_statistics();
}
