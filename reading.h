#ifndef READING_T_H
#define READING_T_H

#include <math.h>

typedef struct reading_t *reading_t;


reading_t reading_new(int v, int u, int s);

float reading_get_value(reading_t reading);
int reading_get_scale(reading_t reading);
char reading_get_unit(reading_t reading);

void reading_set_value(reading_t reading,float value);
void reading_set_scale(reading_t reading, int scale);
void reading_set_unit(reading_t reading, char unit);

int reading_need_scale(reading_t reading, int max, int min);

float reading_get_scaled_value(reading_t reading);
void reading_get_lcd_string(reading_t reading,char* memory);

void reading_get_message_form_from_units(char* memory,float val, int scale, char unit);
void reading_get_message_form(reading_t reading, char* memory);
void reading_get_si(reading_t reading);
float reading_get_val_prefix(reading_t reading, char* prefix);

#endif
