#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "reading.h"
#include "unit_dict.h"

struct reading_t {
    char unit;
    int scale;
    float value;
};

reading_t reading_new(int v, int u, int s){
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


/* warning: probably slow. */
float reading_get_scaled_value(reading_t reading){
    return reading->value * pow(10,(float)reading->scale);
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

void reading_get_message_form_from_units(char* memory,float val, int scale, char unit){
    sprintf(memory,"%.4f%c%i\n",val, unit, scale);
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
    while(val>= 999.0f && scale < 9){
        scale += 3;
        val /= 1000;
    }
    while(val<= 0.01f && scale > -9){
        scale -= 3;
        val *= 1000;
    }
    if(scale >=9){
        *prefix = SCALE_POS9_SH;
    }
    else if(scale >= 6){
        *prefix = SCALE_POS6_SH;
    }
    else if(scale >= 3){
        *prefix = SCALE_POS3_SH;
    }
    else if(scale >= 0){
        *prefix = ' ';
    }
    else if(scale >= -3){
        *prefix = SCALE_NEG6_SH;
    }
    else if(scale >= 6){
        *prefix = SCALE_NEG6_SH;
    }
    else {
        *prefix = '?';
    }

    return val;

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
    sprintf(memory,"%.4f %c%c", val, prefix, reading->unit);
}
