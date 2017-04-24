#include "siggen.h"
#include <stm32f4xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void generate_sin(int amplitude, int frequency,float* mem, int memLength){
    for(int i = 0; i < memLength; i++){
        mem[i] = amplitude * ( sin((2 * PI) * i * frequency) );
    }
}


float* generate_square(int amplitude, int frequency){ //square wave
    float out[10000];
    for(int i = 0; i < 10000; i++){
        out[i] = (i++ % (1/frequency) ) < amplitude ? amplitude : 0;
    }
    return out;
}
float* generate_triangle(int amplitude, int frequency){ //triangle wave
    float out[10000];
    for(int i = 0; i < 10000; i++){
        out[i] = abs((i++ % (1/frequency) ) - amplitude);
    }
    return out;
}