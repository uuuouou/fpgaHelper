#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define LINE_COUNT 128

int main(int argc, char* argv[])
{
    if(argc != 4){
        fprintf(stderr, "Invalid arguments!\n");
        fprintf(stderr, "Usage: Convert391 <SourceFile> <Line> <TargetFile>\n");
        exit(1);
    }
    
    FILE* fpsrc = fopen(argv[1], "r");
    if(fpsrc == NULL){
        fprintf(stderr, "Can not open source file %s\n", argv[1]);
        exit(1);
    }
    FILE* fpdes = fopen(argv[3], "w");
    if(fpdes == NULL){
        fprintf(stderr, "Can not open target file %s\n", argv[3]);
        fclose(fpsrc);
        exit(1);
    }
    
    int i, j, line = atoi(argv[2]);
    int high, mid, low, res;
    char s[391 * 3 + 2], *p;
    const double mag = pow(2, 23);
    for(i = 0; i < line; ++i){
        fgets(s, sizeof(s), fpsrc);
        for(j = 0, p = s + 15; j < LINE_COUNT; ++j, p += 9){
            sscanf(p, "%x %x %x", &high, &mid, &low);
            res = high << 16 | mid << 8 | low;
            if(high & 0x00000080) res |= 0xFF000000;
            fprintf(fpdes, "%.17f\n", res / mag);
        }
    }
    
    fclose(fpsrc);
    fclose(fpdes);
    return 0;
}
