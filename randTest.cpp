#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define N 100000
#define k 10
#define m RAND_MAX

double samples[N];
double bins[k];
double ratio = m/k*1.0;

void makeSamples(){
	for (int i =0; i < N; i++){
		samples[i] =  ((double) rand());
		bins[(int) floor(samples[i]/ratio)] = bins[(int) floor(samples[i]/ratio)] + 1;
		//printf("%4.8f\n" , samples[i] );
	}
	
	
}


int main (int argc,char **argv){
	srand(time(NULL));
	
	makeSamples();
	
	/*
	double count = 0;
	for(int i =0;i < k;i++){
		printf("Bin[%i] = %f\n",i,bins[i]);
		count += bins[i];
	}
	*/
	//Generate chi values
	double chiVal = 0;
	
	for(int i = 0; i < k; i++){
		//System.out.println(chiVal);
		//System.out.println((Math.pow( bins[i]-(n/k),2 ) )/(n/k));
		chiVal += (pow(bins[i]-(N/k),2 ) )/(N/k);
	}
	printf("Chi-Square Value = %f\n",chiVal);
	
}
