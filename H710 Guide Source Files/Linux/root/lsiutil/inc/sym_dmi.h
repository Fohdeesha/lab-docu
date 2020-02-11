/* FILE: sym_dmi.h - MPT/Fusion Driver IOCTL Support header file
*
*************************************************************************
*                                                                       *
*   Copyright 2001 LSI Logic. All rights reserved.                      *
*                                                                       *
*   This file is confidential and a trade secret of LSI Logic.          *
*   The receipt of or possession of this file does not convey any       *
*   rights to reproduce or disclose its contents or to manufacture,     *
*   use, or sell anything is may describe, in whole, or in part,        *
*   without the specific written consent of LSI Logic.                  *
*                                                                       *
************************************************************************/

/*+++HDR
 *
 *  Version History
 *  ---------------
 *
 *    Date    Who?  Description
 *  --------  ----  -------------------------------------------------------
 *  03/20/00  SAM   Initial version
 *  02/14/01  SCH   Added Domain Validation
 *  05/14/01  SCH   Added MPI Event logging
 *  07/09/01  SCH   Changed MAX_VERSION_STRING from 80 to 32
 *  01/02/02  SCH   Added MPI diagnostic reset IOCTL
 *  10/30/03  SCH   Added InBand IOCTL support
 *  03/24/04  BJH   Added Diagnostic IOCTL support
 *  09/10/08  SCH   Added ADAPTER_TYPE_SAS2 define
 *  10/20/08  SCH   Modified diag buffer structures for Gen2
---*/

#ifndef SYM_DMI_H
#define SYM_DMI_H

#define VERSION_SIZE            4
#define MAX_VERSION_STRING      32

// support for MPI messages via an application or service IOCTL

#define MPI_MSG_IOCTL           0x806D7069  // mpi
#define DATA_FROM_APP           0x01
#define SCSI_IO                 0x4000
#define DUAL_SGLS               0x8000

typedef struct _IOCTL_DETAILS {
    U16         Dir;        // data xfer direction
    U16         MsgSize;    // size in 32 bit words
    U32         DataSize;   // data xfer size in bytes
    U32         FormattedMsg[1];
} IOCTL_DETAILS, *pIOCTL_DETAILS;


// support for host driver info / MPI event logging IOCTL

#define DRVR_INFO_IOCTL         0x8043494D  // CIM

/* Adapter Types */
#define ADAPTER_TYPE_SCSI         0
#define ADAPTER_TYPE_SCSI_OVER_FC 1
#define ADAPTER_TYPE_IP_OVER_FC   2
#define ADAPTER_TYPE_SAS          3
#define ADAPTER_TYPE_SAS2         4

/* Page Codes */
#define ADAPTER_INFO_PAGE         1
#define PCI_CONFIG_SPACE_PAGE     3
#define EVENT_QUERY              10
#define EVENT_ENABLE             11
#define GET_EVENTS               12

/* Event Queue Defines */
#define EVENT_QUEUE_SIZE         50
#define MAX_EVENT_DATA_LENGTH    48

// structure to hold PCI bus/device/function information
typedef struct _MPI_PCI_INFO {
    union {
        struct {
            ULONG   DeviceNumber:5;
            ULONG   FunctionNumber:3;
            ULONG   BusNumber:24;
        } bits;
        ULONG   AsULONG;
    } u;
} MPI_PCI_INFO, *PMPI_PCI_INFO;

// Adapter Info structure
typedef struct _DRVR_ADAPTER_INFO_OUT
{
    int     AdapterType;
    int     MpiPortNumber;
    int     PciDeviceId;
    int     PciDeviceHwRev;
    int     SubSystem;
    int     SubSystemVendorId;
    int     NumDevicesAttached;
    int     MpiVersion;
    U32     BiosVersion;
    char    DriverVersion[MAX_VERSION_STRING];
    BOOLEAN BusChangeEvent;
    U8      HostId;
    U16     Reserved1;
    MPI_PCI_INFO  PciInfo;
    U32     PciSegmentId;
} DRVR_ADAPTER_INFO_OUT, *PTR_DRVR_ADAPTER_INFO_OUT;

typedef struct _DRVR_INFO_SRB
{
    SRB_IO_CONTROL      Sic;
    int                 PageCode;                   // [IN]
    char                PageVersion[VERSION_SIZE];  // [OUT]
    DRVR_ADAPTER_INFO_OUT   AdapterPageOut;
} DRVR_INFO_SRB, *PTR_DRVR_INFO_SRB;

typedef struct _PCI_CONFIG_SPACE_SRB
{
    SRB_IO_CONTROL      Sic;
    int                 PageCode;                   // [IN]
    char                PageVersion[VERSION_SIZE];  // [OUT]
    U32                 reserved[3];
    U8                  PciConfigSpace[256];
} PCI_CONFIG_SPACE_SRB, *PTR_PCI_CONFIG_SPACE_SRB;

// support for MPI Event Queue

typedef struct _MPI_EVENTS_SRB
{
    SRB_IO_CONTROL      Sic;
    int                 PageCode;                   // [IN]
    char                PageVersion[VERSION_SIZE];  // [OUT]
    U32                 EventData[1];               // page code dependent
} MPI_EVENTS_SRB, *PTR_MPI_EVENTS_SRB;

typedef struct _MPI_EVENTS_QUEUE
{
    U32                 EventType;                  // from reply frame
    U32                 EventNumber;                // from DevExt->EventNum
    U32                 EventData[MAX_EVENT_DATA_LENGTH];   // from reply frame
} MPI_EVENTS_QUEUE, *PTR_MPI_EVENTS_QUEUE;

// support for MPI Diagnostic Reset IOCTL

#define MPI_DIAG_RESET          0x80727374          // rst

typedef struct _MPI_DIAG_RESET_SRB
{
    SRB_IO_CONTROL      Sic;
    U32                 NewImageSize;               // replacement image size
    U32                 NewFWImage[1];              // new FW image to save
} MPI_DIAG_RESET_SRB, *PTR_MPI_DIAG_RESET_SRB;
// A NewImageSize of 0 indicates that no new FW image is being downloaded

// MPI register reae/write access IOCTL
#define MPI_REG_ACCESS          0x80771959          // magic number
#define REG_IO_READ             1
#define REG_IO_WRITE            2
#define REG_MEM_READ            3
#define REG_MEM_WRITE           4

typedef struct _MPI_REG_ACCESS_SRB
{
    SRB_IO_CONTROL      Sic;
    U32                 Command;                    // command (read/write)
    U32                 RegOffset;                  // register offset
    U32                 RegData;                    // register data
} MPI_REG_ACCESS_SRB, *PTR_MPI_REG_ACCESS_SRB;

// support for bus/target - DevHandle mapping IOCTL
// application can suppy bus/target and get devHandle or
// supply devHandle and get bus/target

#define MPI_BTDH_MAPPING        0x804D4150          // MAP

typedef struct _MPI_BTDH_MAP_SRB
{
    SRB_IO_CONTROL      Sic;
    U32                 TargetID;                   // target ID
    U32                 Bus;                        // bus
    U16                 DevHandle;                  // device handle
    U16                 Reserved;
} MPI_BTDH_MAP_SRB, *PTR_MPI_BTDH_MAP_SRB;



#define MPI_FW_DIAG_IOCTL               (0x80646961)    // dia
#define MPI_FW_DIAG_NEW                 (0x806E6577)    // new
#define MPI_FW_DIAG_TYPE_REGISTER       (0x00000001)
#define MPI_FW_DIAG_TYPE_UNREGISTER     (0x00000002)
#define MPI_FW_DIAG_TYPE_QUERY          (0x00000003)
#define MPI_FW_DIAG_TYPE_READ_BUFFER    (0x00000004)
#define MPI_FW_DIAG_TYPE_RELEASE        (0x00000005)

#define MPI_FW_DIAG_INVALID_UID         (0x00000000)
#define FW_DIAGNOSTIC_BUFFER_COUNT      (3)
#define FW_DIAGNOSTIC_UID_NOT_FOUND     (0xFF)

#define MPI_FW_DIAG_ERROR_SUCCESS           (0x00000000)
#define MPI_FW_DIAG_ERROR_FAILURE           (0x00000001)
#define MPI_FW_DIAG_ERROR_INVALID_PARAMETER (0x00000003)
#define MPI_FW_DIAG_ERROR_POST_FAILED       (0x00000010)
#define MPI_FW_DIAG_ERROR_INVALID_UID       (0x00000011)
#define MPI_FW_DIAG_ERROR_RELEASE_FAILED    (0x00000012)
#define MPI_FW_DIAG_ERROR_NO_BUFFER         (0x00000013)
#define MPI_FW_DIAG_ERROR_ALREADY_RELEASED  (0x00000014)

typedef struct _MPI_FW_DIAG_IOCTL_SRB
{
    SRB_IO_CONTROL      Sic;
    U32                 MpiDiagType;
    U8                  PageVersion[4];
    U32                 MpiDiagData[1];
} MPI_FW_DIAG_IOCTL_SRB, *PTR_MPI_FW_DIAG_IOCTL_SRB;

/* diag register for gen 1 */
typedef struct _MPI_FW_DIAG_REGISTER
{
    U8                  TraceLevel;
    U8                  BufferType;
    U16                 Flags;
    U32                 ExtendedType;
    U32                 ProductSpecific[4];
    U32                 RequestedBufferSize;
    U32                 UniqueId;
} MPI_FW_DIAG_REGISTER, *PTR_MPI_FW_DIAG_REGISTER;

typedef struct _MPI_FW_DIAG_UNREGISTER
{
    U32                 UniqueId;
} MPI_FW_DIAG_UNREGISTER, *PTR_MPI_FW_DIAG_UNREGISTER;

#define MPI_FW_DIAG_FLAG_APP_OWNED          (0x0001)
#define MPI_FW_DIAG_FLAG_BUFFER_VALID       (0x0002)
#define MPI_FW_DIAG_FLAG_FW_BUFFER_ACCESS   (0x0004)

/* diag query for gen 1 */
typedef struct _MPI_FW_DIAG_QUERY
{
    U8                  TraceLevel;
    U8                  BufferType;
    U16                 Flags;
    U32                 ExtendedType;
    U32                 ProductSpecific[4];
    U32                 TotalBufferSize;
    U32                 DriverAddedBufferSize;
    U32                 UniqueId;
} MPI_FW_DIAG_QUERY, *PTR_MPI_FW_DIAG_QUERY;

typedef struct _MPI_FW_DIAG_RELEASE
{
    U32                 UniqueId;
} MPI_FW_DIAG_RELEASE, *PTR_MPI_FW_DIAG_RELEASE;

#define MPI_FW_DIAG_FLAG_REREGISTER         (0x0001)
#define MPI_FW_DIAG_FLAG_FORCE_RELEASE      (0x0002)

typedef struct _MPI_FW_DIAG_READ_BUFFER
{
    U8                  Status;
    U8                  Reserved;
    U16                 Flags;
    U32                 StartingOffset;
    U32                 BytesToRead;
    U32                 UniqueId;
    U32                 DataBuffer[1];
} MPI_FW_DIAG_READ_BUFFER, *PTR_MPI_FW_DIAG_READ_BUFFER;

#endif
