#ifndef _SIGGEN_H_
#define _SIGGEN_H_
#define AMPLITUDE 1 //default amplitude for sig gen
#define FREQ 1 //default frequency for sig gen
#define PI 3.14159265359

void generate_sin(int amplitude, int frequency);
float* generate_square(int amplitude, int frequency);
float* generate_triangle(int amplitude, int frequency);

#endif /*_SIGGEN_H_*/