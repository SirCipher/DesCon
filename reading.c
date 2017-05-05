#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "reading.h"
#include "unit_dict.h"


/* As scaling is non-linear, but its the same across
 * all the readings so we store them in the
 */
const int reading_scales[] = {
    1,
    10,
    50,
    100
};

struct reading_t {
    char unit;
    int scale;
    float value;
};

/* Constructor */
reading_t reading_new(float v, char u, int s){
    reading_t reading = NULL;

    reading = malloc(sizeof(struct reading_t));

    assert(reading != NULL);

    reading->scale = s;
    reading->unit = u;
    reading->value = v;


    return reading;
}

/* Getters */
float reading_get_value(reading_t reading){
    return reading->value;
}
int reading_get_scale(reading_t reading){
    return reading->scale;
}
char reading_get_unit(reading_t reading){
    return reading->unit;
}

/* Setters */
void reading_set_value(reading_t reading, float value){
    reading->value = value;
}
void reading_set_scale(reading_t reading, int scale){
    reading->scale = scale;
}
void reading_set_reading(reading_t reading, char unit){
    reading->unit = unit;
}


/* Scale values by scale value  */
float reading_get_scaled_value(reading_t reading){
    return reading->value * reading_scales[reading->scale];
}


/* convert reading into string in form of <value><unit><scale>*/
void reading_get_message_form_from_units(char* memory,float val, char unit, int scale){
    sprintf(memory,"%.4f%c%d",val, unit, scale);
}

/* produce full named string */
void reading_get_message_form(reading_t reading,char *memory){

    reading_get_message_form_from_units(memory,reading->value,reading->unit,reading->scale);
}


/* set an input character to the correct Si prefix
 * and output the scaled value */
float reading_get_val_prefix(reading_t reading, char *prefix){
    float val = reading->value;
    int scale = reading->scale;
    float scaledValue = val*reading_scales[scale];
    float magnitude = log10(scaledValue);
    if(magnitude>=9){
        *prefix = SCALE_POS9_SH;
        scaledValue/=pow(10,9);
    }
    else if(magnitude>= 6){
        *prefix = SCALE_POS6_SH;
        scaledValue /= pow(10,6);
    }
    else if(magnitude>= 3){
        *prefix = SCALE_POS3_SH;
        scaledValue /= pow(10,3);
    }
    else if(magnitude>= 0){
        *prefix = ' ';
    }
    else if(magnitude>= -3){
        *prefix = SCALE_NEG3_SH;
        scaledValue *=pow(10,3);
    }
    else if(magnitude>= -6){
        *prefix = SCALE_NEG6_SH;

        scaledValue *=pow(10,6);
    }
    else {
        *prefix = ' ';
    }

    return scaledValue;

}

/* Detected whether or not the reading requires scaling*/
int reading_need_scale(reading_t reading, int max, int min){
    if(reading->value >= max){
        return 1;
    }
    else if(reading->value <= min){
        return -1;
    }
    else
        return 0;
}

/* Format string to one suitable for LCD */

void reading_get_lcd_string(reading_t reading, char* memory){
    char prefix;
    float val = reading_get_val_prefix(reading,&prefix);
    sprintf(memory,"%.4f%c%c", val, prefix, reading->unit);
}
