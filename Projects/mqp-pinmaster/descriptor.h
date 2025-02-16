#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

/* Includes: */
#include <avr/pgmspace.h>

#include <LUFA/Drivers/USB/USB.h>

enum registry_props_type {
	REG_SZ = 1,
	REG_EXPAND_SZ,
	REG_BINARY,
	REG_DWORD_LITTLE_ENDIAN,
	REG_DWORD_BIG_ENDIAN,
	REG_LINK,
	REG_MULTI_SZ,
};

/* Macros: */
/** Endpoint address of the PINMASTER data OUT endpoint. */
#define PINMASTER_DATA_OUT_EPADDR         (ENDPOINT_DIR_OUT | 1)

/** Endpoint address of the PINMASTER data IN endpoint. */
#define PINMASTER_DATA_IN_EPADDR          (ENDPOINT_DIR_IN  | 2)

/** Size in bytes of the PINMASTER data endpoint. */
#define PINMASTER_DATA_EPSIZE             64

/** Device control request used for retrieving the OS Feature descriptors of the device. */
#define REQ_GetOSFeatureDescriptor       0x20

/** Descriptor index for a Microsoft Proprietary Extended Device Compatibility descriptor. */
#define EXTENDED_COMPAT_ID_DESCRIPTOR    0x0004

/** Descriptor index for a Microsoft Proprietary Extended Device Properties OS Feature Descriptor. */
#define EXTENDED_PROPERTIES_DESCRIPTOR   0x0005

/** wchar_t is not available on all platforms */
#if ((ARCH == ARCH_AVR8) || (ARCH == ARCH_XMEGA))
#define WCHAR                  wchar_t
#else
#define WCHAR                  uint16_t
#endif

/** Interface & (dummy) Endpoint properties */
#define WCID_IF_NUMBER         0
#define WCID_IN_EPNUM          1
#define WCID_OUT_EPNUM         2
#define WCID_IO_EPSIZE         64

/** Registry values we want to set using the Properties Descriptor */
#define PROPERTY1_NAME         L"DeviceInterfaceGUID"
#define PROPERTY1_TYPE         REG_SZ
#define PROPERTY1_DATA         L"{523ec697-a458-42e1-b515-fc6512580012}"

/** Helper Macro for the above. Header size is 14 bytes */
#define PROPERTY_SIZE (14 + sizeof(PROPERTY1_NAME) + sizeof(PROPERTY1_DATA))

#define __packed __attribute__((packed))
/* Type Defines: */
/** Type define for the device configuration descriptor structure. This must be defined in the
 *  application code, as the configuration descriptor contains several sub-descriptors which
 *  vary between devices, and which describe the device's usage to the host.
 */
typedef struct {
	USB_Descriptor_Configuration_Header_t    Config;

	USB_Descriptor_Interface_t               PINMASTER_Interface;
	USB_Descriptor_Endpoint_t                PINMASTER_DataOutEndpoint;
	USB_Descriptor_Endpoint_t                PINMASTER_DataInEndpoint;
} __packed PINMASTER_USB_Descriptor_Configuration_t;

typedef struct {
	USB_Descriptor_Header_t	Header;
	int			Signature[7];
	uint8_t			VendorCode;
	uint8_t			Reserved;
} __packed USB_OSDescriptor_t;

typedef struct {
	uint8_t	FirstInterfaceNumber;
	uint8_t	Reserved;
	uint8_t	CompatibleID[8];
	uint8_t	SubCompatibleID[8];
	uint8_t	Reserved2[6];
} __packed  USB_OSCompatibleSection_t;

typedef struct {
	uint32_t			TotalLength;
	uint16_t			Version;
	uint16_t			Index;
	uint8_t				TotalSections;
	uint8_t				Reserved[7];
	USB_OSCompatibleSection_t	CompatID;
} __packed USB_OSCompatibleIDDescriptor_t;

typedef struct {
	uint16_t	Length;
	WCHAR		Value[sizeof(PROPERTY1_NAME)/2];
} __packed USB_OSPropertiesName_t;

typedef struct {
	uint32_t	Length;
	WCHAR		Value[sizeof(PROPERTY1_DATA)/2];
} __packed USB_OSPropertiesData_t;

typedef struct {
	uint32_t		Length;
	uint32_t		Type;
	USB_OSPropertiesName_t	Name;
	USB_OSPropertiesData_t	Data;
} __packed USB_OSPropertiesSection_t;

typedef struct {
	uint32_t TotalLength;
	uint16_t Version;
	uint16_t Index;
	uint16_t TotalSections;
} __packed USB_OSPropertiesHeader_t;

typedef struct {
	USB_OSPropertiesHeader_t	Header;
	USB_OSPropertiesSection_t	Property;
} __packed USB_OSPropertiesDescriptor_t;


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
enum PINMASTER_StringDescriptors_t {
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

uint16_t USB_GetOSFeatureDescriptor(const uint8_t InterfaceNumber,
				    const uint8_t wIndex,
				    const uint8_t Recipient,
				    const void** const DescriptorAddress)
	ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(4);

#endif

