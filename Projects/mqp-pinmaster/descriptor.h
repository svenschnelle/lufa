#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <avr/pgmspace.h>

		#include <LUFA/Drivers/USB/USB.h>

	/* Macros: */
		/** Endpoint address of the PINMASTER data OUT endpoint. */
		#define PINMASTER_DATA_OUT_EPADDR         (ENDPOINT_DIR_OUT | 1)

		/** Endpoint address of the PINMASTER data IN endpoint. */
		#define PINMASTER_DATA_IN_EPADDR          (ENDPOINT_DIR_IN  | 2)

		/** Size in bytes of the PINMASTER data endpoint. */
		#define PINMASTER_DATA_EPSIZE             64

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t    Config;

			USB_Descriptor_Interface_t               PINMASTER_Interface;
			USB_Descriptor_Endpoint_t                PINMASTER_DataOutEndpoint;
			USB_Descriptor_Endpoint_t                PINMASTER_DataInEndpoint;
		} PINMASTER_USB_Descriptor_Configuration_t;

		/** Enum for the device interface descriptor IDs within the device. Each interface descriptor
		 *  should have a unique ID index associated with it, which can be used to refer to the
		 *  interface from other descriptors.
		 */
		enum InterfaceDescriptors_t
		{
			INTERFACE_ID_PINMASTER = 0, /**< PINMASTER interface descriptor ID */
		};

		/** Enum for the device string descriptor IDs within the device. Each string descriptor should
		 *  have a unique ID index associated with it, which can be used to refer to the string from
		 *  other descriptors.
		 */
		enum PINMASTER_StringDescriptors_t
		{
			PINMASTER_STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
			PINMASTER_STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
			PINMASTER_STRING_ID_Product      = 2, /**< Product string ID */
			PINMASTER_STRING_ID_Serial       = 3, /**< Serial number string ID */
		};

	/* Function Prototypes: */
		uint16_t PINMASTER_GetDescriptor(const uint16_t wValue,
		                              const uint16_t wIndex,
		                              const void** const DescriptorAddress)
		                              ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

