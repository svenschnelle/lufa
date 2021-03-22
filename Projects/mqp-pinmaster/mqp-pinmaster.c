#include "mqp-pinmaster.h"

#define PP_OUTD0 PF4
#define PP_OUTD1 PF5
#define PP_OUTD2 PF6
#define PP_OUTD3 PF7
#define PP_OUTD4 PB1
#define PP_OUTD5 PB3
#define PP_OUTD6 PB2
#define PP_OUTD7 PB6

#define PP_INIT PD0
#define PP_AUTO_LF PC6
#define PP_STROBE PD4
#define PP_SELECT_PRINTER PD1

#define PP_ACK PB5
#define PP_BUSY PB4
#define PP_PAPER_OUT PE6
#define PP_SELECT PD7

static void pp_setup(void)
{
	PORTD |= _BV(PP_STROBE) | _BV(PP_INIT);
	DDRF = _BV(PF7) | _BV(PF6) | _BV(PF5) | _BV(PF4);
	DDRE = 0;
	DDRD = _BV(PD4) | _BV(PD3) | _BV(PD2) | _BV(PD1) | _BV(PD0);
	DDRC = _BV(PC6);
	DDRB = _BV(PB6) | _BV(PB3) | _BV(PB2) | _BV(PB1);
	PORTD |= _BV(PP_SELECT_PRINTER);
}

static void pp_data_out(void)
{
	int count = Endpoint_BytesInEndpoint();
	static bool ctrl = false;
	for(int i = 0; i < count; i++) {
		uint8_t c = Endpoint_Read_8();

		if (!ctrl && c == 0x9b) {
			ctrl = true;
			continue;
		}

		if (c == 0x9b)
			ctrl = false;

		c ^= 0xff;

		while((PINB & _BV(PP_BUSY)));
		PORTF = (PORTF & 0x0f) | (c << 4);
		if (c & 0x10) PORTB |= _BV(PP_OUTD4); else PORTB &= ~_BV(PP_OUTD4);
		if (c & 0x20) PORTB |= _BV(PP_OUTD5); else PORTB &= ~_BV(PP_OUTD5);
		if (c & 0x40) PORTB |= _BV(PP_OUTD6); else PORTB &= ~_BV(PP_OUTD6);
		if (c & 0x80) PORTB |= _BV(PP_OUTD7); else PORTB &= ~_BV(PP_OUTD7);
		if (ctrl) PORTD &= ~_BV(PP_INIT); else PORTD |= _BV(PP_INIT);
		PORTC |= _BV(PP_AUTO_LF);
		_delay_us(1);
		PORTD &= ~_BV(PP_STROBE);
		_delay_us(1);
		PORTD |= _BV(PP_STROBE);
		ctrl = false;
	}
}

static void pp_data_in(void)
{
	for(int i = 0; i < 64; i++) {
		int cnt = 512;
		while ((PINB & _BV(PP_ACK)) && cnt--);

		if (cnt < 0)
			break;

		PORTD &= ~_BV(PP_SELECT_PRINTER);
		_delay_us(1);
		uint8_t tmp = 0;
		if (PINB & _BV(PP_BUSY)) tmp |= 0x80;
		if (PINB & _BV(PP_ACK)) tmp |= 0x40;
		if (PINE & _BV(PP_PAPER_OUT)) tmp |= 0x20;
		if (PIND & _BV(PP_SELECT)) tmp |= 0x10;
		PORTC &= ~_BV(PP_AUTO_LF);
		_delay_us(1);
		if (PINB & _BV(PP_BUSY)) tmp |= 0x8;
		if (PINB & _BV(PP_ACK)) tmp |= 0x4;
		if (PINE & _BV(PP_PAPER_OUT)) tmp |= 0x2;
		if (PIND & _BV(PP_SELECT)) tmp |= 0x1;
		PORTD |= _BV(PP_SELECT_PRINTER);
		PORTC |= _BV(PP_AUTO_LF);
		Endpoint_Write_8(tmp);
	}
}

int main(void)
{
	SetupHardware();
	pp_setup();
	GlobalInterruptEnable();

	for (;;)
	{
		Pinmaster_Task();
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#endif

	#if defined(RESET_TOGGLES_LIBUSB_COMPAT)
	UpdateCurrentCompatibilityMode();
	#endif
	/* USB Stack Initialization */
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup PINMASTER Data OUT endpoint */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(PINMASTER_DATA_OUT_EPADDR, EP_TYPE_BULK, PINMASTER_DATA_EPSIZE, 1);

	/* Setup PINMASTER Data IN endpoint if it is using a physically different endpoint */
	if ((PINMASTER_DATA_IN_EPADDR & ENDPOINT_EPNUM_MASK) != (PINMASTER_DATA_OUT_EPADDR & ENDPOINT_EPNUM_MASK))
	ConfigSuccess &= Endpoint_ConfigureEndpoint(PINMASTER_DATA_IN_EPADDR, EP_TYPE_BULK, PINMASTER_DATA_EPSIZE, 1);

	/* Indicate endpoint configuration success or failure */
}

void Pinmaster_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	Endpoint_SelectEndpoint(PINMASTER_DATA_IN_EPADDR);
	if (Endpoint_IsINReady()) {
		pp_data_in();
		Endpoint_ClearIN();
	}

	Endpoint_SelectEndpoint(PINMASTER_DATA_OUT_EPADDR);

	if (Endpoint_IsOUTReceived()) {
		pp_data_out();
		Endpoint_ClearOUT();
	}
}

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
	return PINMASTER_GetDescriptor(wValue, wIndex, DescriptorAddress);
}

