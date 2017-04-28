#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "reading.h"
#include "unit_dict.h"

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

reading_t reading_new(float v, char u, int s){
    reading_t reading = NULL;

    reading = malloc(sizeof(struct reading_t));

    assert(reading != NULL);

    reading->scale = s;
    reading->unit = u;
    reading->value = v;


    return reading;
}

float reading_get_value(reading_t reading){
    return reading->value;
}
int reading_get_scale(reading_t reading){
    return reading->scale;
}
char reading_get_unit(reading_t reading){
    return reading->unit;
}

void reading_set_value(reading_t reading, float value){
    reading->value = value;
}
void reading_set_scale(reading_t reading, int scale){
    reading->scale = scale;
}
void reading_set_reading(reading_t reading, char unit){
    reading->unit = unit;
}


float reading_get_scaled_value(reading_t reading){
    return reading->value * reading_scales[reading->scale];
}

float reading_get_normalised_value(reading_t reading){
    float val = reading->value;
    int scale = reading->scale;
    while(val>= 10){
        val /= 10;
        scale++;
    }
    return val;
}

void reading_get_message_form_from_units(char* memory,float val, char unit, int scale){
    sprintf(memory,"%.4f%c%d",val, unit, scale);
}

void reading_get_message_form(reading_t reading,char *memory){
    /*possibly this one*/
    /*
     reading_get_message(*memory,reading->val,reading->unit,reading->scale)
    */
    reading_get_message_form_from_units(memory,reading->value,reading->unit,reading->scale);
}

float reading_get_val_prefix(reading_t reading, char *prefix){
    float val = reading->value;
    int scale = reading->scale;
    float scaledValue = val*reading_scales[scale];
    float magnitude = log10(scaledValue);
    if(magnitude>=9){
        *prefix = SCALE_POS9_SH;
    }
    else if(magnitude>= 6){
        *prefix = SCALE_POS6_SH;
    }
    else if(magnitude>= 3){
        *prefix = SCALE_POS3_SH;
    }
    else if(magnitude>= 0){
        *prefix = ' ';
    }
    else if(magnitude>= -3){
        *prefix = SCALE_NEG6_SH;
    }
    else if(magnitude>= -6){
        *prefix = SCALE_NEG6_SH;
    }
    else {
        *prefix = ' ';
    }

    return scaledValue;

}


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

void reading_get_lcd_string(reading_t reading, char* memory){
    char prefix;
    float val = reading_get_val_prefix(reading,&prefix);
    sprintf(memory,"%.4f%c%c", val, prefix, reading->unit);
}
