#ifndef _MQP_PINMASTER_H_
#define _MQP_PINMASTER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/interrupt.h>
		#include <avr/power.h>

		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Platform/Platform.h>

		#include "descriptor.h"

	/* Function Prototypes: */
		void SetupHardware(void);
		void Pinmaster_Task(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);

		uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		                                    const uint16_t wIndex,
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

