/*
	usb_ch9.h
	Enumeration tables
*/
#ifndef  _USB_CH9_H_
#define  _USB_CH9_H_
// USB functions
void std_request(void);
void class_request(void);
void vendor_request(void);
void send_descriptor(void);
void send_keystroke(void); // ???
void feature(unsigned char);
void get_status(void);
void set_interface(void);
void get_interface(void);
void set_configuration(void);
void get_configuration(void);
// Application code
void do_SETUP(void);      // Handle a USB SETUP transfer
void do_IN3(void);        // Send keyboard characters over Endpoint 3-IN
void check_for_resume(void);
void service_irqs(void);
//void initialize_MAX(void);

#endif /* _USB_CH9_H_ */
