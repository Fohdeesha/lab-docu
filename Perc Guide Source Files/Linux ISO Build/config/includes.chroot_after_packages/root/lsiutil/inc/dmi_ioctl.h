#ifndef _DMI_IOCTL_H_
#define _DMI_IOCTL_H_

/***************************************************************************
 *                                                                         *
 *  Copyright 1998 LSI Logic Corporation.  All rights reserved.            *
 *                                                                         *
 *  This file is confidential and a trade secret of LSI Logic.  The        *
 *  receipt of or possession of this file does not convey any rights to    *
 *  reproduce or disclose its contents or to manufacture, use, or sell     *
 *  anything it may describe, in whole, or in part, without the specific   *
 *  written consent of LSI Logic Corporation.                              *
 *                                                                         *
 ***************************************************************************/

/*           Name:  DMI_IOCTL.H
 *          Title:  Solaris O/S Character mode IOCTL definitions
 *     Programmer:  Russell A. Johnson
 *  Creation Date:  May 26, 1998
 *
 *  Version History
 *  ---------------
 *
 *    Date    Who?  Description
 *  --------  ----  -------------------------------------------------------
#BeginRevision
 *  05/17/00  RAJ   Added bus mode support for fibre channel devices.
 *  12/19/99  RAJ   Added new flags for scsi-3 options.
 *  11/09/98  RAJ   Changed copyright notice to LSI Logic.
#EndRevision
 *            Version 4.02.01
 *  07/22/98  RAJ   Added some more data to the DMI data structure.  This
 *                      includes the PCI bus number, device number, and function
 *                      number.  Also added a major version number for the data
 *                      structure.  That version number indicates major changes.
 *                      When it changes then the entire data format may have
 *                      changed.  See the comments below for more details.
#EndRevision
 *            Version 4.00.04
 *  05/26/98  RAJ   Initial version.
#EndRevision
 *
#BeginDescription
 *
 *  This file contains the Solaris 2.6 character mode driver interface
 *  definitions needed in order to provide IOCTL's for our SCSI HBA driver.
 *
#EndDescription
 *
 *-------------------------------------------------------------------------
 */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * HEADER FILE DEPENDANCIES
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 *  This file is dependent on the following files:
 *
 *      SALIOS.H - type defintions UINT8, UINT16, UINT32, UINT64
 */

/*  The following data is for our character mode ioctl interface.  Per the
 *  Solaris standard mechanism, we define SYMIOCTL as a unique number in the
 *  upper 8 bits and a command value in the lower 8 bits.
 */
#define SYMIOCTL                        ('H' << 8)
#define SYMIOCTL_GET_DMI_DATA           (SYMIOCTL | 1)
#define SYMIOCTL_GET_TIMERS             (SYMIOCTL | 2)
#define SYMIOCTL_UPDATE_FLASH           (SYMIOCTL | 3)
#define SYMIOCTL_RESET_ADAPTER          (SYMIOCTL | 4)
#define SYMIOCTL_GET_PROPERTY           (SYMIOCTL | 5)
#define SYMIOCTL_PASS_THRU              (SYMIOCTL | 6)
#define SYMIOCTL_REG_ACCESS             (SYMIOCTL | 7)
#define SYMIOCTL_EVENT_QUERY            (SYMIOCTL | 8)
#define SYMIOCTL_EVENT_ENABLE           (SYMIOCTL | 9)
#define SYMIOCTL_EVENT_REPORT           (SYMIOCTL | 10)
#define SYMIOCTL_GET_PCI_INFO           (SYMIOCTL | 11)
#define SYMIOCTL_BTDH_MAPPING           (SYMIOCTL | 12)
#define SYMIOCTL_DIAG_ACTION            (SYMIOCTL | 13)
#define SYMIOCTL_PASS_THRU_TIMEOUT      (SYMIOCTL | 14)

/*  The following are our ioctl() return status values.  If everything went
 *  well, we return good status.  If the buffer length sent to us is too short
 *  we return a status to tell the user.
 */
#define SYMIOCTL_STATUS_GOOD            0
#define SYMIOCTL_STATUS_LEN_TOO_SHORT   1

/*  The data structures defined in this file are marked with a data structure
 *  length and data structure version.  The length value is the first value in
 *  the structure and is used to make sure the buffer is large enough for
 *  communication between the driver and the application.  The version number is
 *  a major version number.  If the data structure changes and only has a new
 *  element appended, then the version number will remain the same but the
 *  length will increase.  If the data structure changes in any other way, such
 *  as re-arranging all of its elements, then the version number will increase.
 *
 *  The following macro defines the current version number of the data
 *  structure.
 */
#define SYMHI_DMI_DATA_VERSION      1

/*  The following is the SYMIOCTL_GET_DMI_DATA data structure.  This data
 *  structure is setup so that we hopefully are properly aligned for both 32-bit
 *  and 64-bit mode applications.
 *
 *  StructureLength - This value is the amount of data the caller has allocated
 *  for the structure when they first call us.  After we have filled in the
 *  structure, this indicates the length our data structure is.
 *
 *  MajorVersion - This value is used by the driver to tell the application what
 *  version of the data structure is being provided.  It only changes if the
 *  data ordering of the data below is changed.
 *
 *  MinSyncPeriodNs - This is the minimum period in nano-seconds (ns) that we
 *  will negotiate for on this adapter.  The smaller value the faster
 *  synchronous speed except if this value is zero then asynchronous transfers
 *  is all we support.
 *
 *  MaxWidth - This value indicates the maximum width this bus can be used as.
 *  If the PciDeviceId indicates a width capability of 16 devices and this is
 *  set to 8 devices then the PCI Subsystem ID value has limited our use of this
 *  adapter to 8 devices.  This value also indicates the number of valid
 *  elements in the DevSpeed[] and DevWidth[] array's.
 *
 *  HostScsiId - This is the host adapter SCSI ID being used by this adapter.
 *
 *  PciBusNumber - The number of the PCI bus this adapter is on.  If for some
 *  reason the driver is unable to determine the bus number, device number, or
 *  function number, these values will be set to 0xFF.
 *
 *  PciDeviceNumber - The PCI device number for this device.
 *
 *  PciFunctionNumber - The PCI function number for this device.
 *
 *  PciDeviceId - This is the PCI device ID from PCI configuration space for
 *  this adapter.
 *
 *  PciRevision - This is the PCI revision value from PCI configuration space
 *  for this adapter.
 *
 *  HwBusMode - This value indicates the mode the bus is currently in.  See the
 *  SYM_HW_BUS_MODE_xx macros.
 *
 *  DevSpeed - This array is indexed by the target ID and indicates the
 *  currently negotiated synchronous speed in nano-seconds (ns).  A value of
 *  zero ns indicates asynchronous mode.
 *
 *  DevWidth - This array is indexed by the target ID and indicates the
 *  currently negotiated width in bits.  A value of 8 indicates narrow mode, a
 *  value of 16 indicates wide.
 *
 *  DriverVersion - This is an ascii null-terminated string indicating the
 *  version of this driver.
 *
 *  DevFlags - This array is indexed by the target ID and indicates the
 *  currently negotiated options such as DT and async protection capabilities.
 */
#pragma pack(1)
typedef struct _SYMHI_DMI_DATA
{                                       /* Offset */
    UINT32  StructureLength;            /* 0x00..0x03 */
    UINT32  Reserved1;                  /* 0x04..0x07 */
    UINT32  MajorVersion;               /* 0x08..0x0B */
    UINT16  MinSyncPeriodNs;            /* 0x0C..0x0D */
    UINT8   MaxWidth;                   /* 0x0E */
    UINT8   HostScsiId;                 /* 0x0F */
    UINT8   PciBusNumber;               /* 0x10 */
    UINT8   PciDeviceNumber;            /* 0x11 */
    UINT8   PciFunctionNumber;          /* 0x12 */
    UINT8   Reserved2;                  /* 0x13 */
    UINT16  PciDeviceId;                /* 0x14..0x15 */
    UINT8   PciRevision;                /* 0x16 */
    UINT8   HwBusMode;                  /* 0x17 */
    UINT8   Reserved3[8];               /* 0x18..0x1F */
#if defined TARGET_MPTx
    /*  MPI adapters can have many more devices per SCSI bus than non-MPI
     *  adapters.
     */
    UINT16  DevSpeed[256];              /* 0x20..0x21F */
    UINT8   DevWidth[256];              /* 0x220..0x31F */
    UINT32  DevFlags[256];              /* 0x320..0x71F */
    char    DriverVersion[80];          /* 0x720..0x76F */
#elif defined TARGET_HIHW || defined TARGET_SRHW
    UINT16  DevSpeed[16];               /* 0x20..0x3F */
    UINT8   DevWidth[16];               /* 0x40..0x4F */
    char    DriverVersion[80];          /* 0x50..0x9F */
    UINT32  DevFlags[16];               /* 0xA0..0xDF */
#else
#error "Unknown hardware type"
#endif  /* defined TARGET_xxx */
} SYMHI_DMI_DATA, * PTR_SYMHI_DMI_DATA;
#pragma pack()

/*  The following are definitions for the value of HwBusMode in the
 *  SYMHI_DMI_DATA data structure.
 *
 *  SYM_HW_BUS_MODE_UNKNOWN - The bus mode is not known yet
 *  SYM_HW_BUS_MODE_SINGLE - The bus is in single ended mode
 *  SYM_HW_BUS_MODE_HVD - The bus is in high voltage differential mode
 *  SYM_HW_BUS_MODE_LVD - The bus is in low voltage differential mode
 *  SYM_HW_BUS_MODE_FC - The bus is a fibre channel bus
 */
#define SYM_HW_BUS_MODE_UNKNOWN     (0x00)
#define SYM_HW_BUS_MODE_SINGLE      (0x01)
#define SYM_HW_BUS_MODE_HVD         (0x02)
#define SYM_HW_BUS_MODE_LVD         (0x03)
#define SYM_HW_BUS_MODE_FC          (0x04)

/*  The following are definitions for the DevFlags[] array.  Each bit is set
 *  only when the feature is currently in use for the given device.
 *
 *  SYM_DEV_DT - Set when Dual Transfers (ie. dual edge clock in use) enabled
 *  SYM_DEV_ASYNC_PROT - Set when asynchronous phase protection is enabled
 */
#define SYM_DEV_DT                  (0x00000001)
#define SYM_DEV_ASYNC_PROT          (0x00000002)

#pragma pack(1)
typedef struct _SYM_UPDATE_FLASH
{
    UINT64  PtrBuffer;
    UINT32  ImageChecksum;
    UINT32  ImageOffset;
    UINT32  ImageSize;
    UINT32  ImageType;
} SYM_UPDATE_FLASH, * PTR_SYM_UPDATE_FLASH;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_GET_PROPERTY
{
    UINT64  PtrName;
    UINT64  PtrBuffer;
    UINT32  NameLen;
    UINT32  BufferLen;
    UINT32  PropertyLen;
} SYM_GET_PROPERTY, * PTR_SYM_GET_PROPERTY;
#pragma pack()

#define SYM_PASS_THRU_NONE   0
#define SYM_PASS_THRU_READ   1
#define SYM_PASS_THRU_WRITE  2
#define SYM_PASS_THRU_BOTH   3

#pragma pack(1)
typedef struct _SYM_PASS_THRU
{
    UINT64  PtrRequest;
    UINT64  PtrReply;
    UINT64  PtrData;
    UINT32  RequestSize;
    UINT32  ReplySize;
    UINT32  DataSize;
    UINT32  DataDirection;
    UINT64  PtrDataOut;
    UINT32  DataOutSize;
} SYM_PASS_THRU, * PTR_SYM_PASS_THRU;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_PASS_THRU_TIMEOUT
{
    UINT64  PtrRequest;
    UINT64  PtrReply;
    UINT64  PtrData;
    UINT32  RequestSize;
    UINT32  ReplySize;
    UINT32  DataSize;
    UINT32  DataDirection;
    UINT64  PtrDataOut;
    UINT32  DataOutSize;
    UINT32  Timeout;
} SYM_PASS_THRU_TIMEOUT, * PTR_SYM_PASS_THRU_TIMEOUT;
#pragma pack()

#define REG_IO_READ    1
#define REG_IO_WRITE   2
#define REG_MEM_READ   3
#define REG_MEM_WRITE  4

#pragma pack(1)
typedef struct _SYM_REG_ACCESS
{
    UINT32  Command;
    UINT32  RegOffset;
    UINT32  RegData;
} SYM_REG_ACCESS, * PTR_SYM_REG_ACCESS;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_EVENT_QUERY
{
    UINT32  Entries;
    UINT32  Types;
} SYM_EVENT_QUERY, * PTR_SYM_EVENT_QUERY;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_EVENT_ENABLE
{
    UINT32  Types;
} SYM_EVENT_ENABLE, * PTR_SYM_EVENT_ENABLE;
#pragma pack()

#if defined TARGET_MPT2
#define EVENT_ENTRY_NUM 48
#else
#define EVENT_ENTRY_NUM 2
#endif

#pragma pack(1)
typedef struct _SYM_EVENT_ENTRY
{
    UINT32  Type;
    UINT32  Number;
    UINT32  Data[EVENT_ENTRY_NUM];
} SYM_EVENT_ENTRY, * PTR_SYM_EVENT_ENTRY;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_EVENT_REPORT
{
    UINT32           Size;
    SYM_EVENT_ENTRY  Events[1];
} SYM_EVENT_REPORT, * PTR_SYM_EVENT_REPORT;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_PCI_INFO
{
    UINT32  BusNumber;
    UINT8   DeviceNumber;
    UINT8   FunctionNumber;
    UINT16  InterruptVector;
    UINT8   PciHeader[256];
} SYM_PCI_INFO, * PTR_SYM_PCI_INFO;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_BTDH_MAPPING
{
    UINT16  TargetID;
    UINT16  Bus;
    UINT16  DevHandle;
    UINT16  Reserved;
} SYM_BTDH_MAPPING, * PTR_SYM_BTDH_MAPPING;
#pragma pack()

#pragma pack(1)
typedef struct _SYM_DIAG_ACTION
{
    UINT32  Action;
    UINT32  Length;
    UINT64  PtrDiagAction;
    UINT32  ReturnCode;
} SYM_DIAG_ACTION, * PTR_SYM_DIAG_ACTION;
#pragma pack()

#define FW_DIAGNOSTIC_BUFFER_COUNT              (3)
#define FW_DIAGNOSTIC_UID_NOT_FOUND             (0xFF)

#define MPI_FW_DIAG_NEW                         (0x806E6577)

#define MPI_FW_DIAG_TYPE_REGISTER               (0x00000001)
#define MPI_FW_DIAG_TYPE_UNREGISTER             (0x00000002)
#define MPI_FW_DIAG_TYPE_QUERY                  (0x00000003)
#define MPI_FW_DIAG_TYPE_READ_BUFFER            (0x00000004)
#define MPI_FW_DIAG_TYPE_RELEASE                (0x00000005)

#define MPI_FW_DIAG_INVALID_UID                 (0x00000000)

#define MPI_FW_DIAG_ERROR_SUCCESS               (0x00000000)
#define MPI_FW_DIAG_ERROR_FAILURE               (0x00000001)
#define MPI_FW_DIAG_ERROR_INVALID_PARAMETER     (0x00000002)
#define MPI_FW_DIAG_ERROR_POST_FAILED           (0x00000010)
#define MPI_FW_DIAG_ERROR_INVALID_UID           (0x00000011)
#define MPI_FW_DIAG_ERROR_RELEASE_FAILED        (0x00000012)
#define MPI_FW_DIAG_ERROR_NO_BUFFER             (0x00000013)
#define MPI_FW_DIAG_ERROR_ALREADY_RELEASED      (0x00000014)

#pragma pack(1)
typedef struct _MPI_FW_DIAG_REGISTER
{
    UINT8   TraceLevel;
    UINT8   BufferType;
    UINT16  Flags;
    UINT32  ExtendedType;
    UINT32  ProductSpecific[4];
    UINT32  RequestedBufferSize;
    UINT32  UniqueId;
} MPI_FW_DIAG_REGISTER, * PTR_MPI_FW_DIAG_REGISTER;
#pragma pack()

#pragma pack(1)
typedef struct _MPI_FW_DIAG_UNREGISTER
{
    UINT32                 UniqueId;
} MPI_FW_DIAG_UNREGISTER, * PTR_MPI_FW_DIAG_UNREGISTER;
#pragma pack()

#define MPI_FW_DIAG_FLAG_APP_OWNED              (0x0001)
#define MPI_FW_DIAG_FLAG_BUFFER_VALID           (0x0002)
#define MPI_FW_DIAG_FLAG_FW_BUFFER_ACCESS       (0x0004)

#pragma pack(1)
typedef struct _MPI_FW_DIAG_QUERY
{
    UINT8   TraceLevel;
    UINT8   BufferType;
    UINT16  Flags;
    UINT32  ExtendedType;
    UINT32  ProductSpecific[4];
    UINT32  TotalBufferSize;
    UINT32  DriverAddedBufferSize;
    UINT32  UniqueId;
} MPI_FW_DIAG_QUERY, * PTR_MPI_FW_DIAG_QUERY;
#pragma pack()

#pragma pack(1)
typedef struct _MPI_FW_DIAG_RELEASE
{
    UINT32  UniqueId;
} MPI_FW_DIAG_RELEASE, * PTR_MPI_FW_DIAG_RELEASE;
#pragma pack()

#define MPI_FW_DIAG_FLAG_REREGISTER             (0x0001)
#define MPI_FW_DIAG_FLAG_FORCE_RELEASE          (0x0002)

#pragma pack(1)
typedef struct _MPI_FW_DIAG_READ_BUFFER
{
    UINT8   Status;
    UINT8   Reserved;
    UINT16  Flags;
    UINT32  StartingOffset;
    UINT32  BytesToRead;
    UINT32  UniqueId;
    UINT32  DataBuffer[1];
} MPI_FW_DIAG_READ_BUFFER, * PTR_MPI_FW_DIAG_READ_BUFFER;
#pragma pack()

#endif
