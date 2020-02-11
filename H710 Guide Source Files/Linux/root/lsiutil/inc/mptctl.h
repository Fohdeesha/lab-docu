/***************************************************************************
 *                                                                         *
 *  Copyright 2012 LSI Corporation.  All rights reserved.                  *
 *                                                                         *
 *  This file is confidential and a trade secret of LSI Corporation.  The  *
 *  receipt of or possession of this file does not convey any rights to    *
 *  reproduce or disclose its contents or to manufacture, use, or sell     *
 *  anything it may describe, in whole, or in part, without the specific   *
 *  written consent of LSI Corporation.                                    *
 *                                                                         *
 ***************************************************************************
*/

#ifndef MPTCTL_H_INCLUDED
#define MPTCTL_H_INCLUDED
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include "linux/version.h"
#include "../lsi/mpi_ioc.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*
 *
 */
#define MPT_MISCDEV_BASENAME            "mptctl"
#define MPT_MISCDEV_PATHNAME            "/dev/" MPT_MISCDEV_BASENAME
#define MPT_CSMI_DESCRIPTION	        "LSI Logic Corporation: Fusion MPT Driver " MPT_LINUX_VERSION_COMMON

#define MPT_PRODUCT_LENGTH              12

/*
 *  Generic MPT Control IOCTLs and structures
 */
#define MPT_MAGIC_NUMBER	'm'

#define MPTRWPERF		_IOWR(MPT_MAGIC_NUMBER,0,struct mpt_raw_r_w)

#define MPTFWDOWNLOAD		_IOWR(MPT_MAGIC_NUMBER,15,struct mpt_fw_xfer)
#define MPTFWDOWNLOADBOOT	_IOWR(MPT_MAGIC_NUMBER,16,struct mpt_fw_xfer)
#define MPTCOMMAND		_IOWR(MPT_MAGIC_NUMBER,20,struct mpt_ioctl_command)

#if defined(__KERNEL__) && defined(MPT_CONFIG_COMPAT)
#define MPTFWDOWNLOAD32		_IOWR(MPT_MAGIC_NUMBER,15,struct mpt_fw_xfer32)
#define MPTCOMMAND32		_IOWR(MPT_MAGIC_NUMBER,20,struct mpt_ioctl_command32)
#endif

#define MPTIOCINFO		_IOWR(MPT_MAGIC_NUMBER,17,struct mpt_ioctl_iocinfo)
#define MPTIOCINFO1		_IOWR(MPT_MAGIC_NUMBER,17,struct mpt_ioctl_iocinfo_rev0)
#define MPTIOCINFO2		_IOWR(MPT_MAGIC_NUMBER,17,struct mpt_ioctl_iocinfo_rev1)
#define MPTTARGETINFO		_IOWR(MPT_MAGIC_NUMBER,18,struct mpt_ioctl_targetinfo)
#define MPTTEST			_IOWR(MPT_MAGIC_NUMBER,19,struct mpt_ioctl_test)
#define MPTEVENTQUERY		_IOWR(MPT_MAGIC_NUMBER,21,struct mpt_ioctl_eventquery)
#define MPTEVENTENABLE		_IOWR(MPT_MAGIC_NUMBER,22,struct mpt_ioctl_eventenable)
#define MPTEVENTREPORT		_IOWR(MPT_MAGIC_NUMBER,23,struct mpt_ioctl_eventreport)
#define MPTHARDRESET		_IOWR(MPT_MAGIC_NUMBER,24,struct mpt_ioctl_diag_reset)
#define MPTFWREPLACE		_IOWR(MPT_MAGIC_NUMBER,25,struct mpt_ioctl_replace_fw)
#define MPTDIAGREGISTER		_IOWR(MPT_MAGIC_NUMBER,26,mpt_diag_register_t)
#define MPTDIAGRELEASE		_IOWR(MPT_MAGIC_NUMBER,27,mpt_diag_release_t)
#define MPTDIAGUNREGISTER	_IOWR(MPT_MAGIC_NUMBER,28,mpt_diag_unregister_t)
#define MPTDIAGQUERY		_IOWR(MPT_MAGIC_NUMBER,29,mpt_diag_query_t)
#define MPTDIAGREADBUFFER	_IOWR(MPT_MAGIC_NUMBER,30,mpt_diag_read_buffer_t)
#define MPTHBAPCIINFO		_IOWR(MPT_MAGIC_NUMBER,31,struct mpt_ioctl_hbapciinfo)
#define MPTDIAGRESET		_IOWR(MPT_MAGIC_NUMBER,32,struct mpt_ioctl_diag_reset)

/*
 * SPARC PLATFORM REMARKS:
 * IOCTL data structures that contain pointers
 * will have different sizes in the driver and applications
 * (as the app. will not use 8-byte pointers).
 * Apps should use MPTFWDOWNLOAD and MPTCOMMAND.
 * The driver will convert data from
 * mpt_fw_xfer32 (mpt_ioctl_command32) to mpt_fw_xfer (mpt_ioctl_command)
 * internally.
 *
 * If data structures change size, must handle as in IOCGETINFO.
 */
struct mpt_fw_xfer {
	unsigned int	 iocnum;	/* IOC unit number */
	unsigned int	 fwlen;
	void		*bufp;		/* Pointer to firmware buffer */
};

#if defined(__KERNEL__) && defined(MPT_CONFIG_COMPAT)
struct mpt_fw_xfer32 {
	unsigned int iocnum;
	unsigned int fwlen;
	U32 bufp;
};
#endif	/*}*/

/*
 *  IOCTL header structure.
 *  iocnum - must be defined.
 *  port - must be defined for all IOCTL commands other than MPTIOCINFO
 *  maxDataSize - ignored on MPTCOMMAND commands
 *		- ignored on MPTFWREPLACE commands
 *		- on query commands, reports the maximum number of bytes to be returned
 *		  to the host driver (count includes the header).
 *		  That is, set to sizeof(struct mpt_ioctl_iocinfo) for fixed sized commands.
 *		  Set to sizeof(struct mpt_ioctl_targetinfo) + datasize for variable
 *			sized commands. (MPTTARGETINFO, MPTEVENTREPORT)
 */
typedef struct _mpt_ioctl_header {
	unsigned int	 iocnum;	/* IOC unit number */
	unsigned int	 port;		/* IOC port number */
	int		 maxDataSize;	/* Maximum Num. bytes to transfer on read */
} mpt_ioctl_header;

/*
 * Issue a diagnostic reset
 */
struct mpt_ioctl_diag_reset {
	mpt_ioctl_header hdr;
};


/*
 *  PCI bus/device/function information structure.
 */
struct mpt_ioctl_pci_info {
	union {
		struct {
			unsigned int  deviceNumber   :  5;
			unsigned int  functionNumber :  3;
			unsigned int  busNumber      : 24;
		} bits;
		unsigned int  asUlong;
	} u;
};

struct mpt_ioctl_pci_info2 {
	union {
		struct {
			unsigned int  deviceNumber   :  5;
			unsigned int  functionNumber :  3;
			unsigned int  busNumber      : 24;
		} bits;
		unsigned int  asUlong;
	} u;
  int segmentID;
};

/*
 *  Adapter Information Page
 *  Read only.
 *  Data starts at offset 0xC
 */
#define MPT_IOCTL_INTERFACE_SCSI	(0x00)
#define MPT_IOCTL_INTERFACE_FC		(0x01)
#define MPT_IOCTL_INTERFACE_SAS		(0x03)
#define MPT_IOCTL_VERSION_LENGTH	(32)

struct mpt_ioctl_iocinfo {
	mpt_ioctl_header hdr;
	int		 adapterType;	/* SCSI or FCP */
	int		 port;		/* port number */
	int		 pciId;		/* PCI Id. */
	int		 hwRev;		/* hardware revision */
	int		 subSystemDevice;	/* PCI subsystem Device ID */
	int		 subSystemVendor;	/* PCI subsystem Vendor ID */
	int		 numDevices;		/* number of devices */
	int		 FWVersion;		/* FW Version (integer) */
	int		 BIOSVersion;		/* BIOS Version (integer) */
	char		 driverVersion[MPT_IOCTL_VERSION_LENGTH];	/* Driver Version (string) */
	char		 busChangeEvent;
	char		 hostId;
	char		 rsvd[2];
	struct mpt_ioctl_pci_info2  pciInfo; /* Added Rev 2 */
};

struct mpt_ioctl_iocinfo_rev1 {
	mpt_ioctl_header hdr;
	int		 adapterType;	/* SCSI or FCP */
	int		 port;		/* port number */
	int		 pciId;		/* PCI Id. */
	int		 hwRev;		/* hardware revision */
	int		 subSystemDevice;	/* PCI subsystem Device ID */
	int		 subSystemVendor;	/* PCI subsystem Vendor ID */
	int		 numDevices;		/* number of devices */
	int		 FWVersion;		/* FW Version (integer) */
	int		 BIOSVersion;		/* BIOS Version (integer) */
	char		 driverVersion[MPT_IOCTL_VERSION_LENGTH];	/* Driver Version (string) */
	char		 busChangeEvent;
	char		 hostId;
	char		 rsvd[2];
	struct mpt_ioctl_pci_info  pciInfo; /* Added Rev 1 */
};

/* Original structure, must always accept these
 * IOCTLs. 4 byte pads can occur based on arch with
 * above structure. Wish to re-align, but cannot.
 */
struct mpt_ioctl_iocinfo_rev0 {
	mpt_ioctl_header hdr;
	int		 adapterType;	/* SCSI or FCP */
	int		 port;		/* port number */
	int		 pciId;		/* PCI Id. */
	int		 hwRev;		/* hardware revision */
	int		 subSystemDevice;	/* PCI subsystem Device ID */
	int		 subSystemVendor;	/* PCI subsystem Vendor ID */
	int		 numDevices;		/* number of devices */
	int		 FWVersion;		/* FW Version (integer) */
	int		 BIOSVersion;		/* BIOS Version (integer) */
	char		 driverVersion[MPT_IOCTL_VERSION_LENGTH];	/* Driver Version (string) */
	char		 busChangeEvent;
	char		 hostId;
	char		 rsvd[2];
};

/*
 * Device Information Page
 * Report the number of, and ids of, all targets
 * on this IOC.  The ids array is a packed structure
 * of the known targetInfo.
 * bits 31-24: reserved
 *      23-16: LUN
 *      15- 8: Bus Number
 *       7- 0: Target ID
 */
struct mpt_ioctl_targetinfo {
	mpt_ioctl_header hdr;
	int		 numDevices;	/* Num targets on this ioc */
	int		 targetInfo[1];
};


/*
 * Event reporting IOCTL's.  These IOCTL's will
 * use the following defines:
 */
struct mpt_ioctl_eventquery {
	mpt_ioctl_header hdr;
	unsigned short	 eventEntries;
	unsigned short	 reserved;
	unsigned int	 eventTypes;
};

struct mpt_ioctl_eventenable {
	mpt_ioctl_header hdr;
	unsigned int	 eventTypes;
};

#ifndef __KERNEL__
typedef struct {
	uint	event;
	uint	eventContext;
	uint	data[2];
} MPT_IOCTL_EVENTS;
#endif

struct mpt_ioctl_eventreport {
	mpt_ioctl_header	hdr;
	MPT_IOCTL_EVENTS	eventData[1];
};

#define MPT_MAX_NAME	32
struct mpt_ioctl_test {
	mpt_ioctl_header hdr;
	U8		 name[MPT_MAX_NAME];
	int		 chip_type;
	U8		 product [MPT_PRODUCT_LENGTH];
};

/* Replace the FW image cached in host driver memory
 * newImageSize - image size in bytes
 * newImage - first byte of the new image
 */
typedef struct mpt_ioctl_replace_fw {
	mpt_ioctl_header hdr;
	int		 newImageSize;
	U8		 newImage[1];
} mpt_ioctl_replace_fw_t;


struct mpt_ioctl_mptpciinfo {
    U8  iocNumber;
    U8  iocState;
    U8  revisionID;
    U8  reserved1;
    U16 vendorID;
    U16 deviceID;
    U16 subSystemVendorID;
    U16 subSystemID;
};


struct mpt_ioctl_hbapciinfo {
	mpt_ioctl_header     hdr;
    U8                   totalIOC;
    U8                   reserved[3];
    struct mpt_ioctl_mptpciinfo hbapciinfo[18];
};


/* General MPT Pass through data structure
 *
 * iocnum
 * timeout - in seconds, command timeout. If 0, set by driver to
 *		default value.
 * replyFrameBufPtr - reply location
 * dataInBufPtr - destination for read
 * dataOutBufPtr - data source for write
 * senseDataPtr - sense data location
 * maxReplyBytes - maximum number of reply bytes to be sent to app.
 * dataInSize - num bytes for data transfer in (read)
 * dataOutSize - num bytes for data transfer out (write)
 * dataSgeOffset - offset in words from the start of the request message
 *		to the first SGL
 * MF[1];
 *
 * Remark:  Some config pages have bi-directional transfer,
 * both a read and a write. The basic structure allows for
 * a bidirectional set up. Normal messages will have one or
 * both of these buffers NULL.
 */
struct mpt_ioctl_command {
	mpt_ioctl_header hdr;
	int		timeout;	/* optional (seconds) */
	char		*replyFrameBufPtr;
	char		*dataInBufPtr;
	char		*dataOutBufPtr;
	char		*senseDataPtr;
	int		maxReplyBytes;
	int		dataInSize;
	int		dataOutSize;
	int		maxSenseBytes;
	int		dataSgeOffset;
	char		MF[1];
};

/*
 * SPARC PLATFORM: See earlier remark.
 */
#if defined(__KERNEL__) && defined(MPT_CONFIG_COMPAT)
struct mpt_ioctl_command32 {
	mpt_ioctl_header hdr;
	int	timeout;
	U32	replyFrameBufPtr;
	U32	dataInBufPtr;
	U32	dataOutBufPtr;
	U32	senseDataPtr;
	int	maxReplyBytes;
	int	dataInSize;
	int	dataOutSize;
	int	maxSenseBytes;
	int	dataSgeOffset;
	char	MF[1];
};
#endif	/*}*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*
 *	HP Specific IOCTL Defines and Structures
 */

#define CPQFCTS_IOC_MAGIC 'Z'
#define HP_IOC_MAGIC 'Z'
#define HP_GETHOSTINFO		_IOR(HP_IOC_MAGIC, 20, hp_host_info_t)
#define HP_GETHOSTINFO1		_IOR(HP_IOC_MAGIC, 20, hp_host_info_rev0_t)
#define HP_GETTARGETINFO	_IOR(HP_IOC_MAGIC, 21, hp_target_info_t)

/* All HP IOCTLs must include this header
 */
typedef struct _hp_header {
	unsigned int iocnum;
	unsigned int host;
	unsigned int channel;
	unsigned int id;
	unsigned int lun;
} hp_header_t;

/*
 *  Header:
 *  iocnum 	required (input)
 *  host 	ignored
 *  channe	ignored
 *  id		ignored
 *  lun		ignored
 */
typedef struct _hp_host_info {
	hp_header_t	 hdr;
	U16		 vendor;
	U16		 device;
	U16		 subsystem_vendor;
	U16		 subsystem_id;
	U8		 devfn;
	U8		 bus;
	ushort		 host_no;		/* SCSI Host number, if scsi driver not loaded*/
	U8		 fw_version[16];	/* string */
	U8		 serial_number[24];	/* string */
	U32		 ioc_status;
	U32		 bus_phys_width;
	U32		 base_io_addr;
	U32		 rsvd;
	unsigned int	 hard_resets;		/* driver initiated resets */
	unsigned int	 soft_resets;		/* ioc, external resets */
	unsigned int	 timeouts;		/* num timeouts */
} hp_host_info_t;

/* replace ulongs with uints, need to preserve backwards
 * compatibility.
 */
typedef struct _hp_host_info_rev0 {
	hp_header_t	 hdr;
	U16		 vendor;
	U16		 device;
	U16		 subsystem_vendor;
	U16		 subsystem_id;
	U8		 devfn;
	U8		 bus;
	ushort		 host_no;		/* SCSI Host number, if scsi driver not loaded*/
	U8		 fw_version[16];	/* string */
	U8		 serial_number[24];	/* string */
	U32		 ioc_status;
	U32		 bus_phys_width;
	U32		 base_io_addr;
	U32		 rsvd;
	unsigned long	 hard_resets;		/* driver initiated resets */
	unsigned long	 soft_resets;		/* ioc, external resets */
	unsigned long	 timeouts;		/* num timeouts */
} hp_host_info_rev0_t;

/*
 *  Header:
 *  iocnum 	required (input)
 *  host 	required
 *  channel	required	(bus number)
 *  id		required
 *  lun		ignored
 *
 *  All error values between 0 and 0xFFFF in size.
 */
typedef struct _hp_target_info {
	hp_header_t	 hdr;
	U32 parity_errors;
	U32 phase_errors;
	U32 select_timeouts;
	U32 message_rejects;
	U32 negotiated_speed;
	U8  negotiated_width;
	U8  rsvd[7];				/* 8 byte alignment */
} hp_target_info_t;

#define HP_STATUS_OTHER		1
#define HP_STATUS_OK		2
#define HP_STATUS_FAILED	3

#define HP_BUS_WIDTH_UNK	1
#define HP_BUS_WIDTH_8		2
#define HP_BUS_WIDTH_16		3
#define HP_BUS_WIDTH_32		4

#define HP_DEV_SPEED_ASYNC	2
#define HP_DEV_SPEED_FAST	3
#define HP_DEV_SPEED_ULTRA	4
#define HP_DEV_SPEED_ULTRA2	5
#define HP_DEV_SPEED_ULTRA160	6
#define HP_DEV_SPEED_SCSI1	7
#define HP_DEV_SPEED_ULTRA320	8

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define MPI_FW_DIAG_IOCTL               (0x80646961)    // dia
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
#define MPI_FW_DIAG_ERROR_INVALID_PARAMETER (0x00000002)
#define MPI_FW_DIAG_ERROR_POST_FAILED       (0x00000010)
#define MPI_FW_DIAG_ERROR_INVALID_UID       (0x00000011)
#define MPI_FW_DIAG_ERROR_RELEASE_FAILED    (0x00000012)
#define MPI_FW_DIAG_ERROR_NO_BUFFER         (0x00000013)
#define MPI_FW_DIAG_ERROR_ALREADY_RELEASED  (0x00000014)

#define MPT_DIAG_CAPABILITY(bufftype) (MPI_IOCFACTS_CAPABILITY_DIAG_TRACE_BUFFER << bufftype)

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

typedef struct _mpt_diag_register {
	mpt_ioctl_header hdr;
	MPI_FW_DIAG_REGISTER data;
} mpt_diag_register_t;

typedef struct _MPI_FW_DIAG_UNREGISTER
{
    U32                 UniqueId;
} MPI_FW_DIAG_UNREGISTER, *PTR_MPI_FW_DIAG_UNREGISTER;

typedef struct _mpt_diag_unregister {
	mpt_ioctl_header hdr;
	MPI_FW_DIAG_UNREGISTER data;
} mpt_diag_unregister_t;

#define MPI_FW_DIAG_FLAG_APP_OWNED          (0x0001)
#define MPI_FW_DIAG_FLAG_BUFFER_VALID       (0x0002)
#define MPI_FW_DIAG_FLAG_FW_BUFFER_ACCESS   (0x0004)

typedef struct _MPI_FW_DIAG_QUERY
{
    U8                  TraceLevel;
    U8                  BufferType;
    U16                 Flags;
    U32                 ExtendedType;
    U32                 ProductSpecific[4];
    U32                 DataSize;
    U32                 DriverAddedBufferSize;
    U32                 UniqueId;
} MPI_FW_DIAG_QUERY, *PTR_MPI_FW_DIAG_QUERY;

typedef struct _mpt_diag_query {
	mpt_ioctl_header hdr;
	MPI_FW_DIAG_QUERY data;
} mpt_diag_query_t;

typedef struct _MPI_FW_DIAG_RELEASE
{
    U32                 UniqueId;
} MPI_FW_DIAG_RELEASE, *PTR_MPI_FW_DIAG_RELEASE;

typedef struct _mpt_diag_release {
	mpt_ioctl_header hdr;
	MPI_FW_DIAG_RELEASE data;
} mpt_diag_release_t;

#define MPI_FW_DIAG_FLAG_REREGISTER         (0x0001)

typedef struct _MPI_FW_DIAG_READ_BUFFER
{
    U8                  Status;
    U8                  Reserved;
    U16                 Flags;
    U32                 StartingOffset;
    U32                 BytesToRead;
    U32                 UniqueId;
    U32                 DiagnosticData[1];
} MPI_FW_DIAG_READ_BUFFER, *PTR_MPI_FW_DIAG_READ_BUFFER;

typedef struct _mpt_diag_read_buffer {
	mpt_ioctl_header hdr;
	MPI_FW_DIAG_READ_BUFFER data;
} mpt_diag_read_buffer_t;

typedef struct _mpt_FWDownload_MF {
	FWDownload_t	FWMessage;
	U32		SGL_Word;
} mpt_FWDownload_MF_t;

#endif
