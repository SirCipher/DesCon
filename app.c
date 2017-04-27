#include "app.h"

void app_set_mode(int mode){
	char* newMode = malloc(sizeof(char) * 15);
  sprintf(newMode,"m:%d", mode);
	send_String(USART3, newMode);
	free(newMode);
}
