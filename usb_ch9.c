#include "usb_ch9.h"
#include "max3421_registers.h"
#include "max3421e_driver.h"

unsigned char DD[] = // DEVICE Descriptor
		{ 0x12, // bLength = 18d
				0x01, // bDescriptorType = Device (1)
				0x10, 0x01, // bcdUSB(L/H) USB spec rev (BCD)
				0x00, 0x00, 0x00, // bDeviceClass, bDeviceSubClass, bDeviceProtocol
				0x40, // bMaxPacketSize0 EP0 is 64 unsigned chars
				0x6A, 0x0B, // idVendor(L/H)--Maxim is 0B6A
				0x46, 0x53, // idProduct(L/H)--5346
				0x34, 0x12, // bcdDevice--1234
				1, 2, 3, // iManufacturer, iProduct, iSerialNumber
				1 }; // bNumConfigurations

unsigned char CD[] = {
		// CONFIGURATION Descriptor [9]
		0x09, // bLength
		0x02, // bDescriptorType = Config
		0x22, 0x00, // wTotalLength(L/H) = 34 unsigned chars
		0x01, // bNumInterfaces
		0x01, // bConfigValue
		0x00, // iConfiguration
		0xE0, // bmAttributes. b7=1 b6=self-powered b5=RWU supported
		0x32, // MaxPower is 100 ma

		// INTERFACE Descriptor [9]
		0x09, // length = 9
		0x04, // type = IF
		0x00, // IF #0
		0x00, // bAlternate Setting
		0x01, // bNum Endpoints
		0x03, // bInterfaceClass = HID
		0x01, 0x02, // bInterfaceSubClass, bInterfaceProtocol
		0x00, // iInterface

		// HID Descriptor [9] --It's at CD[18]
		0x09, // bLength
		0x21, // bDescriptorType = HID
		0x10, 0x01, // bcdHID(L/H) Rev 1.1
		0x00, // bCountryCode (none)
		0x01, // bNumDescriptors (one report descriptor)
		0x22, // bDescriptorType	(report)
		43, 0, // CD[25]: wDescriptorLength(L/H) (report descriptor size is 43 unsigned chars)

		// Endpoint Descriptor [7]
		0x07, // bLength
		0x05, // bDescriptorType (Endpoint)
		0x83, // bEndpointAddress (EP3-IN)
		0x03, // bmAttributes	(interrupt)
		64, 0, // wMaxPacketSize (64)
		10 }; // bInterval (poll every 10 msec)


unsigned char RepD[] = // Report descriptor
		{ 0x05, 0x01, // Usage Page (generic desktop)
				0x09, 0x06, // Usage (keyboard)
				0xA1, 0x01, // Collection
				0x05, 0x07, //   Usage Page 7 (keyboard/keypad)
				0x19, 0xE0, //   Usage Minimum = 224
				0x29, 0xE7, //   Usage Maximum = 231
				0x15, 0x00, //   Logical Minimum = 0
				0x25, 0x01, //   Logical Maximum = 1
				0x75, 0x08, //   Report Size = 8
				0x95, 0x01, //   Report Count = 1
				0x81, 0x02, //  Input(Data,Variable,Absolute)
				0x95, 0x01, //   Report Count = 1
				0x75, 0x08, //   Report Size = 8
				0x81, 0x01, //  Input(Constant)
				0x19, 0x00, //   Usage Minimum = 0
				0x29, 0x65, //   Usage Maximum = 101
				0x15, 0x00, //   Logical Minimum = 0,
				0x25, 0x65, //   Logical Maximum = 101
				0x75, 0x08, //   Report Size = 8
				0x95, 0x01, //   Report Count = 1
				0x81, 0x00, //  Input(Data,Variable,Array)
				0xC0 }; // End Collection

// STRING descriptors. An array of string arrays

unsigned char strDesc[][64] = {
// STRING descriptor 0--Language string
		{ 0x04, // bLength
				0x03, // bDescriptorType = string
				0x09, 0x04 // wLANGID(L/H) = English-United Sates
		},
		// STRING descriptor 1--Manufacturer ID
		{ 12, // bLength
				0x03, // bDescriptorType = string
				'M', 0, 'a', 0, 'x', 0, 'i', 0, 'm', 0 // text in Unicode
		},
		// STRING descriptor 2 - Product ID
		{
				24, // bLength
				0x03, // bDescriptorType = string
				'M', 0, 'A', 0, 'X', 0, '3', 0, '4', 0, '2', 0, '0', 0, 'E', 0,
				' ', 0, 'E', 0, 'n', 0, 'u', 0, 'm', 0, ' ', 0, 'C', 0, 'o', 0,
				'd', 0, 'e', 0 },

		// STRING descriptor 3 - Serial Number ID
		{
				20, // bLength
				0x03, // bDescriptorType = string
				'S', 0, '/', 0, 'N', 0, ' ', 0, '3', 0, '4', 0, '2', 0, '0', 0,
				'E', 0, } };

unsigned char Message[] = { // each letter is 3 unsigned chars--shiftcode, 00, HID keycode
		0x00, 0x00, 0x28, // (cr)
				0x02, 0x00, 0x17, // T (02 is shift)
				0x00, 0x00, 0x0B, // h
				0x00, 0x00, 0x08, // e
				0x00, 0x00, 0x2C, // (sp)
				0x02, 0x00, 0x10, // M
				0x02, 0x00, 0x04, // A
				0x02, 0x00, 0x1B, // X
				0x00, 0x00, 0x20, // 3
				0x00, 0x00, 0x21, // 4
				0x00, 0x00, 0x1F, // 2
				0x00, 0x00, 0x27, // 0
				0x02, 0x00, 0x08, // E
				0x00, 0x00, 0x2C, // (sp)
				0x00, 0x00, 0x07, // d
				0x00, 0x00, 0x12, // o
				0x00, 0x00, 0x08, // e
				0x00, 0x00, 0x16, // s
				0x00, 0x00, 0x2C, // (sp)
				0x02, 0x00, 0x18, // U
				0x02, 0x00, 0x16, // S
				0x02, 0x00, 0x05, // B
				0x02, 0x00, 0x1E, // !
				0x00, 0x00, 0x28 }; // (cr)

//Global variables
unsigned char SUD[8]; // Local copy of the 8 setup data read from the MAX3420E SUDFIFO
unsigned char ep3stall = 0;
unsigned char configval = 0;
unsigned char RWU_enabled = 0; // Set by Set/Clear_Feature RWU request, sent back for Get_Status-RWU
unsigned char Suspended = 0; // Tells the main loop to look for host resume and RWU pushbutton
static unsigned char epirq, usbirq;
unsigned char sudavirq, suspirq, uresdnirq, uresirq, unknownirq;
unsigned char REGISTER[27];
unsigned char send3zeros;
unsigned char inhibit_send = 0;
unsigned char msgidx = 0;
unsigned char msglen = sizeof(Message);

void delay_usss(unsigned long us) {
	us = us * 16;
	while (us != 0) {
		asm("nop");
		us--;
	}
}

void service_irqs(void) {
	epirq = max3421_rreg(rEPIRQ); // Check the EPIRQ bits
	usbirq = max3421_rreg(rUSBIRQ); // Check the USBIRQ bits

	//	if ((max3421_rreg(rPINCTL) & 0x20) == 0x20) {
	//		ep0bc_count = max3421_rreg(rEP0BC);
	//		max3421_rblock(rEP0FIFO,64,ep0bc);
	//		ep0bc_count = ep0bc_count + 0;
	//	}
	//	if((epirq & bmIN0BAVIRQ) != 0) {
	//		ep0bc_count = max3421_rreg(rEP0BC);
	//		max3421_rblock(rEP0FIFO,64,ep0bc);
	//		ep0bc_count = ep0bc_count + 0;
	//	}

	if ((epirq & bmSUDAVIRQ) != 0) {
		sudavirq++;
		max3421_wreg(rEPIRQ, bmSUDAVIRQ); // clear the SUDAV IRQ
		do_SETUP();
	} else if ((configval != 0) && ((usbirq & bmSUSPIRQ) != 0)) { // HOST suspended bus for 3 msec
		suspirq++;
		max3421_wreg(rUSBIRQ, (bmSUSPIRQ + bmBUSACTIRQ)); // clear the IRQ and bus activity IRQ
		Suspended = 1; // signal the main loop
	} else if ((usbirq & bmURESDNIRQ) != 0) {
		uresdnirq++;
		max3421_wreg(rUSBIRQ, bmURESDNIRQ); // clear the IRQ bit
		Suspended = 0; // in case we were suspended
		ENABLE_IRQS; // ...because a bus reset clears the IE bits
	} else if ((usbirq & bmURESIRQ) != 0) {
		uresirq++;
		max3421_wreg(rUSBIRQ, bmURESIRQ); // clear the IRQ
	} else if (epirq & bmIN3BAVIRQ) { // Was an EP3-IN packet just dispatched to the host?

		do_IN3(); // Yes--load another keystroke and arm the endpoint
		// NOTE: don't clear the IN3BAVIRQ bit here--loading the EP3-IN byte
		// count register in the do_IN3() function does it.
	} else {
		unknownirq++;
		//dump_registers();
	}

}

void do_SETUP(void) {
	//dump_SUD();
	max3421_rblock(rSUDFIFO, 8, SUD); // got a SETUP packet. Read 8 SETUP unsigned chars
	switch (SUD[bmRequestType] & 0x60) { // Parse the SETUP packet. For request type, look only at b6&b5
	case 0x00:
		std_request();
		break;
	case 0x20:
		class_request();
		break; // just a stub in this program
	case 0x40:
		vendor_request();
		break; // just a stub in this program
	default:
		STALL_EP0; // unrecognized request type
	}
}

void send_descriptor(void) {
	unsigned short reqlen, sendlen, desclen, send_ZLP = 0;
	unsigned char *pDdata; // pointer to ROM Descriptor data to send
	//
	// NOTE This function assumes all descriptors are 64 or fewer bytes and can be sent in a single packet
	//
	desclen = 0; // check for zero as error condition (no case statements satisfied)
	reqlen = SUD[wLengthL] + 256 * SUD[wLengthH];
	switch (SUD[wValueH]) { // wValueH is descriptor type
	case GD_DEVICE:
		desclen = DD[0]; // descriptor length
		pDdata = DD;
		break;
	case GD_CONFIGURATION:
		desclen = CD[2]; // Config descriptor includes interface, HID, report and ep descriptors
		pDdata = CD;
		break;
	case GD_STRING:
		desclen = strDesc[SUD[wValueL]][0]; // wValueL=string index, array[0] is the length
		pDdata = strDesc[SUD[wValueL]]; // point to first array element
		break;
	case GD_HID:
		desclen = CD[18];
		pDdata = &CD[18];
		break;
	case GD_REPORT:
		desclen = CD[25];
		pDdata = RepD;
		break;
	default:
		asm("nop");
		break;
	} // end switch on descriptor type

	if (desclen != 0) // one of the case statements above filled in a value
	{
		sendlen = (reqlen <= desclen) ? reqlen : desclen; // send the smaller of requested and avaiable
		while (!(max3421_rreg(rEPIRQ) & bmIN0BAVIRQ)) {
			// Meanwhile output buffer isn't free
		};
		//max3421_wblock(rEP0FIFO, sendlen, pDdata);
		for (send_ZLP = 0; send_ZLP < sendlen; send_ZLP++) {
			max3421_wreg(rEP0FIFO, pDdata[send_ZLP]);
		}
		max3421_wreg_as(rEP0BC, sendlen); // load EP0BC to arm the EP0-IN transfer & ACKSTAT
	}
		else STALL_EP0; // none of the descriptor types match
}

void std_request(void) {
	switch (SUD[bRequest]) {
	case SR_GET_DESCRIPTOR:
		send_descriptor();
		break;
	case SR_SET_FEATURE:
		feature(1);
		break;
	case SR_CLEAR_FEATURE:
		feature(0);
		break;
	case SR_GET_STATUS:
		get_status();
		break;
	case SR_SET_INTERFACE:
		set_interface();
		break;
	case SR_GET_INTERFACE:
		get_interface();
		break;
	case SR_GET_CONFIGURATION:
		get_configuration();
		break;
	case SR_SET_CONFIGURATION:
		set_configuration();
		break;
	case SR_SET_ADDRESS:
		TRANSMIT_ACK;
		break; // discard return value
	default:
		STALL_EP0;
		break;
	}
}

void class_request(void) {
	STALL_EP0;
}

void vendor_request(void) {
	STALL_EP0;
}

void check_for_resume(void) {
	if (max3421_rreg(rUSBIRQ) & bmBUSACTIRQ) { // THE HOST RESUMED BUS TRAFFIC
		Suspended = 0; // no longer suspended
	} else if (RWU_enabled) { // Only if the host enabled RWU
		if ((max3421_rreg(rGPIO) & 0x40) == 0) {// See if the Remote Wakeup button was pressed
			Suspended = 0; // no longer suspended
			SETBIT(rUSBCTL,bmSIGRWU) // signal RWU
			while ((max3421_rreg(rUSBIRQ) & bmRWUDNIRQ) == 0)
				; // spin until RWU signaling done
			CLRBIT(rUSBCTL,bmSIGRWU) // remove the RESUME signal
			max3421_wreg(rUSBIRQ, bmRWUDNIRQ); // clear the IRQ
			max3421_wreg(rUSBIRQ, bmBUSACTIRQ); // wait for bus traffic -- clear the BUS Active IRQ
			while ((max3421_rreg(rUSBIRQ) & bmBUSACTIRQ) == 0)
				; // & hang here until it's set again...
		}
	}
}

void do_IN3(void) {
	if (inhibit_send == 0x01) {
		max3421_wreg(rEP3INFIFO, 0); // send the "keys up" code
		max3421_wreg(rEP3INFIFO, 0);
		max3421_wreg(rEP3INFIFO, 0);
	} else if (send3zeros == 0x01) { // precede every keycode with the "no keys" code
		max3421_wreg(rEP3INFIFO, 0); // send the "keys up" code
		max3421_wreg(rEP3INFIFO, 0);
		max3421_wreg(rEP3INFIFO, 0);
		send3zeros = 0; // next time through this function send the keycode
	} else {
        send3zeros=1;
        max3421_wreg(rEP3INFIFO,Message[msgidx++]);	// load the next keystroke (3 bytes)
        max3421_wreg(rEP3INFIFO,Message[msgidx++]);
        max3421_wreg(rEP3INFIFO,Message[msgidx++]);
	if(msgidx >= msglen)                    // check for message wrap
            {
            msgidx=0;
            inhibit_send=1;                     // send the string once per pushbutton press
            }
	}
	max3421_wreg(rEP3INBC, 3); // arm it
}

void feature(unsigned char sc) {
	unsigned char mask;
	if ((SUD[bmRequestType] == 0x02) // dir=h->p, recipient = ENDPOINT
			&& (SUD[wValueL] == 0x00) // wValueL is feature selector, 00 is EP Halt
			&& (SUD[wIndexL] == 0x83)) { // wIndexL is endpoint number IN3=83
		mask = max3421_rreg(rEPSTALLS); // read existing bits
		if (sc == 1) { // set_feature
			mask += bmSTLEP3IN; // Halt EP3IN
			ep3stall = 1;
		} else { // clear_feature
			mask &= ~bmSTLEP3IN; // UnHalt EP3IN
			ep3stall = 0;
			max3421_wreg(rCLRTOGS, bmCTGEP3IN); // clear the EP3 data toggle
		}
		max3421_wreg(rEPSTALLS, (mask | bmACKSTAT)); // Don't use wregAS for this--directly writing the ACKSTAT bit
	} else if ((SUD[bmRequestType] == 0x00) // dir=h->p, recipient = DEVICE
			&& (SUD[wValueL] == 0x01)) { // wValueL is feature selector, 01 is Device_Remote_Wakeup
		RWU_enabled = sc << 1; // =2 for set, =0 for clear feature. The shift puts it in the get_status bit position.
		max3421_rreg_as(rFNADDR); // dummy read to set ACKSTAT
	}
		else STALL_EP0;
}

void get_status(void) {
	unsigned char testbyte;
	testbyte = SUD[bmRequestType];
	switch (testbyte) {
	case 0x80: // directed to DEVICE
		max3421_wreg(rEP0FIFO, (RWU_enabled + 1)); // first byte is 000000rs where r=enabled for RWU and s=self-powered.
		max3421_wreg(rEP0FIFO, 0x00); // second byte is always 0
		max3421_wreg_as(rEP0BC, 2); // load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
		break;
	case 0x81: // directed to INTERFACE
		max3421_wreg(rEP0FIFO, 0x00); // this one is easy--two zero bytes
		max3421_wreg(rEP0FIFO, 0x00);
		max3421_wreg_as(rEP0BC, 2); // load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
		break;
	case 0x82: // directed to ENDPOINT
		if (SUD[wIndexL] == 0x83) { // We only reported ep3, so it's the only one the host can stall IN3=83
			max3421_wreg(rEP0FIFO, ep3stall); // first byte is 0000000h where h is the halt (stall) bit
			max3421_wreg(rEP0FIFO, 0x00); // second byte is always 0
			max3421_wreg_as(rEP0BC, 2); // load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
			break;
		}
			else STALL_EP0; // Host tried to stall an invalid endpoint (not 3)

	default:
		STALL_EP0; // don't recognize the request
	}
}

void set_configuration(void) {
	configval = SUD[wValueL]; // Store the config value
	if (configval != 0) // If we are configured,
		SETBIT(rUSBIEN,bmSUSPIE); // start looking for SUSPEND interrupts
	max3421_rreg_as(rFNADDR); // dummy read to set the ACKSTAT bit
}

void get_configuration(void) {
	max3421_wreg(rEP0FIFO, configval); // Send the config value
	max3421_wreg_as(rEP0BC, 1);
}

void set_interface(void) { // All we accept are Interface=0 and AlternateSetting=0, otherwise send STALL
	unsigned char dumval = 0;
	dumval++;
	if ((SUD[wValueL] == 0) // wValueL=Alternate Setting index
			&& (SUD[wIndexL] == 0)) // wIndexL=Interface index
		dumval = max3421_rreg_as(rFNADDR); // dummy read to set the ACKSTAT bit
		else STALL_EP0;
}

void get_interface(void) { // Check for Interface=0, always report AlternateSetting=0
	if (SUD[wIndexL] == 0) { // wIndexL=Interface index
		max3421_wreg(rEP0FIFO, 0); // AS=0
		max3421_wreg_as(rEP0BC, 1); // send one byte, ACKSTAT
	}
		else STALL_EP0;
}

void send_keystroke(void) {
	if ((max3421_rreg(rGPIO) & 0x10) == 0) // Check the pushbutton on GPI-0
	{
		inhibit_send = 0x00; // Tell the "do_IN3" function to send the text string
		// Turn on the SEND light
	}
}
