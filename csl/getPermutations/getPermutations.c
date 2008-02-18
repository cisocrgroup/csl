#include<stdio.h>
#include<stdlib.h>

#define MAXTOKENS 400
#define MAXTOKENSIZE 500
#define DELIM ','

void initA();
void permute(int left);
void swap(int x, int y);
int min(int x, int y);

int tokCount;
char *tokens[MAXTOKENS];
int a[MAXTOKENS];



int fac(int i) {
    if(i<=0) return 0;
    if(i==1) return 1;
    return i*fac(i-1);
}

int main(int argc, char** argv) {

    int a[100];
    initA();

    int size = 3;
    unsigned long long rank = 2;

    int s = size;
    int r = rank;

    int i,n;
    for(i=0; i<size; ++i) {
	printf("i=%d,s=%d,r=%d\n",i,s,r);
	
	--s;
	if(fac(s) > 0) n = (int)(r/fac(s));
	else n=0;

	printf("%d\n",n);
	r = r - n*fac(s);
    }
    
    
    


/*     initA(); */

/*     FILE* file = fopen(argv[1],"r");     */

/*     char line[5000]; */
/*     char *c, *begin; */
/*     int lineCount = 0; */
/*     while(fgets(line,5000,file)) { */
/* 	// tokenize */
/* 	begin = line; */
/* 	tokCount = 0; */
/* 	while(c = strchr(begin,DELIM)) { */
/* 	    *c = 0; */
/* 	    tokens[tokCount++] = begin; */
/* 	    begin = c+1; */
/* 	} */

/* 	// call recursive permute() */
/* 	permute(0); */

/* 	if((++lineCount % 1000) == 0) fprintf(stderr,"%d lines processed.\r",lineCount); */
/*     } */
}

void printArray() {
    int n;
    for(n=0;n<tokCount;++n) printf("%s,",tokens[a[n]]);
    printf("\n");
}

int move(int n) {
    int tmp = a[n];

//    printf("Before: ");printArray();
    while((n>=0) && (a[n-1] > tmp)) {
	a[n] = a[n-1];
	--n;
    }
    while((n+1<tokCount) && (a[n+1] < tmp)) {
	a[n] = a[n+1];
	++n;
    }
    a[n] = tmp;
//    printf("After: ");printArray();

    return n;
}

void permute(int left) {
    int i;

    if(left==tokCount-1) {
	printArray();
	return;
    }

    for(i = left; i<tokCount;++i) {
	swap(left,i);
	permute(42);
    }

}


void initA() {
    int i;
    for(i=0;i<MAXTOKENS; a[i]=i++);
}

void swap(int x, int y) {
    int tmp = a[x];
    a[x] = a[y];
    a[y] = tmp;
}

