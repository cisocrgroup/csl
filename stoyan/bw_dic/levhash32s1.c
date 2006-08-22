#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <strings.h>
#include <assert.h>

#define MAXLETTERS	100
#define FALSE 0
#define TRUE  1
#define MAXRESULTS	10000

#define t_ind(lvt,chv,cs)	( (cs)*2 + (chv) * (lvt).coresets * 2 )
#define f_ind(lvt,k,cs)		( (cs) + (k) * (lvt).coresets )

typedef unsigned char uchar;
typedef unsigned long long uulong;

// a cell of the compressed transition table
typedef union {
    long long i;
    struct {
	int b,c;
    } tc;
} tcell;

// ???
typedef struct {
    tcell * cells;
    uchar * strings;
    int initial;
} tdict;

// all data from the levenshtein automaton
typedef struct {
    int *tab; // the transition table
    int *fin; // final state information
    int k, coresets; // k=upper distance bound, coresets=?
    uulong z2k1, z2k2;
} tlevtab;
	

// alphabet data
typedef struct {
    int letter_count;
    int codes[256];
    uchar letters[256];
} tazb;


typedef struct {
    uulong *charvec;
    tazb *azbuka;
    tlevtab *lvt;
    int n;
} tcharvecs;

uulong zff, z10;

// ???
int trans12[5] = {0, 1, 26, 29, 27};
int trans13[5] = {0, 1, 191, 195, 192};
int trans23[30] = {0, 1, 2, 137, 147, 149, 150, 154, 155, 157, 158, 159, 167, 168, 170, 171, 175, 176, 178, 179, 180, 182, 183, 187, 188, 190, 191, 192, 194, 195};


//get memory for charvec, set pointers to alphabet and levtab
void init_charvec(tcharvecs * chv, tazb *azbuka, tlevtab *lvt) {
    chv->charvec = malloc(azbuka->letter_count*sizeof(uulong));
    chv->azbuka = azbuka;
    chv->lvt = lvt;
}

// calculate charvec for pattern w, store it into chv
void calc_charvec(uchar *w, tcharvecs * chv) {
    uulong c;
    int i;

    memset(chv->charvec, 0, chv->azbuka->letter_count*sizeof(uulong));

    chv->n=strlen(w);
    for (c=z10, i=0; i< chv->n; i++, c>>=1) {
	chv->charvec[chv->azbuka->codes[w[i]]] |= c;
    }
}

// calculate a distinct substring of a charvec
int calc_k_charvec(tcharvecs * chv, uchar c, int i) {
    uulong r;

    r = (chv->charvec[c] >> (64-(2 * chv->lvt->k + 1 + i))) & chv->lvt->z2k1;
    if (chv->n - i < 2 * chv->lvt->k + 1)
	r = ((r >> (2 * chv->lvt->k + 1 - (chv->n - i))) | (zff << ((chv->n - i)+1))) & chv->lvt->z2k2;
    return (int) r;
}

// read alphabet from a filehandle
void read_azbuka(tazb * azbuka, FILE *fi) {
    int c;
    memset(azbuka->codes, 0, 256*sizeof(int));
    memset(azbuka->letters, 0, 256);
    for (azbuka->letter_count=1; (c=getc(fi))>=' '; azbuka->letter_count++) {
	azbuka->codes[c] = azbuka->letter_count;
	azbuka->letters[azbuka->letter_count]=c;
    }
}


// read levtab from a filehandle
void read_levtable(tlevtab * lvt, FILE *fi) {
    int cs, c, t;

    fscanf(fi,"%d\n",&lvt->k);
    fscanf(fi,"%d\n",&lvt->coresets);

    lvt->z2k1 = 1ll; lvt->z2k1 <<= 2*lvt->k + 1; lvt->z2k1--;
    lvt->z2k2 = 1ll; lvt->z2k2 <<= 2*lvt->k + 2; lvt->z2k2--;


    lvt->tab = malloc(lvt->z2k2*lvt->coresets*2* sizeof(int));
    lvt->fin = malloc((2*lvt->k+1)*lvt->coresets* sizeof(int));


    for (c=0, t=0; c<lvt->z2k2; c++) {
	for (cs=0; cs<lvt->coresets; cs++) {
	    fscanf(fi,"%d,",&lvt->tab[t++]);
	    fscanf(fi,"%d,",&lvt->tab[t++]);

	}
	fscanf(fi,"\n");
    }

    for (c=0, t=0; c<2*lvt->k+1; c++) {
	for (cs=0; cs<lvt->coresets; cs++) {
	    fscanf(fi,"%d,",&lvt->fin[t++]);
	}
	fscanf(fi,"\n");
    }

}

// read dic from a filehandle
void read_dict (tdict * dict, FILE * fi, int size) {
    dict->cells = malloc(size);
    fread(dict->cells, 1, size, fi);
    dict->initial = dict->cells[0].tc.b; // that's the root of the dic
    dict->strings = (uchar *) & dict->cells[dict->cells[1].tc.b]; // that's the beginning of the 'fast-traversable' strings
}


uchar res[MAXRESULTS][MAXLETTERS];
int nres;

tdict * dict; 
uchar s[MAXLETTERS];
uchar sr[MAXLETTERS];
tcharvecs *chv_; tlevtab *lvt_; 
tcharvecs *chv; tlevtab *lvt; 
tazb *azb;
int distance1, distance2;
int rev; 

//                      state  levstate pat_pos   offset in s
void find_intersection3(int d, int cs,   int i,   int j) {
    int t, c1;
    uchar * c;

    if (dict->cells[d].tc.c==0 && //if state (d) is final
	( chv->n - i < 2 * lvt->k + 1 ) && 
	lvt->fin[ f_ind( *lvt, 2 * lvt->k - ( chv->n - i ), cs ) ] > distance2 ) { // levtable-state is final with a distance lower than distance2

	// add current str to result set
	if (rev) {
	    for (t=strlen(s)-1, c1=0; t>=0; t--, c1++) sr[c1]=s[t]; sr[c1]=0; // store reversed s in rs
			
//			for (t=0; t<nres && strcmp(sr, res[t])!=0; t++) ; 
//			if (t==nres) 
	    strcpy(res[nres++],sr);
	} else {
//			for (t=0; t<nres && strcmp(s, res[t])!=0; t++) ; 
//			if (t==nres) 
	    strcpy(res[nres++],s);
	}
    }
    s[j+1]=0;
    c = dict->strings + dict->cells[d].tc.b; // get fast-traversable string
    for ( ; *c!=0; c++)
	if (lvt->tab[t=t_ind(*lvt, calc_k_charvec(chv,*c,i),cs)] != -1) {
	    s[j]=azb->letters[*c];
	    find_intersection3(dict->cells[d+*c].tc.b, lvt->tab[t], i+lvt->tab[t+1], j+1);
	}
    s[j]=0;
}


// match first half of the pattern (or reverse pattern)
//                      state  levstate pat_pos   offset in s
void find_intersection1(int d, int cs,   int i,     int j) {
    int t;
    uchar * c;

    // if s is an approximate match for first half, try to continue with second half
    if ( ( chv_->n - i ) < ( 2 * lvt_->k + 1 ) && 
	 lvt_->fin[ f_ind( *lvt_, 2 * lvt_->k - ( chv_->n - i ), cs ) ] > distance1 ) {
	find_intersection3(d, 0, 0, j);
    }
    s[j+1]=0;
    c = dict->strings + dict->cells[d].tc.b;
    for ( ; *c!=0; c++) {
	if (lvt_->tab[t=t_ind(*lvt_, calc_k_charvec(chv_,*c,i),cs)] != -1) {
	    s[j]=azb->letters[*c];
	    find_intersection1(dict->cells[d+*c].tc.b, lvt_->tab[t], i+lvt_->tab[t+1], j+1);
	}
    }
    s[j]=0;
}


// walk w in dict, beginning from state d
int find_word(int d, uchar * w) {
    int k,c;
    for (k=0; w[k]!=0; k++) {
	c = azb->codes[w[k]];
	if (dict->cells[d+c].tc.c == c) d = dict->cells[d+c].tc.b;
	else return FALSE;
    }
    return d;
}


// for querying without bw-dics
void find_intersection( int d, int cs, int i, int j ) {
    int t;
    uchar * c;

    if (dict->cells[d].tc.c==0 && chv->n-i<2*lvt->k+1 && lvt->fin[f_ind(*lvt,2*lvt->k-(chv->n-i),cs)]!=-1) {
	if (nres<MAXRESULTS-1) strcpy(res[nres++],s);
    }
    s[j+1]=0;
    c = dict->strings + dict->cells[d].tc.b;
    for ( ; *c!=0; c++)
	if (lvt->tab[t=t_ind(*lvt, calc_k_charvec(chv,*c,i),cs)] != -1) {
	    s[j]=azb->letters[*c];
	    find_intersection(dict->cells[d+*c].tc.b, lvt->tab[t], i+lvt->tab[t+1], j+1);
	}
    s[j]=0;
}



int get_proc_time() {
    FILE *fi;

    int pid,ppid,pgrp,session,tty,tpgid,flags,minflt,cminflt,majflt,cmajflt,utime,stime,cutime,cstime;
    char state,comm[100];

    fi = fopen("/proc/self/stat","r");
    fscanf(fi,"%d %s %c %d %d %d %d %d %u %u %u %u %u %d %d %d %d", &pid,comm,&state,&ppid,&pgrp,&session,&tty,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime);
    fclose(fi);
    return utime+stime;
}


void main (int argc, uchar **argv) {

    tdict sdict, rdict;
    tlevtab lvt1, lvt2, lvt3;
    tazb azbuka;
    tcharvecs chv1, chv2, chv3;

    uchar p[MAXLETTERS], pr[MAXLETTERS];
    uchar p1[MAXLETTERS], pr1[MAXLETTERS];
    uchar p2[MAXLETTERS], pr2[MAXLETTERS];
    FILE *fo, *fi;

    struct stat f_stat;

    uchar *text;
    uchar *l;
    int k1;

    int start, end;
    double elapsed1;
    int n_words, n_all_res;
	
    int i, c1, c2, m, n;

    zff = 0xffffffffffffffffll;
    z10 = 1ll << 63;

    if (argc!=8) {
	printf("levhash <dic_alphabet> <dict> <rev_dict> <lev_data_1> <lev_data_2> <lev_data_3> <text>\n\n");
	exit(1);
    }


    fi = fopen(argv[1],"rb");
    read_azbuka(&azbuka, fi);
    fclose(fi);

    stat(argv[2],&f_stat);
    fi = fopen(argv[2],"rb");
    read_dict(&sdict,fi,f_stat.st_size);
    fclose(fi);

    stat(argv[3],&f_stat);
    fi = fopen(argv[3],"rb");
    read_dict(&rdict,fi,f_stat.st_size);
    fclose(fi);

    fi = fopen(argv[4],"rb");
    read_levtable(&lvt1, fi);
    fclose(fi);

    fi = fopen(argv[5],"rb");
    read_levtable(&lvt2, fi);
    fclose(fi);

    fi = fopen(argv[6],"rb");
    read_levtable(&lvt3, fi);
    fclose(fi);


    init_charvec(&chv1, &azbuka, &lvt1);
    init_charvec(&chv2, &azbuka, &lvt2);
    init_charvec(&chv3, &azbuka, &lvt3);


    stat(argv[7],&f_stat);
    fi = fopen(argv[7],"rb");
    text = malloc(f_stat.st_size);
    fread(text, 1, f_stat.st_size, fi);
    fclose(fi);

    n_words = 0;
    n_all_res = 0;
    start = get_proc_time();

    s[0]=0; k1=0;
    dict=&sdict; lvt=&lvt3; chv = &chv3; azb = &azbuka;
/*
  for (l=text; l < text+f_stat.st_size; l++) {
  if (*l<' ') {
  n_words++;
  s[k1] = 0;
  calc_charvec(s, chv);

  nres=0;
  s[0]=0;
  find_intersection(dict->initial,0,0,0);
//for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");
n_all_res += nres;
s[0]=0; k1=0;
} else s[k1++] = *l;
}

end = get_proc_time();
elapsed1 = ((double) (end - start)) / (double) 100;
printf("*******************************************************************************\n");
printf("********** Classical correction method with Levenshtein table lookup***********\n");
printf("*******************************************************************************\n");
printf("Number of word: 					%d  \n", n_words);
//	printf("Summary automata simulation and correction time:	%13.10f sec. \n",elapsed1);
printf("Average automaton simulation and corr. time (per wrd):	%13.10f sec. \n",elapsed1/n_words);
printf("Avarage number of corrections per word: 		%13.10f\n", (float) n_all_res / n_words);
//	printf("Number of corrections:			 		%13.10f  \n", (float) n_all_res);
printf("*******************************************************************************\n");

*/
    n_words = 0;
    n_all_res = 0;
    start = get_proc_time();

    lvt=&lvt3; chv = &chv3;
    lvt_=&lvt1; chv_ = &chv1;
    s[0]=0; 
    k1=0; // char count 

    // text loop
    for (l=text; l < text+f_stat.st_size; l++) {
	if (*l<' ') {
	    n_words++;
	    s[k1] = 0;
			
	    // split pattern into 2 halves
	    n = strlen(s); m = n/2;
	    for (i=0, c1=0, c2=0; i<n; i++) 
		if (i<m) p1[c1++]=s[i];
		else p2[c2++]=s[i];
	    p1[c1]=0; p2[c2]=0;

	    // compute reverse patterns for both halves
	    for (i=strlen(p1)-1, c1=0; i>=0; i--, c1++) pr1[c1]=p1[i]; pr1[c1]=0;
	    for (i=strlen(p2)-1, c2=0; i>=0; i--, c2++) pr2[c2]=p2[i]; pr2[c2]=0;

//printf("s='%s', p1='%s', p2='%s', pr1='%s', pr2='%s'\n",s,p1,p2,pr1,pr2);			

	    nres=0;

	    // 0 | 0,1,2,3 errors
	    rev = FALSE;
	    dict= &sdict;
	    if ((i=find_word(dict->initial, p1))) {
		chv = &chv3; lvt = &lvt3;
		distance2 = -1;
		calc_charvec(p2,chv);
		strcpy(s, p1);
		find_intersection3(i, 0, 0, strlen(p1));
	    }
//for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");

	    // 1 | 0,1,2 errors
	    chv = &chv2; lvt = &lvt2;
	    s[0]=0;			 
	    calc_charvec(p1, chv_);
	    calc_charvec(p2, chv);
	    distance1=0; distance2=-1;
	    find_intersection1(dict->initial,0,0,0);
//for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");
			
	    // 2,3 | 0 errors
	    rev = TRUE;
	    dict=&rdict; 
	    if ((i=find_word(dict->initial, pr2))) {
		chv = &chv3; lvt = &lvt3;
		calc_charvec(pr1,chv);
		strcpy(s, pr2);
		distance2=1;
		find_intersection3(i, 0, 0, strlen(pr2));
	    }
//for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");

	    // 2 | 1 errors
	    chv = &chv2; lvt = &lvt2;
	    s[0]=0;
	    calc_charvec(pr2, chv_);
	    calc_charvec(pr1, chv);
	    distance2=1; distance1=0;
	    find_intersection1(dict->initial,0,0,0);
//for (c1=0; c1<nres; c1++) printf("%s, ",res[c1]); printf("\n");
			
	    n_all_res += nres;
	    s[0]=0; k1=0;
	} // if word is complete
	else s[k1++] = *l; 
    }

    end = get_proc_time();
    elapsed1 = ((double) (end - start)) / (double) 100;
    printf("*******************************************************************************\n");
    printf("********** New correction method with lookup in reversed dictionary ***********\n");
    printf("*******************************************************************************\n");
    printf("Number of word: 					%d  \n", n_words);
//	printf("Summary automata simulation and correction time:	%13.10f sec. \n",elapsed1);
    printf("Average automaton simulation and corr. time (per wrd):	%13.10f sec. \n",elapsed1/n_words);
    printf("Avarage number of corrections per word: 		%13.10f\n", (float) n_all_res / n_words);
//	printf("Number of corrections:			 		%13.10f  \n", (float) n_all_res);
    printf("*******************************************************************************\n");


}

