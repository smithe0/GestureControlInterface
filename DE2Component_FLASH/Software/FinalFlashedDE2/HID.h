#ifndef HID_CLASS_REQUESTS
#define HID_CLASS_REQUESTS

void HID_Get_Report();	//Sends HID report over control endpoint
void HID_Get_Idle();
void HID_Get_Protocol();
void HID_Set_Report();
void HID_Set_Idle(); 	//Sets idle value of HID keyboard
void HID_Set_Protocol();

#endif
