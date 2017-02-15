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
#include "stm32f4_usb_hid_host.h"

/* Private variables */
extern USB_OTG_CORE_HANDLE				USB_OTG_Core;
extern USBH_HOST						USB_Host;
extern USB_HIDHOST_Result_t 			USB_HIDHOST_INT_Result;
extern USB_HIDHOST_Keyboard_t 		USB_HIDHOST_INT_Keyboard;
extern USB_HIDHOST_Mouse_t 			USB_HIDHOST_INT_Mouse;
uint8_t USB_HIDHOST_Initialized = 	0;

void USB_HIDHOST_Init(void) {
	/* Init Host Library */
	USBH_Init(&USB_OTG_Core, 
#ifdef USE_USB_OTG_FS
            USB_OTG_FS_CORE_ID,
#else
            USB_OTG_HS_CORE_ID,
#endif
            &USB_Host,
            &HID_cb, 
            &USR_Callbacks);
	/* We are initialized */
	USB_HIDHOST_Initialized = 1;
}

USB_HIDHOST_Result_t USB_HIDHOST_Process(void) {
	/* Not initialized */
	if (!USB_HIDHOST_Initialized) {
		return USB_HIDHOST_Result_LibraryNotInitialized;
	}
	
	/* Host Task handler */
	USBH_Process(&USB_OTG_Core, &USB_Host);
	
	/* Return device status */
	return USB_HIDHOST_Device();
}

USB_HIDHOST_Result_t USB_HIDHOST_Device(void) {
	/* Not initialized */
	if (!USB_HIDHOST_Initialized) {
		return USB_HIDHOST_Result_LibraryNotInitialized;
	}
	return USB_HIDHOST_INT_Result;
}

USB_HIDHOST_Result_t USB_HIDHOST_ReadKeyboard(USB_HIDHOST_Keyboard_t* Keyboard) {
	if (USB_HIDHOST_INT_Result != USB_HIDHOST_Result_KeyboardConnected) {
		/* Keyboard not initialized */
		return USB_HIDHOST_Result_Error;
	}
	
	/* Fill data */
	Keyboard->Button = USB_HIDHOST_INT_Keyboard.Button;
	Keyboard->ButtonStatus = USB_HIDHOST_INT_Keyboard.ButtonStatus;
	
	/* Reset internal data */
#if USB_HIDHOST_REINITIALIZE_KEYBOARD_AFTER_READ > 0
	USB_HIDHOST_INT_Keyboard.ButtonStatus = USB_HIDHOST_Button_Released;
	USB_HIDHOST_INT_Keyboard.Button = 0;
#endif
	
	/* Return keyboard connected */
	return USB_HIDHOST_Result_KeyboardConnected;
}

USB_HIDHOST_Result_t USB_HIDHOST_ReadMouse(USB_HIDHOST_Mouse_t* Mouse) {
	if (USB_HIDHOST_INT_Result != USB_HIDHOST_Result_MouseConnected) {
		/* Mouse is not connected */
		return USB_HIDHOST_Result_Error;
	}

	/* Fill data */
	Mouse->AbsoluteX =    USB_HIDHOST_INT_Mouse.AbsoluteX;
	Mouse->AbsoluteY =    USB_HIDHOST_INT_Mouse.AbsoluteY;
	Mouse->DiffX =        USB_HIDHOST_INT_Mouse.DiffX;
	Mouse->DiffY =        USB_HIDHOST_INT_Mouse.DiffY;
	Mouse->LeftButton =   USB_HIDHOST_INT_Mouse.LeftButton;
	Mouse->MiddleButton = USB_HIDHOST_INT_Mouse.MiddleButton;
	Mouse->RightButton =  USB_HIDHOST_INT_Mouse.RightButton;
	
	/* Reset internal data */
	/* Difference from last call is 0 */
	USB_HIDHOST_INT_Mouse.DiffX = 0;
	USB_HIDHOST_INT_Mouse.DiffY = 0;
	
	/* Buttons are not pressed any more */
#if USB_HIDHOST_REINITIALIZE_MOUSE_AFTER_READ > 0
	USB_HIDHOST_INT_Mouse.LeftButton = USB_HIDHOST_Button_Released;
	USB_HIDHOST_INT_Mouse.MiddleButton = USB_HIDHOST_Button_Released;
	USB_HIDHOST_INT_Mouse.RightButton = USB_HIDHOST_Button_Released;
#endif
	
	/* Return mouse connected */
	return USB_HIDHOST_Result_MouseConnected;
}

