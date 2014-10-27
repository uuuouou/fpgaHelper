#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define BITS     24


int main(int argc, char* argv[])
{
    if(argc != 3){
        fprintf(stderr, "Invalid arguments!\n");
        fprintf(stderr, "Usage: GenerateSin <SamplePoints> <TargetFile>\n");
        exit(1);
    }
    
    FILE* fp = fopen(argv[2], "w");
    if(fp == NULL){
        fprintf(stderr, "Can not open target file %s\n", argv[2]);
        exit(1);
    }
    int i, points = atoi(argv[1]);
    double v;
    const double PI = acos(-1.0), mag = pow(2, BITS-1), positiveLimit = (mag-1)/mag;
    fprintf(fp, "%d\n", points);
    for(i = 0; i < points; ++i){
        v = sin((2*PI)/points * i);
        if(v > positiveLimit) v = positiveLimit;
        fprintf(fp, "%.17f\n", v);
    }
    fclose(fp);
    
    return 0;
}
