#include "utils.h"


double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


char *int2bin(int a, char *buffer, int buf_size){
		int i;
		buffer += (buf_size - 1);
	
		for(i = 31; i >= 0; i--){
			*buffer-- = (a & 1) + '0';
			a >>= 1;
		}
		
		return buffer;
}
