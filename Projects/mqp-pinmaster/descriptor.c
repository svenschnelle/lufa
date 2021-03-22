#include "descriptor.h"
const USB_Descriptor_Device_t PROGMEM PINMASTER_DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(1,1,0),
	.Class                  = USB_CSCP_VendorSpecificClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x1748,
	.ProductID              = 0x0001,
	.ReleaseNumber          = VERSION_BCD(0,0,1),

	.ManufacturerStrIndex   = PINMASTER_STRING_ID_Manufacturer,
	.ProductStrIndex        = PINMASTER_STRING_ID_Product,
	.SerialNumStrIndex      = PINMASTER_STRING_ID_Serial,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const PINMASTER_USB_Descriptor_Configuration_t PROGMEM PINMASTER_ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(PINMASTER_USB_Descriptor_Configuration_t),
			.TotalInterfaces        = 1,

			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.PINMASTER_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = INTERFACE_ID_PINMASTER,
			.AlternateSetting       = 0,

			.TotalEndpoints         = 2,

			.Class                  = USB_CSCP_VendorSpecificClass,
			.SubClass               = USB_CSCP_NoDeviceSubclass,
			.Protocol               = USB_CSCP_NoDeviceProtocol,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.PINMASTER_DataInEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = PINMASTER_DATA_IN_EPADDR,
			.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = PINMASTER_DATA_EPSIZE,
			.PollingIntervalMS      = 0x0A
		},

	.PINMASTER_DataOutEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = PINMASTER_DATA_OUT_EPADDR,
			.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = PINMASTER_DATA_EPSIZE,
			.PollingIntervalMS      = 0x0A
		},
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM PINMASTER_LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM PINMASTER_ManufacturerString = USB_STRING_DESCRIPTOR(L"MQP Electronics");

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM PINMASTER_ProductString = USB_STRING_DESCRIPTOR(L"Pinmaster USB Adapter");

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
const USB_Descriptor_String_t PROGMEM PINMASTER_SerialString = USB_STRING_DESCRIPTOR(L"000200012345\0"
    // Note: Real PINMASTER-MKII has the embedded NUL byte, bug in firmware?
);

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t PINMASTER_GetDescriptor(const uint16_t wValue,
                              const uint16_t wIndex,
                              const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &PINMASTER_DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &PINMASTER_ConfigurationDescriptor;
			Size    = sizeof(PINMASTER_USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case PINMASTER_STRING_ID_Language:
					Address = &PINMASTER_LanguageString;
					Size    = pgm_read_byte(&PINMASTER_LanguageString.Header.Size);
					break;
				case PINMASTER_STRING_ID_Manufacturer:
					Address = &PINMASTER_ManufacturerString;
					Size    = pgm_read_byte(&PINMASTER_ManufacturerString.Header.Size);
					break;
				case PINMASTER_STRING_ID_Product:
					Address = &PINMASTER_ProductString;
					Size    = pgm_read_byte(&PINMASTER_ProductString.Header.Size);
					break;
				case PINMASTER_STRING_ID_Serial:
					Address = &PINMASTER_SerialString;
					Size    = PINMASTER_SerialString.Header.Size;
					break;
			}

			break;
	}

	*DescriptorAddress = Address;
	return Size;
}
