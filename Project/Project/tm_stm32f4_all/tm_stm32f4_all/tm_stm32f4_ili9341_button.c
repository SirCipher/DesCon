/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "stm32f4_ili9341_button.h"

ILI9341_Button_t ILI9341_Buttons[ILI9341_BUTTON_MAX_BUTTONS];

int8_t ILI9341_Button_Add(ILI9341_Button_t* button) {
	uint8_t id = 0;
	while ((ILI9341_Buttons[id].flags & BUTTON_FLAG_USED) && (id < ILI9341_BUTTON_MAX_BUTTONS)) {
		id++;
	}
	
	if (id == ILI9341_BUTTON_MAX_BUTTONS) {
		//Max button reached
		return -1;
	}
	ILI9341_Buttons[id].x = button->x;
	ILI9341_Buttons[id].y = button->y;
	ILI9341_Buttons[id].width = button->width;
	ILI9341_Buttons[id].height = button->height;
	ILI9341_Buttons[id].background = button->background;
	ILI9341_Buttons[id].borderColor = button->borderColor;
	ILI9341_Buttons[id].flags = button->flags | BUTTON_FLAG_USED | BUTTON_FLAG_ENABLED;
	ILI9341_Buttons[id].label = button->label;
	ILI9341_Buttons[id].color = button->color;
	ILI9341_Buttons[id].font = button->font;
	ILI9341_Buttons[id].image = button->image;
	
	return id;
}

void ILI9341_Button_DrawAll(void) {
	uint8_t id = 0;
	for (id = 0; id < ILI9341_BUTTON_MAX_BUTTONS; id++) {
		if ((ILI9341_Buttons[id].flags & BUTTON_FLAG_USED)) {
			//Button enabled, draw it to screen
			ILI9341_Button_Draw(id);
		}
	}
}

ErrorStatus ILI9341_Button_Draw(uint8_t id) {
	uint16_t fontWidth, fontHeight, x, y, i, j;
	if ((ILI9341_Buttons[id].flags & BUTTON_FLAG_USED) == 0) {
		//Button not enabled
		return ERROR;
	}
	//Draw label
	if (ILI9341_Buttons[id].flags & BUTTON_FLAG_IMAGE) {
		//Draw picture
		for (i = 0; i < ILI9341_Buttons[id].width; i++) {
			for (j = 0; j < ILI9341_Buttons[id].height; j++) {
				ILI9341_DrawPixel(	ILI9341_Buttons[id].x + i,
										ILI9341_Buttons[id].y + j,
										*(uint16_t *)(ILI9341_Buttons[id].image + j * ILI9341_Buttons[id].width + i)
				);
			}
		}
	} else {
		//Background
		ILI9341_DrawFilledRectangle(	ILI9341_Buttons[id].x,
										ILI9341_Buttons[id].y,
										ILI9341_Buttons[id].x + ILI9341_Buttons[id].width,
										ILI9341_Buttons[id].y + ILI9341_Buttons[id].height,
										ILI9341_Buttons[id].background );
		
	}
	
	//Display label
	if ((ILI9341_Buttons[id].flags & BUTTON_FLAG_NOLABEL) == 0) {
		ILI9341_GetStringSize(ILI9341_Buttons[id].label, ILI9341_Buttons[id].font, &fontWidth, &fontHeight);
		x = ILI9341_Buttons[id].x + ILI9341_Buttons[id].width / 2 - (fontWidth / 2);
		y = ILI9341_Buttons[id].y + ILI9341_Buttons[id].height / 2 - (fontHeight / 2);
		ILI9341_Puts(x, y, ILI9341_Buttons[id].label, ILI9341_Buttons[id].font, ILI9341_Buttons[id].color, ILI9341_Buttons[id].background);
	}
	
	//Border
	if ((ILI9341_Buttons[id].flags & BUTTON_FLAG_NOBORDER) == 0) {
		//Border enabled
		ILI9341_DrawRectangle(	ILI9341_Buttons[id].x,
									ILI9341_Buttons[id].y,
									ILI9341_Buttons[id].x + ILI9341_Buttons[id].width,
									ILI9341_Buttons[id].y + ILI9341_Buttons[id].height,
									ILI9341_Buttons[id].borderColor );
	}
	
	return SUCCESS;
}

int8_t ILI9341_Button_Touch(STMPE811_TouchData* TouchData) {
	uint8_t id;
	for (id = 0; id < ILI9341_BUTTON_MAX_BUTTONS; id++) {
		//If button not enabled, ignore it
		if ((ILI9341_Buttons[id].flags & BUTTON_FLAG_ENABLED) == 0) {
			continue;
		}
		//If touch data is inside button somewhere
		if (
			(TouchData->x > ILI9341_Buttons[id].x && TouchData->x < (ILI9341_Buttons[id].x + ILI9341_Buttons[id].width)) &&
			(TouchData->y > ILI9341_Buttons[id].y && TouchData->y < (ILI9341_Buttons[id].y + ILI9341_Buttons[id].height))
		) {
			//Return its id
			return id;
		}
	}
	
	//No one was pressed
	return -1;
}


void ILI9341_Button_Enable(uint8_t id) {
	//Add enabled flag
	ILI9341_Buttons[id].flags |= BUTTON_FLAG_ENABLED;
}

void ILI9341_Button_Disable(uint8_t id) {
	//Remove enabled flag
	ILI9341_Buttons[id].flags &= ~BUTTON_FLAG_ENABLED;
}

void ILI9341_Button_DeleteAll(void) {
	uint8_t i;
	for (i = 0; i < ILI9341_BUTTON_MAX_BUTTONS; i++) {
		ILI9341_Button_Delete(i);
	}
}

void ILI9341_Button_Delete(uint8_t id) {
	//Just remove USED flag from button
	ILI9341_Buttons[id].flags &= ~BUTTON_FLAG_USED;
}

