#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define STATFREQ	100000
#define COMPWIN		1000
#define MAXLETTERS	5000
#define MAXLINE		5000
#define FALSE 0
#define TRUE  1

#define INFORELS 100
#define CELLRELS 30

#define HASHC1 257
#define HASHC2 107

#define state(x,y) states [(x) * (letter_count)  + (y)]
#define clear_state(j) for((c)=0; (c)<(letter_count); state((j), (c)++) = 0 )
#define cell_to_int(c) ((c).tc.b)
#define free_cell(d) cells[(d)].i==0

int HASHSK, HASHS;
int HASHIK, HASHI;

typedef unsigned short ushort;
typedef union {
	long long i;
	struct {
		int b,c;
	} tc;
} tcell;

int letter_count, codes[65536], line=0;
ushort wf[MAXLINE], owf[MAXLINE], inf[MAXLINE];

int *states_hash_table;
int *infos_hash_table;
int *hash_codes;

int info_size, info_full, info_delta, infos_count;
int cells_size, cells_delta, first_free_cell, last_full_cell, states_count, full_cells;
ushort *infos;
int *states;
tcell *cells;
FILE *fi,*fo;

void error (char *s1) {
	printf("\n%s in line %d\n\n", s1, line);
	exit(1);
}

int fgetchar(FILE *fi) {
	unsigned short c2;
	
	if (fread(&c2,2,1,fi)==1) return (int) c2 ; else return EOF;
}

int wstrlen(ushort *s) {
	int i;
	for (i=0; s[i]!=0; i++) ;
	return i;
}

ushort * wstrcpy (ushort * dst, ushort * src) {
	int i;
	for (i=0; src[i]!=0; i++)
		dst[i]=src[i];
	dst[i]=0;
	return dst;
}

int wstrcmp (ushort * s1, ushort * s2) {
	int i;
	for (i=0; s1[i]==s2[i] && s1[i]!=0 && s2[i]!=0; i++) ;
	if (s1[i]==0) {
		if (s2[i]==0) return 0;
		else return -1;
	} else if (s2[i]==0) {
		return +1;
	} else {
		if (s1[i]>s2[i]) return +1;
		else return -1;
	}
}


int infohashcode(ushort * info) {
	int h,j;
	for(h=0, j=0; info[j]!=0; h = h*HASHC1 + info[j++] ) ;
	return h;
}

void infohashresize() {
	int * iht1;
	int i, hashi1, h, c;

	HASHIK++;
	hashi1 = (1 << HASHIK) - 1;
	if ((iht1=malloc(hashi1 * sizeof(int)))==NULL) error("Memory full");
	memset(iht1, 0, hashi1 * sizeof(int));
	for (i=4, c=0; i<info_full; i=i+wstrlen(infos+i)+1, c++) {
		h=abs(infohashcode(infos+i))%hashi1;
		while (iht1[h]!=0) h=(h+HASHC2)%hashi1;
		iht1[h] = i;
	}
	assert(c==infos_count);
	free(infos_hash_table);
	infos_hash_table=iht1;
	HASHI=hashi1;
}

int find_info(ushort *inf) {
	int p;
	int h;
	
	h=abs(infohashcode(inf))%HASHI;

	while ((p = infos_hash_table[h]) != 0 && wstrcmp(infos+p,inf)!=0) h=(h+HASHC2)%HASHI;
	if (p==0) {
		while (info_size <= info_full+wstrlen(inf)) 
			if ((infos=realloc(infos, (info_size+=info_delta)*sizeof(ushort)))==NULL) 
				error("Memory full");
		if (HASHI - infos_count < HASHI / 10 ) {
			infohashresize();
			h=abs(infohashcode(inf))%HASHI;
			while (infos_hash_table[h] != 0) h=(h+HASHC2)%HASHI;
		}
		infos_hash_table[h] = info_full;
		wstrcpy(infos+info_full, inf);
		info_full += wstrlen(inf)+1;
		infos_count++;
	}
	return infos_hash_table[h];
}

int state_cmp (int p, int i) {
	int j;

	for (j=0; j < letter_count; j++)
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

int new_state(int i, int k) {
	int j,c;

	for (j=first_free_cell; ; j++) {
		if (j>=cells_size-k) {
			if ((cells=realloc(cells,(cells_size+=cells_delta)*sizeof(tcell)))==NULL)
				error("Memory full");
			memset(cells+(cells_size-cells_delta),0,cells_delta*sizeof(tcell));
		}
		if (free_cell(j) && state_fits(i,j,k)) {
			for (c=0; c<=k; c++)
				if (state(i,c)!=0) save_cell(j+c, c, state(i,c));
			if (state(i,0)==0) {cells[j].tc.c=letter_count; full_cells++;}
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

	info_size = fsize / INFORELS+10; info_delta = info_size / 3+10; 
	info_full = 4; infos_count=0;
	if ((infos = malloc(info_size*sizeof(ushort)))==NULL) error("Memory full");

	cells_size = fsize / (CELLRELS * sizeof(tcell))+10; cells_delta = cells_size / 3+10;
	first_free_cell = 1; last_full_cell = 0; states_count = 0; full_cells = 0;
	if ((cells = malloc(cells_size * sizeof(tcell)))==NULL) error("Memory full");
	memset(cells, 0, cells_size * sizeof(tcell));


	HASHSK=15;
	HASHS = (1 << HASHSK) - 1;
	if ((states_hash_table=malloc(HASHS * sizeof(int)))==NULL) error("Memory full");

	memset(states_hash_table, 0, HASHS * sizeof(int));
	if ((hash_codes=malloc(HASHS * sizeof(int)))==NULL) error("Memory full");
	
	HASHIK=15;
	HASHI = (1 << HASHIK) - 1;
	if ((infos_hash_table=malloc(HASHI * sizeof(int)))==NULL) error("Memory full");
	memset(infos_hash_table, 0, HASHI * sizeof(int));
	
}

void print_statistics() {
	printf("\n\nStatistics\n===========\n");
	printf("Words processed: %d\n", line);
	printf("States created:  %d\n", states_count);
	printf("All cell count:  %d\n", last_full_cell+1);
	printf("Full cells:      %d\n", full_cells);
	printf("First free cell: %d\n", first_free_cell);
	printf("Cells allocated: %d\n", cells_size);
	printf("Infos:           %d\n", infos_count);
	printf("Infos size:      %d\n", info_full);
	printf("Info size alloc: %d\n", info_size);
}

int get_word_info(ushort *wf, ushort *inf) {
	int i, c;

	i=0;
	while ((c=fgetchar(fi))>=' ') wf[i++]=c;
	wf[i]=0;
	
	if (c==EOF) return FALSE;
	if (c!='\t') error("Delimiter not found");

	i=0;
	while ((c=fgetchar(fi))>=' ') inf[i++]=c;
	inf[i]=0;

	if (c=='\r') c=fgetchar(fi);
	if (c!='\n') error("Wrong format");
	
	return(TRUE);
}

void main (int argc, char **argv) {
	int i, j, c;
	struct stat f_stat;

	printf("compdic: Utility for compilation of wordform dictionaries\n");
	if (argc!=5)
		error("Usage:\ncompdic <alphabet> <text_dic> <info_dic> <comp_dic>");
	
	if ((fi=fopen(argv[1],"r"))==NULL) error("Error opening alphabet file");
	memset(codes, 0, 65536*sizeof(int));
	for (letter_count=1; (c=fgetchar(fi))>=' '; codes[c] = letter_count++) ;
	fclose(fi);

	stat(argv[2],&f_stat);
	init_mem(letter_count, f_stat.st_size);

	if ((fi=fopen(argv[2],"r"))==NULL) error("Error opening input file");

	clear_state(0);
	owf[0]=0;

	while (get_word_info(wf,inf)) {
		if (++line % STATFREQ == 0) {printf("%d\n",line); print_statistics();}	

		for (i=0; wf[i]!=0 && wf[i]==owf[i]; i++) ;		// find difference location
		if (wf[i]==0 || wf[i]<owf[i]) error("Dictionary file not sorted");

		for (j=wstrlen(owf)-1; j>=i; j--) state(j, codes[owf[j]]) = save_state(j+1);

		for (j=i+1; j<=wstrlen(wf); j++) clear_state(j);

		state(--j,0) = find_info(inf);

		wstrcpy(owf, wf);
	}
	fclose(fi);

	for (j=wstrlen(owf)-1; j>=0; j--) state(j, codes[owf[j]]) = save_state(j+1);

	save_cell(0, 'S', save_state(0));
	infos[0]='S';	infos[1]='&';	infos[2]='S';	infos[3]=0;
	
	fo = fopen(argv[3], "wb");
	fwrite (infos, sizeof(ushort), info_full, fo);
	fclose(fo);

	fo = fopen(argv[4], "wb");
	fwrite (cells, sizeof(tcell), last_full_cell+1, fo);
	fclose(fo);

	print_statistics();
}

