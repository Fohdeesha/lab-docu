/***************************************************************************
 *                                                                         *
 *  Copyright 2008 LSI Logic Corporation.  All rights reserved.            *
 *                                                                         *
 *  This file is confidential and a trade secret of LSI Logic.  The        *
 *  receipt of or possession of this file does not convey any rights to    *
 *  reproduce or disclose its contents or to manufacture, use, or sell     *
 *  anything it may describe, in whole, or in part, without the specific   *
 *  written consent of LSI Logic Corporation.                              *
 *                                                                         *
 ***************************************************************************
 *
 *           Name:  SAS.H
 *          Title:  SAS Protocol Structures and Definitions
 *     Programmer:  -------------
 *  Creation Date:  January 20, 2003
 *
 *  Version History
 *  ---------------
 *
 *  Last Updated
 *  -------------
 *  Version         %version: 52 %
 *  Date Updated    %date_modified: %
 *  Programmer      %created_by: mjwang %
 *
 *  Date      Who   Description
 *  --------  ---   -------------------------------------------------------
 *  01/20/03  GWK   Initial version
 *  10/24/04  BDB   Updated to SAS 1.1 Revision 6
 *  08/20/08  KAM   updated for SDM big endian platform and gcc bitfields
 *  12/09/08  RTS   updated to SAS 2.0 Revision 15
 *
 *
 * Description
 * ------------
 * This include file contains SAS protocol structures and definitions. This
 * file only should include structures and definitions specific to SAS. SCSI
 * CDBs used in SAS belong in scsi.h.
 *
 * NOTE: This version is current as of version 2.0 Rev 15 of the SAS specification.
 *
 * NOTE: This file is dependant on ata.h, so include ata.h before sas.h.
 *
 *-------------------------------------------------------------------------
 */

/* If this header file has not been included yet */
#ifndef SAS_H_SOURCE
#define SAS_H_SOURCE

/* None of the frames defined included CRC. The HW deals with
 * CRC and we can't access it.
 */


/******************************************************************************/
/*                                                                            */
/*      S A S   A D D R E S S   D E F I N I T I O N S                         */
/*                                                                            */
/******************************************************************************/

/*
 * SAS Address (full 8 byte WWID)
 * This structure is in little endian, so a SAS address of 0x50(MSB) 08 05 EF FF FF 80 00(LSB)
 * would show up here as High = 0xEF050850, Low = 0x0080FFFF
 */
#if 1  //KAM
#define SAS_WWID_LENGTH                                         (8)
typedef union _SAS_ADDRESS
{
    U8                      WWID[SAS_WWID_LENGTH];              /* 0x00 */
    struct
    {
        U32                 High;
        U32                 Low;
    }Word;

} SAS_ADDRESS, *PTR_SAS_ADDRESS;
#endif

/******************************************************************************/
/*                                                                            */
/*      S A S   L U N   D E F I N I T I O N                                   */
/*                                                                            */
/******************************************************************************/
typedef union _SAS_LOGICAL_UNIT
{
    U8  Byte[8];
    U16 LevelAddressing[4];
    U32 Dword[2];
    struct {
        U8 Reserved1;
        U8 Lun;
        U8 Reserved2[6];
    } SingleLevel;
} SAS_LOGICAL_UNIT, *PTR_SAS_LOGICAL_UNIT;

/* SAS Device Type field */
#define SAS_DEVICE_TYPE_END_DEVICE                              (0x01)
#define SAS_DEVICE_TYPE_EDGE_EXPANDER                           (0x02)
#define SAS_DEVICE_TYPE_FANOUT_EXPANDER                         (0x03)


/******************************************************************************/
/*                                                                            */
/*      S S P   F R A M E   D E F I N I T I O N S                             */
/*                                                                            */
/******************************************************************************/

/*
 * COMMAND Information Unit
 */
#define SAS_COMMAND_IU_CDB_LENGTH                               (16)
typedef struct _COMMAND_IU
{
    SAS_LOGICAL_UNIT        LogicalUnitNumber;                  /* 0x00 */
    U8                      Reserved08;                         /* 0x08 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      EnableFirstBurst                :1; /* 0x09 */
    U8                      Reserved09Bits3to6              :4;
    U8                      TaskAttribute                   :3;
    U8                      Reserved0A;                         /* 0x0A */
    U8                      AdditionalCDBLength             :6; /* 0x0B */
    U8                      Reserved0BBits0to1              :2;
#else              /* little endian */
    U8                      TaskAttribute                   :3; /* 0x09 */
    U8                      Reserved09Bits3to6              :4;
    U8                      EnableFirstBurst                :1;
    U8                      Reserved0A;                         /* 0x0A */
    U8                      Reserved0BBits0to1              :2; /* 0x0B */
    U8                      AdditionalCDBLength             :6;
#endif
    U8                      CDB[SAS_COMMAND_IU_CDB_LENGTH];     /* 0x0C */
    U32                     AdditionalCDBBytes[1];              /* 0x1C */
    /* Note: AdditionalCDBBytes is variable and may really be 0 to 0x3F dwords */
} COMMAND_IU, *PTR_COMMAND_IU;

/* Task Attribute field */
#define SSP_COMMAND_IU_TASK_ATTRIBUTE_SIMPLE                    (0x0)
#define SSP_COMMAND_IU_TASK_ATTRIBUTE_HEAD_OF_QUEUE             (0x1)
#define SSP_COMMAND_IU_TASK_ATTRIBUTE_ORDERED                   (0x2)
#define SSP_COMMAND_IU_TASK_ATTRIBUTE_ACA                       (0x4)

/*
 * TASK Information Unit
 */
typedef struct _TASK_IU
{
    SAS_LOGICAL_UNIT        LogicalUnitNumber;                  /* 0x00 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      Reserved09;                         /* 0x09 */
    U8                      TaskManagementFunction;             /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U16                     TagOfTaskToBeManaged;               /* 0x0C */
    U8                      Reserved0Eto1B[14];                 /* 0x0E */
} TASK_IU, *PTR_TASK_IU;

/* Task Management Function field */
#define SSP_TASK_IU_TASK_MANAGEMENT_FUNCTION_ABORT_TASK         (0x01)
#define SSP_TASK_IU_TASK_MANAGEMENT_FUNCTION_ABORT_TASK_SET     (0x02)
#define SSP_TASK_IU_TASK_MANAGEMENT_FUNCTION_CLEAR_TASK_SET     (0x04)
#define SSP_TASK_IU_TASK_MANAGEMENT_FUNCTION_LOGICAL_UNIT_RESET (0x08)
#define SSP_TASK_IU_TASK_MANAGEMENT_FUNCTION_CLEAR_ACA          (0x40)
#define SSP_TASK_IU_TASK_MANAGEMENT_FUNCTION_QUERY_TASK         (0x80)

/*
 * XFER_RDY Information Unit
 */
typedef struct _XFER_RDY_IU
{
    U32                     RequestedOffset;                    /* 0x00 */
    U32                     WriteDataLength;                    /* 0x04 */
    U8                      Reserved08to0B[4];                  /* 0x08 */
} XFER_RDY_IU, *PTR_XFER_RDY_IU;

/*
 * DATA Information Unit
 */
typedef struct _DATA_IU
{
    U8                      Data[1];                            /* 0x00 - ?? */
    /* Note the data length is variable */
} DATA_IU, *PTR_DATA_IU;

/*
 * RESPONSE Data field
 */
typedef struct _RESPONSE_DATA
{
    U8                      Reserved00;                         /* 0x00 */
    U8                      Reserved01;                         /* 0x01 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      ResponseCode;                       /* 0x03 */
} RESPONSE_DATA, *PTR_RESPONSE_DATA;

/* Response Code field */
#define SSP_RESPONSE_CODE_NO_FAILURE                            (0x00)
#define SSP_RESPONSE_CODE_TASK_MGMT_FUNCTION_COMPLETE           (0x00)
#define SSP_RESPONSE_CODE_INVALID_FRAME                         (0x02)
#define SSP_RESPONSE_CODE_TASK_MGMT_FUNCTION_NOT_SUPPORTED      (0x04)
#define SSP_RESPONSE_CODE_TASK_MGMT_FUNCTION_FAILED             (0x05)
#define SSP_RESPONSE_CODE_TASK_MGMT_FUNCTION_SUCCEEDED          (0x08)
#define SSP_RESPONSE_CODE_INVALID_LOGICAL_UNIT_NUMBER           (0x09)

/*
 * RESPONSE Information Unit
 */
typedef struct _RESPONSE_IU
{
    U8                      Reserved00to09[10];                 /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved0ABits2to7              :6;
    U8                      DataPres                        :2; /* 0x0A */
#else              /* little endian */
    U8                      DataPres                        :2; /* 0x0A */
    U8                      Reserved0ABits2to7              :6;
#endif
    U8                      Status;                             /* 0x0B */
    U32                     Reserved0Cto0F;                     /* 0x0C */
    U32                     SenseDataListLength;                /* 0x10 */
    U32                     ResponseDataListLength;             /* 0x14 */
    union
    {
        RESPONSE_DATA       ResponseData;
        U8                  SenseData[1];
        /* Note sense data length is variable */
    } ResponseOrSenseData;
} RESPONSE_IU, *PTR_RESPONSE_IU;

/* DataPres field */
#define SSP_RESPONSE_IU_DATAPRES_NO_DATA                        (0x00)
#define SSP_RESPONSE_IU_DATAPRES_RESPONSE_DATA                  (0x01)
#define SSP_RESPONSE_IU_DATAPRES_SENSE_DATA                     (0x02)

/*
 * Union of all IUs
 */
typedef union _SAS_IU
{
    COMMAND_IU          CommandIU;
    TASK_IU             TaskIU;
    XFER_RDY_IU         XferRdyIU;
    DATA_IU             DataIU;
    RESPONSE_IU         ResponseIU;
} SAS_IU, *PTR_SAS_IU;

/******************************************************************************/
/*                                                                            */
/*      S T P   F R A M E   D E F I N I T I O N S                             */
/*                                                                            */
/******************************************************************************/

/* STP frames are identical to SATA FIS's, so these are defined in ata.h */


/******************************************************************************/
/*                                                                            */
/*      S M P   F R A M E   D E F I N I T I O N S                             */
/*                                                                            */
/******************************************************************************/

/*
 * SMP Request
 */
typedef struct _SMP_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U8                      AdditionalRequestBytes[1];          /* 0x04 */
    /* Note AdditionalRequestBytes is of variable length based on function */
} SMP_REQUEST, *PTR_SMP_REQUEST;

/* SMP Frame Types */
#define SMP_FRAME_TYPE_SMP_REQUEST                              (0x40)
#define SMP_FRAME_TYPE_SMP_RESPONSE                             (0x41)

/* SMP Functions */
#define SMP_FUNCTION_REPORT_GENERAL                             (0x00)
#define SMP_FUNCTION_REPORT_MANUFACTURER_INFORMATION            (0x01)
#define SMP_FUNCTION_READ_GPIO                                  (0x02)
#define SMP_FUNCTION_REPORT_SELF_CONFIG_STATUS                  (0x03)
#define SMP_FUNCTION_DISCOVER                                   (0x10)
#define SMP_FUNCTION_REPORT_PHY_ERROR_LOG                       (0x11)
#define SMP_FUNCTION_REPORT_PHY_SATA                            (0x12)
#define SMP_FUNCTION_REPORT_ROUTE_INFORMATION                   (0x13)
#define SMP_FUNCTION_REPORT_EXP_ROUTE_TABLE                     (0x17)
#define SMP_FUNCTION_WRITE_GPIO                                 (0x82)
#define SMP_FUNCTION_CONFIGURE_ROUTE_INFORMATION                (0x90)
#define SMP_FUNCTION_PHY_CONTROL                                (0x91)
/* These are vendor unique functions for LSI expanders */
#define SMP_FUNCTION_RING_WRITE_N_REGISTERS                     (0xC0)
#define SMP_FUNCTION_RING_READ_N_REGISTERS                      (0x40)
#define SMP_FUNCTION_ICL_DISCOVER                               (0x41)

#define SMP_FUNCTION_AHB_WRITE_N_REGISTERS                      (0xC2)
#define SMP_FUNCTION_AHB_READ_N_REGISTERS                       (0x42)

/* These are for SGPIO use */
#define SMP_FUNCTION_READ_GPIO_REGISTERS                        (0x02)
#define SMP_FUNCTION_WRITE_GPIO_REGISTERS                       (0x82)

/* Zoning SMPs */
#define SMP_FUNCTION_CONFIGURE_ZONE_PHY                         (0x8a)
#define SMP_FUNCTION_CONFIGURE_ZONE_PERMISSION                  (0x8b)
#define SMP_FUNCTION_REPORT_ZONE_PERMISSION                     (0x04)
#define SMP_FUNCTION_ENABLE_DISABLE_ZONING                      (0x81)
#define SMP_FUNCTION_ZONE_ACTIVATE                              (0x87)
#define SMP_FUNCTION_ZONE_LOCK                                  (0x86)
#define SMP_FUNCTION_ZONE_UNLOCK                                (0x88)
#define SMP_FUNCTION_CONFIGURE_ZONE_MANAGER_PASSWORD            (0x89)

/*
 * SMP Response
 */
typedef struct _SMP_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U8                      AdditionalResponseBytes[1];         /* 0x04 */
    /* Note AdditionalResponseBytes is of variable length based on function */
} SMP_RESPONSE, *PTR_SMP_RESPONSE;

/* SMP Function Result field */
#define SMP_RESPONSE_FUNCTION_RESULT_ACCEPTED                   (0x00)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_SMP_FUNCTION       (0x01)
#define SMP_RESPONSE_FUNCTION_RESULT_SMP_FUNCTION_FAILED        (0x02)
#define SMP_RESPONSE_FUNCTION_RESULT_INVALID_REQUEST_LENGTH     (0x03)
#define SMP_RESPONSE_FUNCTION_RESULT_INVALID_EXP_CHANGE_COUNT   (0x04)
#define SMP_RESPONSE_FUNCTION_RESULT_BUSY                       (0x05)
#define SMP_RESPONSE_FUNCTION_RESULT_INCOMPLETE_DESCRIPTOR_LIST (0x06)

#define SMP_RESPONSE_FUNCTION_RESULT_PHY_DOES_NOT_EXIST         (0x10)
#define SMP_RESPONSE_FUNCTION_RESULT_INDEX_DOES_NOT_EXIST       (0x11)
#define SMP_RESPONSE_FUNCTION_RESULT_PHY_DOES_NOT_SUPPORT_SATA  (0x12)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_PHY_OPERATION      (0x13)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_PHY_TEST_FUNCTION  (0x14)
#define SMP_RESPONSE_FUNCTION_RESULT_PHY_TEST_FUNCTION_IN_PROG  (0x15)
#define SMP_RESPONSE_FUNCTION_RESULT_PHY_VACANT                 (0x16)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_PHY_EVENT_SOURCE   (0x17)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_DESCRIPTOR_TYPE    (0x18)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_PHY_FILTER         (0x19)
#define SMP_RESPONSE_FUNCTION_RESULT_AFFILIATION_VIOLATION      (0x1a)

#define SMP_RESPONSE_FUNCTION_RESULT_SMP_ZONE_VIOLATION                     (0x20)
#define SMP_RESPONSE_FUNCTION_RESULT_NO_MANAGEMENT_ACCESS_RIGHTS            (0x21)
#define SMP_RESPONSE_FUNCTION_RESULT_UNKNOWN_ENABLE_DISABLE_ZONING_VALUE    (0x22)
#define SMP_RESPONSE_FUNCTION_RESULT_ZONE_LOCK_VIOLATION                    (0x23)
#define SMP_RESPONSE_FUNCTION_RESULT_NOT_ACTIVATED                          (0x24)
#define SMP_RESPONSE_FUNCTION_RESULT_ZONE_GROUP_OUT_OF_RANGE                (0x25)
#define SMP_RESPONSE_FUNCTION_RESULT_NO_PHYSICAL_PRESENCE                   (0x26)
#define SMP_RESPONSE_FUNCTION_RESULT_SAVING_NOT_SUPPORTED                   (0x27)
#define SMP_RESPONSE_FUNCTION_RESULT_SOURCE_ZONE_GROUP_DOES_NOT_EXIST       (0x28)
#define SMP_RESPONSE_FUNCTION_RESULT_DISABLED_PASSWORD_NOT_SUPPORTED        (0x29)


/*
 * SMP Report General request
 */
typedef struct _SMP_REPORT_GENERAL_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
} SMP_REPORT_GENERAL_REQUEST, *PTR_SMP_REPORT_GENERAL_REQUEST;

/*
 * SMP Report General response
 */
typedef struct _SMP_REPORT_GENERAL_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     ExpanderRouteIndexes;               /* 0x06 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      LongResponse                    :1;
    U8                      Reserved08                      :7; /* 0x08 */
#else
    U8                      Reserved08                      :7;
    U8                      LongResponse                    :1; /* 0x08 */
#endif
    U8                      NumberOfPhys;                       /* 0x09 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      TableToTableSup                 :1; /* 0x0A */
    U8                      ZoneConfiguring                 :1;
    U8                      SelfConfiguring                 :1;
    U8                      StpContinueAWT                  :1;
    U8                      OpenRejectRetrySupported        :1;
    U8                      ConfiguresOthers                :1;
    U8                      Configuring                     :1;
    U8                      ConfigurableRouteTable          :1;
#else              /* little endian */
    U8                      ConfigurableRouteTable          :1; /* 0x0A */
    U8                      Configuring                     :1;
    U8                      ConfiguresOthers                :1;
    U8                      OpenRejectRetrySupported        :1;
    U8                      StpContinueAWT                  :1;
    U8                      SelfConfiguring                 :1;
    U8                      ZoneConfiguring                 :1;
    U8                      TableToTableSup                 :1;
#endif
    U8                      Reserved0B;                         /* 0x0B */
    SAS_ADDRESS             EnclosureLogicalIdentifier;         /* 0x0C */
    U8                      Reserved14to1B[8];                  /* 0x14 */
    U16                     Reserved1C;                         /* 0x1c */
    U16                     StpBusInactivityTimeLimit;          /* 0x1e */
    U16                     StpMaxConnectTimeLimit;             /* 0x20 */
    U16                     StpNexusLossTime;                   /* 0x22 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      NoOfZoneGroups                  :2; /* 0x24 */
    U8                      Reserved24                      :1;
    U8                      ZoneLocked                      :1;
    U8                      PhysPresenceSupported           :1;
    U8                      PhysPresenceAsserted            :1;
    U8                      ZoningSupported                 :1;
    U8                      ZoningEnabled                   :1;
#else              /* little endian */
    U8                      ZoningEnabled                   :1; /* 0x24 */
    U8                      ZoningSupported                 :1;
    U8                      PhysPresenceAsserted            :1;
    U8                      PhysPresenceSupported           :1;
    U8                      ZoneLocked                      :1;
    U8                      Reserved24                      :1;
    U8                      NoOfZoneGroups                  :2;
#endif
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved25                      :3; /* 0x25 */
    U8                      Saving                          :1;
    U8                      SavingZoneMgrPasswdSupported    :1;
    U8                      SavingZonePhyInfoSupported      :1;
    U8                      SavingZonePermTableSupported    :1;
    U8                      SavingZoningEnabledSupported    :1;
#else              /* little endian */
    U8                      SavingZoningEnabledSupported    :1; /* 0x25 */
    U8                      SavingZonePermTableSupported    :1;
    U8                      SavingZonePhyInfoSupported      :1;
    U8                      SavingZoneMgrPasswdSupported    :1;
    U8                      Saving                          :1;
    U8                      Reserved25                      :3;
#endif
    U16                     MaxRoutedSasAddresses;              /* 0x26 */
    SAS_ADDRESS             ActiveZoneManager;                  /* 0x28 */
    U16                     ZoneLockInactiveTime;               /* 0x30 */
    U16                     Reserved32;                         /* 0x32 */
    U8                      Reserved34;                         /* 0x34 */
    U8                      FirstEnclosureConnectorIndex;       /* 0x35 */
    U8                      EnclosureConnectorIndexes;          /* 0x36 */
    U8                      Reserved37;                         /* 0x37 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      ReducedFunctionality            :1; /* 0x38 */
    U8                      Reserved38                      :7;
#else              /* little endian */
    U8                      Reserved38                      :7; /* 0x38 */
    U8                      ReducedFunctionality            :1;
#endif
    U8                      TimeToReducedFunc;                  /* 0x39 */
    U8                      InitTimeReducedFunc;                /* 0x3a */
    U8                      MaxReducedFuncTime;                 /* 0x3b */

    U16                     LastSelfConfigStatus;               /* 0x3c - 0x3d */
    U16                     MaxStoredSelfConfigStatus;          /* 0x3e - 0x3f */
    U16                     LastPhyEventDesc;                   /* 0x40 - 0x41 */
    U16                     MaxStoredPhyEventDesc;              /* 0x43 - 0x42 */
    U16                     STPRejectLimit;                     /* 0x44 - 0x45 */
    U16                     Reserved47;                         /* 0x47 - 0x46 */
    U32                     CRC;
} SMP_REPORT_GENERAL_RESPONSE, *PTR_SMP_REPORT_GENERAL_RESPONSE;

/*
 * SMP Report Manufacturer Information request
 */
typedef struct _SMP_REPORT_MANUFACTURER_INFO_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
} SMP_REPORT_MANUFACTURER_INFO_REQUEST, *PTR_SMP_REPORT_MANUFACTURER_INFO_REQUEST;

/*
 * SMP Report Manufacturer Information response
 */
#define VEND_ID_LEN                                             (8)
#define PROD_ID_LEN                                             (16)

#define PROD_REV_LEN                                            (4)

#define VEND_SPEC_LEN                                           (8)
#define COMP_VEND_ID_LEN                                        (8)

typedef struct _SMP_REPORT_MANUFACTURER_INFO_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 - 0x05 */
    U16                     Reserved06;                         /* 0x06 - 0x07 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved0ABits1to7              :7; /* 0x08 */
    U8                      SAS11Format                     :1;
#else              /* little endian */
    U8                      SAS11Format                     :1; /* 0x08 */
    U8                      Reserved0ABits1to7              :7;
#endif
    U8                      Reserved09;                         /* 0x09 */
    U8                      Reserved0A;                         /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U8                      VendorIdentification[VEND_ID_LEN];  /* 0x0C */
    U8                      ProductIdentification[PROD_ID_LEN]; /* 0x14 */
    U8                      ProductRevisionLevel[PROD_REV_LEN]; /* 0x24 */
    U8                      ComponentVendorId[COMP_VEND_ID_LEN];/* 0x28 */
    U16                     ComponentId;                        /* 0x30 */
    U8                      ComponentRevisionId;                /* 0x32 */
    U8                      Reserved51;                         /* 0x33 */
    union
    {
        struct
        {
            U8              VuX12TruncatedEnclosureId[5];       /* 0x34 */
            U8              OEMVendorUnique57to50[3];           /* 0x39 */
        }Enclosure;
        U8                  VendorSpecific[VEND_SPEC_LEN];      /* 0x34 */
    }u;
    U32                     CRC;
} SMP_REPORT_MANUFACTURER_INFO_RESPONSE, *PTR_SMP_REPORT_MANUFACTURER_INFO_RESPONSE;


/*
 * SMP Report Self-Configuration request
 */
typedef struct _SMP_REPORT_SELF_CONFIG_STATUS_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     Reserved04to05;                     /* 0x04 - 0x05 */
    U16                     StartingStatusIndex;                /* 0x06 - 0x07 */
} SMP_REPORT_SELF_CONFIG_STATUS_REQUEST, *PTR_SMP_REPORT_SELF_CONFIG_STATUS_REQUEST;


/*
 * SMP Report Self-Configuration response descriptor
 */
typedef struct _SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR
{
    U8                      StatusType;                         /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved01Bits1to7              :7; /* 0x01 */
    U8                      Final                           :1;
#else              /* little endian */
    U8                      Final                           :1; /* 0x01 */
    U8                      Reserved01Bits1to7              :7;
#endif
    U8                      Reserved02;                         /* 0x02 */
    U8                      PhyIdentifier;                      /* 0x03 */
    U32                     Reserved04to07;                     /* 0x04 - 0x07 */
    SAS_ADDRESS             SASAddress;                         /* 0x08 - 0x0f */
} SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR, *PTR_SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR;


/*
 * SMP Report Self-Configuration response
 */
typedef struct _SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE
{
    U8                      SMPFrameType;                             /* 0x00 */
    U8                      Function;                                 /* 0x01 */
    U8                      FunctionResult;                           /* 0x02 */
    U8                      ResponseLength;                           /* 0x03 */
    U16                     ExpanderChangeCount;                      /* 0x04 - 0x05 */
    U16                     StartingStatusIndex;                      /* 0x06 - 0x07 */
    U16                     TotalNumStatusDescriptors;                /* 0x08 - 0x09 */
    U16                     LastStatusIndex;                          /* 0x0a - 0x0b */
    U8                      StatusDescriptorLength;                   /* 0x0c */
    U8                      Reserved0D;                               /* 0x0d */
    U16                     Reserved0Eto0F;                           /* 0x0e - 0x0f */
    U16                     Reserved10to11;                           /* 0x10 - 0x11 */
    U8                      Reserved12;                               /* 0x12 */
    U8                      NumberStatusDescriptors;                  /* 0x13 */
    SMP_REPORT_SELF_CONFIG_STATUS_DESCRIPTOR  StatusDescriptors[1];   /* 0x14... */
} SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE, *PTR_SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE;


/*
 * SMP Discover request
 */
typedef struct _SMP_DISCOVER_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U32                     Reserved04to07;                     /* 0x04 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved08                      :7; /* 0x08 */
    U8                      NoZoneMask                      :1;
#else              /* little endian */
    U8                      NoZoneMask                      :1; /* 0x08 */
    U8                      Reserved08                      :7;
#endif
    U8                      PhyIdentifier;                      /* 0x09 */
    U16                     Reserved0Ato0B;                     /* 0x0A */
} SMP_DISCOVER_REQUEST, *PTR_SMP_DISCOVER_REQUEST;

/*
 * SMP Discover response
 */


// If type = 20h - 23h then this format is valid
typedef struct _SMP_X12_VU_SLOT_DISCOVER_REPSONSE
{
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                     ConnectorFieldsValid            :2;
    U8                     ConnectorType                   :6;
#else              /* little endian */
    U8                     ConnectorType                   :6;
    U8                     ConnectorFieldsValid            :2;
#endif
    U8                     ConnectorElementIndex;
} SMP_X12_VU_SLOT_DISCOVER_REPSONSE, * PTR_SMP_X12_VU_SLOT_DISCOVER_REPSONSE;

// if type != 20h - 23h then this format is valid
typedef struct _SMP_X12_VU_DISCOVER_REPSONSE
{
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                     ConnectorFieldsValid            :2;
    U8                     ConnectorType                   :6;
    U8                     ConnectorElementIndex           :4;
    U8                     ConnectorPhysicalLink           :4;
#else              /* little endian */
    U8                     ConnectorType                   :6;
    U8                     ConnectorFieldsValid            :2;
    U8                     ConnectorPhysicalLink           :4;
    U8                     ConnectorElementIndex           :4;
#endif
} SMP_X12_VU_DISCOVER_REPSONSE, * PTR_SMP_X12_VU_DISCOVER_REPSONSE;

typedef struct _SMP_DISCOVER_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     Reserved06;                         /* 0x06 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved0A;                         /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved0CBit7                  :1; /* 0x0C */
    U8                      AttachedDeviceType              :3;
    U8                      AttachedReason                  :4;
    U8                      Reserved0DBits4to7              :4; /* 0x0D */
    U8                      NegotiatedPhysLinkRate          :4;
    U8                      Reserved0EBits4to7              :4; /* 0x0E */
    U8                      AttachedSSPInitiator            :1;
    U8                      AttachedSTPInitiator            :1;
    U8                      AttachedSMPInitiator            :1;
    U8                      AttachedSATAHost                :1;
    U8                      AttachedSATAPortSelector        :1; /* 0x0F */
    U8                      Reserved0FBits4to6              :3;
    U8                      AttachedSSPTarget               :1;
    U8                      AttachedSTPTarget               :1;
    U8                      AttachedSMPTarget               :1;
    U8                      AttachedSATATarget              :1;
#else              /* little endian */
    U8                      AttachedReason                  :4; /* 0x0C */
    U8                      AttachedDeviceType              :3;
    U8                      Reserved0CBit7                  :1;
    U8                      NegotiatedPhysLinkRate          :4; /* 0x0D */
    U8                      Reserved0DBits4to7              :4;
    U8                      AttachedSATAHost                :1; /* 0x0E */
    U8                      AttachedSMPInitiator            :1;
    U8                      AttachedSTPInitiator            :1;
    U8                      AttachedSSPInitiator            :1;
    U8                      Reserved0EBits4to7              :4;
    U8                      AttachedSATATarget              :1; /* 0x0F */
    U8                      AttachedSMPTarget               :1;
    U8                      AttachedSTPTarget               :1;
    U8                      AttachedSSPTarget               :1;
    U8                      Reserved0FBits4to6              :3;
    U8                      AttachedSATAPortSelector        :1;
#endif
    SAS_ADDRESS             SASAddress;                         /* 0x10 */
    SAS_ADDRESS             AttachedSASAddress;                 /* 0x18 */
    U8                      AttachedPhyIdentifier;              /* 0x20 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved21Bits3to7              :5; /* 0x21 */
    U8                      AttachedInsideZpsdsPersistent   :1;
    U8                      AttachedRequestedInsideZpsds    :1;
    U8                      AttachedBreakReplyCapable       :1;
#else              /* little endian */
    U8                      AttachedBreakReplyCapable       :1; /* 0x21 */
    U8                      AttachedRequestedInsideZpsds    :1;
    U8                      AttachedInsideZpsdsPersistent   :1;
    U8                      Reserved21Bits3to7              :5;
#endif
    U8                      Reserved22;                         /* 0x22 */
    U8                      Reserved23;                         /* 0x23 */
    U32                     Reserved24to27;                     /* 0x24 - 0x27 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      ProgMinPhysLinkRate             :4; /* 0x28 */
    U8                      HardwareMinPhysLinkRate         :4;
    U8                      ProgMaxPhysLinkRate             :4; /* 0x29 */
    U8                      HardwareMaxPhysLinkRate         :4;
    U8                      PhyChangeCount;                     /* 0x2A */
    U8                      VirtualPhy                      :1; /* 0x2B */
    U8                      Reserved2BBits4to6              :3;
    U8                      PartialPathTimeoutValue         :4;
    U8                      Reserved2CBits4to7              :4; /* 0x2C */
    U8                      RoutingAttribute                :4;
    U8                      Reserved2DBit7                  :1; /* 0x2D */
    U8                      ConnectorType                   :7;
#else              /* little endian */
    U8                      HardwareMinPhysLinkRate         :4; /* 0x28 */
    U8                      ProgMinPhysLinkRate             :4;
    U8                      HardwareMaxPhysLinkRate         :4; /* 0x29 */
    U8                      ProgMaxPhysLinkRate             :4;
    U8                      PhyChangeCount;                     /* 0x2A */
    U8                      PartialPathTimeoutValue         :4; /* 0x2B */
    U8                      Reserved2BBits4to6              :3;
    U8                      VirtualPhy                      :1;
    U8                      RoutingAttribute                :4; /* 0x2C */
    U8                      Reserved2CBits4to7              :4;
    U8                      ConnectorType                   :7; /* 0x2D */
    U8                      Reserved2DBit7                  :1;
#endif
    U8                      ConnectorElementIndex;              /* 0x2E */
    U8                      ConnectorPhysicalLink;              /* 0x2F */
    U8                      Reserved30;                         /* 0x30 */
    U8                      Reserved31;                         /* 0x31 */
    union
    {
        SMP_X12_VU_DISCOVER_REPSONSE      VuX12Connector;       /* 0x32 */
        SMP_X12_VU_SLOT_DISCOVER_REPSONSE VuX12Slot;
        U16                               VendorSpecific;       /* 0x32 */
    } u;
    SAS_ADDRESS             AttachedDeviceName;                 /* 0x34 - 3B*/
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved3cBit7                  :1; /* 0x3c */
    U8                      ReqInsideZpsdsByExp             :1;
    U8                      InsideZpsdsPersistent           :1;
    U8                      RequestedInsideZpsds            :1;
    U8                      Reserved3cBit3                  :1;
    U8                      ZoneGroupPersistent             :1;
    U8                      InsideZpsds                     :1;
    U8                      ZoningEnabled                   :1;
#else              /* little endian */
    U8                      ZoningEnabled                   :1; /* 0x3c */
    U8                      InsideZpsds                     :1;
    U8                      ZoneGroupPersistent             :1;
    U8                      Reserved3cBit3                  :1;
    U8                      RequestedInsideZpsds            :1;
    U8                      InsideZpsdsPersistent           :1;
    U8                      ReqInsideZpsdsByExp             :1;
    U8                      Reserved3cBit7                  :1;
#endif
    U16                     Reserved3d;                         /* 0x3d */
    U8                      ZoneGroup;                          /* 0x3f */
    U8                      SelfConfigurationStatus;            /* 0x40 */
    U8                      SelfConfigLevelsCompleted;          /* 0x41 */
    U16                     Reserved42;                         /* 0x42 */
    SAS_ADDRESS             SelfConfigSasAddress;               /* 0x44 */
    U32                     ProgrammedPhyCapabilities;          /* 0x4C - 0x4F */
    U32                     CurrentPhyCapabilities;             /* 0x50 - 0x53 */
    U32                     AttachedPhyCapabilities;            /* 0x54 - 0x57 */
    U32                     Reserved58;                         /* 0x58 - 0x5B */
    U16                     Reserved5C;                         /* 0x5C - 0x5D */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reason                          :4; /* 0x5E */
    U8                      NegotiatedPhysicalLinkRate      :4;

    U8                      Reserved5FBits2to7              :6; /* 0x5F */
    U8                      NegotiatedSSC                   :1;
    U8                      HardwareMuxingSupported         :1;

    U8                      Reserved60Bits6to7              :2; /* 0x60 Bits 6 - 7 */
    U8                      DefaultInsideZpsdsPersistent    :1; /* 0x60 Bit 5 */
    U8                      DefaultRequestedInsideZpsds     :1; /* 0x60 Bit 4 */
    U8                      Reserved60Bit3                  :1; /* 0x60 Bit 3 */
    U8                      DefaultZoneGroupPersistent      :1; /* 0x60 Bit 2 */
    U8                      Reserved60Bit1                  :1; /* 0x60 Bit 1 */
    U8                      DefaultZoningEnabled            :1; /* 0x60 Bit 0 */
#else              /* little endian */
    U8                      NegotiatedPhysicalLinkRate      :4; /* 0x5E */
    U8                      Reason                          :4;

    U8                      HardwareMuxingSupported         :1; /* 0x5F */
    U8                      NegotiatedSSC                   :1;
    U8                      Reserved5FBits2to7              :6;

    U8                      DefaultZoningEnabled            :1; /* 0x60 Bit 0 */
    U8                      Reserved60Bit1                  :1; /* 0x60 Bit 1 */
    U8                      DefaultZoneGroupPersistent      :1; /* 0x60 Bit 2 */
    U8                      Reserved60Bit3                  :1; /* 0x60 Bit 3 */
    U8                      DefaultRequestedInsideZpsds     :1; /* 0x60 Bit 4 */
    U8                      DefaultInsideZpsdsPersistent    :1; /* 0x60 Bit 5 */
    U8                      Reserved60Bits6to7              :2; /* 0x60 Bits 6 - 7 */
#endif
    U8                      Reserved61;                         /* 0x61 */
    U8                      Reserved62;                         /* 0x62 */
    U8                      DefaultZoneGroup;                   /* 0x63 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved64Bits6to7              :2; /* 0x64 Bits 6 - 7 */
    U8                      SavedInsideZpsdsPersistent      :1; /* 0x64 Bit 5 */
    U8                      SavedRequestedInsideZpsds       :1; /* 0x64 Bit 4 */
    U8                      Reserved64Bit3                  :1; /* 0x64 Bit 3 */
    U8                      SavedZoneGroupPersistent        :1; /* 0x64 Bit 2 */
    U8                      Reserved64Bit1                  :1; /* 0x64 Bit 1 */
    U8                      SavedZoningEnabled              :1; /* 0x64 Bit 0 */
#else              /* little endian */
    U8                      SavedZoningEnabled              :1; /* 0x64 Bit 0 */
    U8                      Reserved64Bit1                  :1; /* 0x64 Bit 1 */
    U8                      SavedZoneGroupPersistent        :1; /* 0x64 Bit 2 */
    U8                      Reserved64Bit3                  :1; /* 0x64 Bit 3 */
    U8                      SavedRequestedInsideZpsds       :1; /* 0x64 Bit 4 */
    U8                      SavedInsideZpsdsPersistent      :1; /* 0x64 Bit 5 */
    U8                      Reserved64Bits6to7              :2; /* 0x64 Bits 6 - 7 */
#endif
    U8                      Reserved65;                         /* 0x65 */
    U8                      Reserved66;                         /* 0x66 */
    U8                      SavedZoneGroup;                     /* 0x67 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved68Bits6to7              :2; /* 0x68 Bits 6 - 7 */
    U8                      ShadowInsideZpsdsPersistent     :1; /* 0x68 Bit 5 */
    U8                      ShadowRequestedInsideZpsds      :1; /* 0x68 Bit 4 */
    U8                      Reserved68Bit3                  :1; /* 0x68 Bit 3 */
    U8                      ShadowZoneGroupPersistent       :1; /* 0x68 Bit 2 */
    U8                      Reserved68Bit0to1               :2; /* 0x68 Bits 0 -1 */
#else              /* little endian */
    U8                      Reserved68Bit0to1               :2; /* 0x68 Bits 0 -1 */
    U8                      ShadowZoneGroupPersistent       :1; /* 0x68 Bit 2 */
    U8                      Reserved68Bit3                  :1; /* 0x68 Bit 3 */
    U8                      ShadowRequestedInsideZpsds      :1; /* 0x68 Bit 4 */
    U8                      ShadowInsideZpsdsPersistent     :1; /* 0x68 Bit 5 */
    U8                      Reserved68Bits6to7              :2; /* 0x68 Bits 6 - 7 */
#endif
    U8                      Reserved69;                         /* 0x69 */
    U8                      Reserved6A;                         /* 0x6A */
    U8                      ShadowZoneGroup;                    /* 0x6B */
    U8                      DeviceSlotNumber;                   /* 0x6C */
    U8                      GroupNumber;                        /* 0x6D */
    U16                     PathToEnclosure[3];                 /* 0x6E */
    U32                     CRC;
} SMP_DISCOVER_RESPONSE, *PTR_SMP_DISCOVER_RESPONSE;

/* Discover attached device types */
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_NO_DEVICES_ATTACHED   (0x0)
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_END_DEVICE_ONLY       (0x1)
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_EDGE_EXPANDER         (0x2)
#define SMP_DISCOVER_ATTACHED_DEVICE_TYPE_FANOUT_EXPANDER       (0x3)

/* Discover negotiated physical link rate */
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_UNKNOWN_RATE         (0x0)
#define SMP_NEG_PHYS_LINK_RATE_PHY_DISABLED                     (0x1)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_SPEED_NEG_FAILED     (0x2)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_IN_SATA_SPINUP_HOLD  (0x3)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_PORT_SELECTOR        (0x4)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_RESET_IN_PROGRESS    (0x5)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_AT_1_5_GBPS          (0x8)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_AT_3_0_GBPS          (0x9)
#define SMP_NEG_PHYS_LINK_RATE_PHY_ENABLED_AT_6_0_GBPS          (0xA)

/* Discover hardware and programmed physical link rates */
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_NOT_PROGRAMMABLE        (0x0)
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_1_5_GBPS                (0x8)
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_3_0_GBPS                (0x9)
#define SMP_DISCOVER_PHYSICAL_LINK_RATE_6_0_GBPS                (0xA)

/* Discover routing attributes */
#define SMP_DISCOVER_ROUTING_ATTRIBUTE_DIRECT_ROUTING           (0x0)
#define SMP_DISCOVER_ROUTING_ATTRIBUTE_SUBTRACTIVE_ROUTING      (0x1)
#define SMP_DISCOVER_ROUTING_ATTRIBUTE_TABLE_ROUTING            (0x2)

#define SMP_DISCOVER_CONNECTOR_FIELDS_VALID                     (0x2)
/* Valid Connector Types*/
#define SMP_DISCOVER_CONNECTOR_TYPE_UNKNOWN                     (0X00)
#define SMP_DISCOVER_CONNECTOR_TYPE_SAS_EXTERNAL                (0X01)
#define SMP_DISCOVER_CONNECTOR_TYPE_SAS_INTERNAL                (0X10)
#define SMP_DISCOVER_CONNECTOR_TYPE_SAS_BACKPLANE_RECEPTACLE    (0X20)
#define SMP_DISCOVER_CONNECTOR_TYPE_SATA_HOST_PLUG              (0X21)
#define SMP_DISCOVER_CONNECTOR_TYPE_SAS_PLUG                    (0X22)
#define SMP_DISCOVER_CONNECTOR_TYPE_SATA_DEVICE_PLUG            (0X23)


/*
 * SMP Report Phy Error Log request
 */
typedef struct _SMP_REPORT_PHY_ERROR_LOG_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U32                     Reserved04to07;                     /* 0x04 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U16                     Reserved0Ato0B;                     /* 0x0A */
} SMP_REPORT_PHY_ERROR_LOG_REQUEST, *PTR_SMP_REPORT_PHY_ERROR_LOG_REQUEST;

/*
 * SMP Report Phy Error Log response
 */
typedef struct _SMP_REPORT_PHY_ERROR_LOG_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     Reserved06;                         /* 0x06 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved0A;                         /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U32                     InvalidDwordCount;                  /* 0x0C */
    U32                     RunningDisparityErrorCount;         /* 0x10 */
    U32                     LossOfDwordSynchronizationCount;    /* 0x14 */
    U32                     PhyResetProblemCount;               /* 0x18 */
    U32                     CRC;
} SMP_REPORT_PHY_ERROR_LOG_RESPONSE, *PTR_SMP_REPORT_PHY_ERROR_LOG_RESPONSE;

/*
 * SMP Report Phy SATA request
 */
typedef struct _SMP_REPORT_PHY_SATA_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U32                     Reserved04to07;                     /* 0x04 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      AffiliationContext;                 /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
} SMP_REPORT_PHY_SATA_REQUEST, *PTR_SMP_REPORT_PHY_SATA_REQUEST;

/*
 * SMP Report Phy SATA response
 */
typedef struct _SMP_REPORT_PHY_SATA_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     Reserved06;                         /* 0x06 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved0A;                         /* 0x0A */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                          Reserved0BBits3to7          :5; /* 0x0B */
    U8                          StpITNexusLossOccurred      :1;
    U8                          AffiliationsSupported       :1;
    U8                          AffiliationValid            :1;
#else              /* little endian */
    U8                          AffiliationValid            :1; /* 0x0B */
    U8                          AffiliationsSupported       :1;
    U8                          StpITNexusLossOccurred      :1;
    U8                          Reserved0BBits3to7          :5;
#endif
    U32                         Reserved0Cto0F;                     /* 0x0C */
    SAS_ADDRESS                 STPSASAddress;                      /* 0x10 */
    REGISTER_DEVICE_TO_HOST_FIS RegisterDeviceToHostFIS;            /* 0x18 */ /* Defined in ata.h */
    U32                         Reserved2Cto2F;                     /* 0x2C */
    SAS_ADDRESS                 AffiliatedSTPInitiatorSASAddress;   /* 0x30 */
    SAS_ADDRESS                 StpITNexusLossSASAddress;           /* 0x38 - 0x3F */
    U8                          Reserved46;                         /* 0x40 */
    U8                          AffiliationContext;                 /* 0x41 */
    U8                          CurrentAffiliationContexts;         /* 0x42 */
    U8                          MaximumAffiliationContexts;         /* 0x43 */
    U32                         CRC;
} SMP_REPORT_PHY_SATA_RESPONSE, *PTR_SMP_REPORT_PHY_SATA_RESPONSE;

/*
 * SMP Report Route Information request
 */
typedef struct _SMP_REPORT_ROUTE_INFO_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     Reserved04to05;                     /* 0x04 */
    U16                     ExpanderRouteIndex;                 /* 0x06 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U16                     Reserved0Ato0B;                     /* 0x0A */
} SMP_REPORT_ROUTE_INFO_REQUEST, *PTR_SMP_REPORT_ROUTE_INFO_REQUEST;

/*
 * SMP Report Route Information response
 */
typedef struct _SMP_REPORT_ROUTE_INFO_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     ExpanderRouteIndex;                 /* 0x06 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Reserved0A;                         /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      RouteEntryDisabled              :1; /* 0x0C */
    U8                      Reserved0CBits0to6              :7;
#else              /* little endian */
    U8                      Reserved0CBits0to6              :7; /* 0x0C */
    U8                      RouteEntryDisabled              :1;
#endif
    U8                      Reserved0Dto0F[3];                  /* 0x0D */
    SAS_ADDRESS             RoutedSASAddress;                   /* 0x10 */
    U8                      Reserved18to23[12];                 /* 0x18 */
    U32                     Reserved24to27;                     /* 0x24 */
    U32                     CRC;
} SMP_REPORT_ROUTE_INFO_RESPONSE, *PTR_SMP_REPORT_ROUTE_INFO_RESPONSE;

/*
 * SMP Configure Route Information request
 */
typedef struct _SMP_CONFIGURE_ROUTE_INFO_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     ExpanderRouteIndex;                 /* 0x06 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U16                     Reserved0Aand0B;                    /* 0x0A */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      DisableRouteEntry               :1; /* 0x0C */
    U8                      Reserved0CBits0to6              :7;
#else              /* little endian */
    U8                      Reserved0CBits0to6              :7; /* 0x0C */
    U8                      DisableRouteEntry               :1;
#endif
    U8                      Reserved0Dto0F[3];                  /* 0x0D */
    SAS_ADDRESS             RoutedSASAddress;                   /* 0x10 */
    U8                      Reserved18to23[12];                 /* 0x18 */
    U32                     Reserved24to27;                     /* 0x24 */
} SMP_CONFIGURE_ROUTE_INFO_REQUEST, *PTR_SMP_CONFIGURE_ROUTE_INFO_REQUEST;

/*
 * SMP Configure Route Information response
 */

typedef struct _SMP_CONFIGURE_ROUTE_INFO_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     CRC;
} SMP_CONFIGURE_ROUTE_INFO_RESPONSE, *PTR_SMP_CONFIGURE_ROUTE_INFO_RESPONSE;

/*
 * SMP Report Expander Route Table request
 */
typedef struct _SMP_REPORT_EXP_ROUTE_TABLE_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U32                     Reserved04to07;                     /* 0x04 */
    U16                     MaxNumDescriptors;                  /* 0x08 */
    U16                     StartingRoutedSASAddressIdx;        /* 0x0A */
    U32                     Reserved0Cto0F;                     /* 0x0C */
    U16                     Reserved10and11;                    /* 0x10 */
    U8                      Reserved12;                         /* 0x12 */
    U8                      StartingPhyId;                      /* 0x13 */
    U32                     Reserved14to17;                     /* 0x14 */
    U32                     Reserved18to1B;                     /* 0x18 */
} SMP_REPORT_EXP_ROUTE_TABLE_REQUEST, *PTR_SMP_REPORT_EXP_ROUTE_TABLE_REQUEST;

#define SMP_REPORT_EXP_ROUTE_TABLE_REQUEST_LENGTH               (0x06)

typedef struct _SMP_REPORT_EXP_ROUTE_TABLE_DESCRIPTOR
{
    SAS_ADDRESS             RoutedSASAddress;                   /* 0x00 */
    U32                     PhyBitMapBit16to47;                 /* 0x08 */
    U16                     PhyBitMapBit0to15;                  /* 0x0C */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      ZoneGroupValid                  :1; /* 0x0E */
    U8                      Reserved0EBit0to6               :7;
#else
    U8                      Reserved0EBit0to6               :7; /* 0x0E */
    U8                      ZoneGroupValid                  :1;
#endif
    U8                      ZoneGroup;                          /* 0x0F */
} SMP_REPORT_EXP_ROUTE_TABLE_DESCRIPTOR, *PTR_SMP_REPORT_EXP_ROUTE_TABLE_DESCRIPTOR;

/*
 * SMP Report Expander Route Table response
 */
typedef struct _SMP_REPORT_EXP_ROUTE_TABLE_RESPONSE
{
    U8                                      SMPFrameType;           /* 0x00 */
    U8                                      Function;               /* 0x01 */
    U8                                      FunctionResult;         /* 0x02 */
    U8                                      ResponseLength;         /* 0x03 */
    U16                                     ExpanderChangeCount;    /* 0x04 */
    U16                                     ExpRouteTableChangeCnt; /* 0x06 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                                      Reserved08Bit4to7   :4; /* 0x08 */
    U8                                      SelfConfiguring     :1;
    U8                                      ZoneConfiguring     :1;
    U8                                      Configuring         :1;
    U8                                      Reserved08Bit0      :1;
#else
    U8                                      Reserved08Bit0      :1; /* 0x08 */
    U8                                      Configuring         :1;
    U8                                      ZoneConfiguring     :1;
    U8                                      SelfConfiguring     :1;
    U8                                      Reserved08Bit4to7   :4;
#endif
    U8                                      Reserved09;             /* 0x09 */
    U8                                      ExpanderRouteTableDescriptorLength;     /* 0x0A */
    U8                                      NumDescriptors;         /* 0x0B */
    U16                                     FirstRoutedSASAddrIdx;  /* 0x0C */
    U16                                     LastRoutedSASAddrIdx;   /* 0x0E */
    U16                                     Reserved10and11;        /* 0x10 */
    U8                                      Reserved12;             /* 0x12 */
    U8                                      StartingPhyIdentifier;  /* 0x13 */
    U32                                     Reserved20to23;         /* 0x14 */
    U32                                     Reserved24to27;         /* 0x18 */
    U32                                     Reserved28to31;         /* 0x1c */
    SMP_REPORT_EXP_ROUTE_TABLE_DESCRIPTOR   ExpRouteTableDesc[1];   /* 0x20 */
}SMP_REPORT_EXP_ROUTE_TABLE_RESPONSE, *PTR_SMP_REPORT_EXP_ROUTE_TABLE_RESPONSE;

/* This define was calculated by taking the maximum response size of 1024
 * subtracting the non-descriptor response size of 32 bytes and dividing
 * by the descriptor size
 */
#define MAX_REPORT_EXP_ROUTE_TABLE_DESCRIPTOR   (1024 - 32)/sizeof(SMP_REPORT_EXP_ROUTE_TABLE_DESCRIPTOR)

/*
 * SMP Phy Control request
 */
typedef struct _SMP_PHY_CONTROL_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     Reserved06to07;                     /* 0x06 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      PhyOperation;                       /* 0x0A */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved0BBits1to7              :7; /* 0x0B */
    U8                      UpdatePartialPathTimeoutValue   :1;
#else              /* little endian */
    U8                      UpdatePartialPathTimeoutValue   :1; /* 0x0B */
    U8                      Reserved0BBits1to7              :7;
#endif

    U32                     Reserved0C;                         /* 0x0C - 0x0F */
    U32                     Reserved10;                         /* 0x10 - 0x13 */
    U32                     Reserved14;                         /* 0x14 - 0x17 */
    SAS_ADDRESS             AttachedDeviceName;                 /* 0x18 - 0x1F */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      ProgMinPhysLinkRate             :4; /* 0x20 */
    U8                      Reserved20Bits0to3              :4;
    U8                      ProgMaxPhysLinkRate             :4; /* 0x21 */
    U8                      Reserved21Bits0to3              :4;
    U16                     Reserved22to23;                     /* 0x22 */
    U8                      Reserved24Bits4to7              :4; /* 0x24 */
    U8                      PartialPathTimeoutValue         :4;
#else              /* little endian */
    U8                      Reserved20Bits0to3              :4; /* 0x20 */
    U8                      ProgMinPhysLinkRate             :4;
    U8                      Reserved21Bits0to3              :4; /* 0x21 */
    U8                      ProgMaxPhysLinkRate             :4;
    U16                     Reserved22to23;                     /* 0x22 */
    U8                      PartialPathTimeoutValue         :4; /* 0x24 */
    U8                      Reserved24Bits4to7              :4;
#endif
    U8                      Reserved25;                         /* 0x25 */
    U16                     Reserved26and27;                    /* 0x26 */
} SMP_PHY_CONTROL_REQUEST, *PTR_SMP_PHY_CONTROL_REQUEST;

/* Phy Control Phy operation field */
#define SMP_PHY_CONTROL_PHY_OPERATION_NOP                       (0x00)
#define SMP_PHY_CONTROL_PHY_OPERATION_LINK_RESET                (0x01)
#define SMP_PHY_CONTROL_PHY_OPERATION_HARD_RESET                (0x02)
#define SMP_PHY_CONTROL_PHY_OPERATION_DISABLE                   (0x03)
#define SMP_PHY_CONTROL_PHY_OPERATION_CLEAR_ERROR_LOG           (0x05)
#define SMP_PHY_CONTROL_PHY_OPERATION_CLEAR_AFFILIATION         (0x06)
#define SMP_PHY_CONTROL_TRANSMIT_SATA_PORT_SEL_SIGNAL           (0x07)

/* Phy Control programmed physical link rates */
#define SMP_PHY_CONTROL_PHYSICAL_LINK_RATE_DONT_CHANGE          (0x0)
#define SMP_PHY_CONTROL_PHYSICAL_LINK_RATE_1_5_GBPS             (0x8)
#define SMP_PHY_CONTROL_PHYSICAL_LINK_RATE_3_0_GBPS             (0x9)

/*
 * SMP Phy Control response
 */

typedef struct _SMP_PHY_CONTROL_RESPONSE
{
    U8                      SMPFrameType;           /* 0x00 */
    U8                      Function;               /* 0x01 */
    U8                      FunctionResult;         /* 0x02 */
    U8                      ResponseLength;         /* 0x03 */
    U32                     CRC;
} SMP_PHY_CONTROL_RESPONSE, *PTR_SMP_PHY_CONTROL_RESPONSE;


/* set allowable vendor unique write size to maximum - SDE12 EDS shows
 * granularity as double dwords, but just treat as dwords here
 */
#define MAX_SMP_REGISTERS                                       (254)


/*
 * This section contains generic SGPIO R/W SMP commands
 */

typedef struct _SMP_READ_N_GPIO_REGISTERS_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      RegisterType;                       /* 0x02 */
    U8                      RegisterIndex;                      /* 0x03 */
    U8                      RegisterCount;                      /* 0x04 */
    U8                      Reserved05[3];                      /* 0x05 */
} SMP_READ_N_GPIO_REGISTERS_REQUEST, *PTR_SMP_READ_N_GPIO_REGISTERS_REQUEST;

typedef struct _SMP_READ_N_GPIO_REGISTERS_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     ReadData[1];                        /* 0x08-... */
    /* Note this field can actually be up to MAX_SMP_REGISTERS dwords long */
} SMP_READ_N_GPIO_REGISTERS_RESPONSE, *PTR_SMP_READ_N_GPIO_REGISTERS_RESPONSE;

typedef struct _SMP_WRITE_N_GPIO_REGISTERS_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      RegisterType;                       /* 0x02 */
    U8                      RegisterIndex;                      /* 0x03 */
    U8                      RegisterCount;                      /* 0x04 */
    U8                      Reserved05[3];                      /* 0x05 */
    U32                     WriteData[1];                       /* 0x08-... */
    /* Note this field can actually be up to MAX_SMP_REGISTERS dwords long */
} SMP_WRITE_N_GPIO_REGISTERS_REQUEST, *PTR_SMP_WRITE_N_GPIO_REGISTERS_REQUEST;

typedef struct _SMP_WRITE_N_REGISTERS_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     CRC;
} SMP_WRITE_N_GPIO_REGISTERS_RESPONSE, *PTR_SMP_WRITE_N_GPIO_REGISTERS_RESPONSE;

/* This section contains vendor unique SMP commands for the LSI expander */

#define LSI_AHB_WRITE_N_BYTE_WRITE      (0x08000000)
#define LSI_AHB_WRITE_N_ADDR_MASK       (0xf7ffffff)

typedef struct _SMP_LSI_AHB_WRITE_N_REGISTERS_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      N;                                  /* 0x03 */
    U32                     StartAddr;                          /* 0x04 - 0x07 */
    U32                     WriteData[1];                       /* 0x08-... */
} SMP_LSI_AHB_WRITE_N_REGISTERS_REQUEST, *PTR_SMP_LSI_AHB_WRITE_N_REGISTERS_REQUEST;

typedef struct _SMP_LSI_AHB_WRITE_N_REGISTERS_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     CRC;
} SMP_LSI_AHB_WRITE_N_REGISTERS_RESPONSE, *PTR_SMP_LSI_AHB_WRITE_N_REGISTERS_RESPONSE;

typedef struct _SMP_LSI_AHB_READ_N_REGISTERS_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      N;                                  /* 0x03 */
    U32                     StartAddr;                          /* 0x04 - 0x07 */
} SMP_LSI_AHB_READ_N_REGISTERS_REQUEST, *PTR_SMP_LSI_AHB_READ_N_REGISTERS_REQUEST;

typedef struct _SMP_LSI_AHB_READ_N_REGISTERS_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     ReadData[1];                        /* 0x04-... */
} SMP_LSI_AHB_READ_N_REGISTERS_RESPONSE, *PTR_SMP_LSI_AHB_READ_N_REGISTERS_RESPONSE;

typedef struct _SMP_LSIX12_WRITE_N_REGISTERS_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      N;                                  /* 0x03 */
    U16                     StartAdr;                           /* 0x04 */
    U8                      StartByteEnables;                   /* 0x06 */
    U8                      EndByteEnables;                     /* 0x07 */
    U32                     WriteData[2];                       /* 0x08-... */
    /* Note this field can actually be up to MAX_SMP_REGISTERS dwords long */
} SMP_LSIX12_WRITE_N_REGISTERS_REQUEST, *PTR_SMP_LSIX12_WRITE_N_REGISTERS_REQUEST;

typedef struct _SMP_LSIX12_WRITE_N_REGISTERS_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     CRC;
} SMP_LSIX12_WRITE_N_REGISTERS_RESPONSE, *PTR_SMP_LSIX12_WRITE_N_REGISTERS_RESPONSE;

typedef struct _SMP_LSIX12_READ_N_REGISTERS_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      N;                                  /* 0x03 */
    U16                     StartAdr;                           /* 0x04 */
    U16                     Reserved06and07;                    /* 0x06 */

} SMP_LSIX12_READ_N_REGISTERS_REQUEST, *PTR_SMP_LSIX12_READ_N_REGISTERS_REQUEST;

typedef struct _SMP_LSIX12_READ_N_REGISTERS_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     ReadData[1];                        /* 0x04-... */
    /* Note this field can actually be up to MAX_SMP_REGISTERS dwords long */
} SMP_LSIX12_READ_N_REGISTERS_RESPONSE, *PTR_SMP_LSIX12_READ_N_REGISTERS_RESPONSE;


#define SMP_SGPIO_REGISTER_TYPE_CFG         (0)
#define SMP_SGPIO_REGISTER_TYPE_RX          (1)
#define SMP_SGPIO_REGISTER_TYPE_RX_GP       (2)
#define SMP_SGPIO_REGISTER_TYPE_TX          (3)
#define SMP_SGPIO_REGISTER_TYPE_TX_GP       (4)

/*
 * SMP ICL Discover request
 */
typedef struct _SMP_ICL_DISCOVER_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U16                     Reserved02and03;                    /* 0x02 */
    U32                     Ignored04to07;                      /* 0x04 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Ignored0A;                          /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
} SMP_ICL_DISCOVER_REQUEST, *PTR_SMP_ICL_DISCOVER_REQUEST;

/*
 * SMP ICL Discover response
 */

typedef struct _SMP_ICL_DISCOVER_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     Ignored04to07;                      /* 0x04 */
    U8                      Reserved08;                         /* 0x08 */
    U8                      PhyIdentifier;                      /* 0x09 */
    U8                      Ignored0A;                          /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved0CBit7                  :1; /* 0x0C */
    U8                      AttachedDeviceType              :3;
    U8                      Ignored0CBits0to3               :4;
    U8                      Reserved0DBits4to7              :4; /* 0x0D */
    U8                      NegotiatedPhysLinkRate          :4;
    U8                      Reserved0EBits4to7              :4; /* 0x0E */
    U8                      AttachedSSPInitiator            :1;
    U8                      AttachedSTPInitiator            :1;
    U8                      AttachedSMPInitiator            :1;
    U8                      AttachedSATAHost                :1;
    U8                      Reserved0FBits4to7              :4; /* 0x0F */
    U8                      AttachedSSPTarget               :1;
    U8                      AttachedSTPTarget               :1;
    U8                      AttachedSMPTarget               :1;
    U8                      AttachedSATATarget              :1;
    SAS_ADDRESS             SASAddress;                         /* 0x10 */
    SAS_ADDRESS             AttachedSASAddress;                 /* 0x18 */
    U8                      AttachedPhyIdentifier;              /* 0x20 */
    U8                      Reserved21to27[7];                  /* 0x21 */
    U8                      ProgMinPhysLinkRate             :4; /* 0x28 */
    U8                      HardwareMinPhysLinkRate         :4;
    U8                      ProgMaxPhysLinkRate             :4; /* 0x29 */
    U8                      HardwareMaxPhysLinkRate         :4;
    U8                      PhyChangeCount;                     /* 0x2A */
    U8                      VirtualPhy                      :1; /* 0x2B */
    U8                      Reserved2BBits4to6              :3;
    U8                      PartialPathTimeoutValue         :4;
    U8                      Reserved2CBits4to7              :4; /* 0x2C */
    U8                      RoutingAttribute                :4;
    U8                      Reserved2DBit7                  :1; /* 0x2D */
    U8                      ConnectorType                   :7;
#else              /* little endian */
    U8                      Ignored0CBits0to3               :4; /* 0x0C */
    U8                      AttachedDeviceType              :3;
    U8                      Reserved0CBit7                  :1;
    U8                      NegotiatedPhysLinkRate          :4; /* 0x0D */
    U8                      Reserved0DBits4to7              :4;
    U8                      AttachedSATAHost                :1; /* 0x0E */
    U8                      AttachedSMPInitiator            :1;
    U8                      AttachedSTPInitiator            :1;
    U8                      AttachedSSPInitiator            :1;
    U8                      Reserved0EBits4to7              :4;
    U8                      AttachedSATATarget              :1; /* 0x0F */
    U8                      AttachedSMPTarget               :1;
    U8                      AttachedSTPTarget               :1;
    U8                      AttachedSSPTarget               :1;
    U8                      Reserved0FBits4to7              :4;
    SAS_ADDRESS             SASAddress;                         /* 0x10 */
    SAS_ADDRESS             AttachedSASAddress;                 /* 0x18 */
    U8                      AttachedPhyIdentifier;              /* 0x20 */
    U8                      Reserved21to27[7];                  /* 0x21 */
    U8                      HardwareMinPhysLinkRate         :4; /* 0x28 */
    U8                      ProgMinPhysLinkRate             :4;
    U8                      HardwareMaxPhysLinkRate         :4; /* 0x29 */
    U8                      ProgMaxPhysLinkRate             :4;
    U8                      PhyChangeCount;                     /* 0x2A */
    U8                      PartialPathTimeoutValue         :4; /* 0x2B */
    U8                      Reserved2BBits4to6              :3;
    U8                      VirtualPhy                      :1;
    U8                      RoutingAttribute                :4; /* 0x2C */
    U8                      Reserved2CBits4to7              :4;
    U8                      ConnectorType                   :7; /* 0x2D */
    U8                      Reserved2DBit7                  :1;
#endif

    U8                      ConnectorElementIndex;
    U8                      ConnectorPhysicalLink;
    U16                     Reserved30to31;

    /* Vendor Unique */
    union
    {
        SMP_X12_VU_DISCOVER_REPSONSE      VuX12Connector;       /* 0x32 */
        SMP_X12_VU_SLOT_DISCOVER_REPSONSE VuX12Slot;
        U16                               VendorSpecific;       /* 0x32 */
    }u;
    U32                     CRC;

} SMP_ICL_DISCOVER_RESPONSE, *PTR_SMP_ICL_DISCOVER_RESPONSE;

/*
 * SMP Configure Zone Phy request
 */
typedef struct _SMP_CONFIGURE_ZONE_PHY_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      ZonePhyConfigDescriptorLength   :6; /* 0x06 */
    U8                      Save                            :2; /* 0x06 */
#else              /* little endian */
    U8                      Save                            :2; /* 0x06 */
    U8                      ZonePhyConfigDescriptorLength   :6; /* 0x06 */
#endif
    U8                      NumberOfZonePhyEntries;             /* 0x07 */
    /* note: zone phy entry data follows... */
} SMP_CONFIGURE_ZONE_PHY_REQUEST, *PTR_SMP_CONFIGURE_ZONE_PHY_REQUEST;

typedef struct _SMP_ZONE_PHY_CONFIG_DESCRIPTOR
{
    U8              PhyIdentifier;                             /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8              Reserved01a             :2;                /* 0x01 */
    U8              InsideZpsdsPersistent   :1;
    U8              ReqInsideZpsds          :1;
    U8              Reserved01b             :1;
    U8              ZoneGroupPersistent     :1;
    U8              Reserved01              :2;
#else              /* little endian */
    U8              Reserved01              :2;                /* 0x01 */
    U8              ZoneGroupPersistent     :1;
    U8              Reserved01b             :1;
    U8              ReqInsideZpsds          :1;
    U8              InsideZpsdsPersistent   :1;
    U8              Reserved01a             :2;
#endif
    U8              Reserved02;                                /* 0x02 */
    U8              ZoneGroup;                                 /* 0x03 */
} SMP_ZONE_PHY_CONFIG_DESCRIPTOR, *PTR_SMP_ZONE_PHY_CONFIG_DESCRIPTOR;

/*
 * SMP Configure Zone Phy response
 */

typedef struct _SMP_CONFIGURE_ZONE_PHY_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     CRC;
} SMP_CONFIGURE_ZONE_PHY_RESPONSE, *PTR_SMP_CONFIGURE_ZONE_PHY_RESPONSE;

/*
 * SMP Configure Zone Permission request
 */
typedef struct _SMP_CONFIGURE_ZONE_PERMISSION_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U8                      StartSrcZoneGroup;                  /* 0x06 */
    U8                      NumberOfZoneDescr;                  /* 0x07 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      NumberOfZoneGroups      :2;         /* 0x08 */
    U8                      Reserved07              :4;
    U8                      Save                    :2;
#else              /* little endian */
    U8                      Save                    :2;         /* 0x08 */
    U8                      Reserved07              :4;
    U8                      NumberOfZoneGroups      :2;
#endif
    U8                      ZonePermCfgDescLen;                 /* 0x09 */
    U16                     Reserved0Ato0B;                     /* 0x0A - 0x0B */
    U32                     Reserved0Cto0F;                     /* 0x0C - 0x0F */
    /* note: zone permission data follows... */
} SMP_CONFIGURE_ZONE_PERMISSION_REQUEST, *PTR_SMP_CONFIGURE_ZONE_PERMISSION_REQUEST;

typedef struct _SMP_ZONE_PERMISSION_DESCRIPTOR
{
    U8              PermBits[32];
} SMP_ZONE_PERMISSION_DESCRIPTOR, *PTR_SMP_ZONE_PERMISSION_DESCRIPTOR;

typedef struct _SMP_ZONE_PERMISSION_DESCRIPTOR_YETI
{
    U8              PermBits[16];
} SMP_ZONE_PERMISSION_DESCRIPTOR_YETI, *PTR_SMP_ZONE_PERMISSION_DESCRIPTOR_YETI;

/*
 * SMP Configure Zone Phy response
 */

typedef struct _SMP_CONFIGURE_ZONE_PERMISSION_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     CRC;
} SMP_CONFIGURE_ZONE_PERMISSION_RESPONSE, *PTR_SMP_CONFIGURE_ZONE_PERMISSION_RESPONSE;

/*
 * SMP Report Zone Permission request
 */
typedef struct _SMP_REPORT_ZONE_PERMISSION_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved04          :6;             /* 0x04 */
    U8                      ReportType          :2;
#else              /* little endian */
    U8                      ReportType          :2;             /* 0x04 */
    U8                      Reserved04          :6;
#endif
    U8                      Reserved05;                         /* 0x05 */
    U8                      StartSrcZoneGroup;                  /* 0x06 */
    U8                      MaxNumberOfZoneDescr;               /* 0x07 */
} SMP_REPORT_ZONE_PERMISSION_REQUEST, *PTR_SMP_REPORT_ZONE_PERMISSION_REQUEST;

/*
 * SMP Report Zone Permission Response
 */

typedef struct _SMP_REPORT_ZONE_PERMISSION_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      ZoneLocked          :1;             /* 0x06 */
    U8                      Reserved06          :5;
    U8                      ReportType          :2;

    U8                      NumberOfZoneGroups  :2;  /* 0x07 Bits 6 - 7 */
    U8                      Reserved07Bit0to5   :6;  /* 0x07 Bits 0 - 5 */

#else              /* little endian */
    U8                      ReportType          :2;             /* 0x06 */
    U8                      Reserved06          :5;
    U8                      ZoneLocked          :1;

    U8                      Reserved07Bit0to5   :6;  /* 0x07 Bits 0 - 5 */
    U8                      NumberOfZoneGroups  :2;  /* 0x07 Bits 6 - 7 */
#endif
    U32                     Reserved08;                         /* 0x08 */
    U8                      Reserved0c;                         /* 0x0c */
    U8                      ZonePermissionDescriptorLength;     /* 0x0d */
    U8                      StartSrcZoneGroup;                  /* 0x0e */
    U8                      NumberZonePermissionDescr;          /* 0x0f */
    /* zone permission descriptors start here ... */
} SMP_REPORT_ZONE_PERMISSION_RESPONSE, *PTR_SMP_REPORT_ZONE_PERMISSION_RESPONSE;

/*
 * SMP Enable / Disable Zoning request
 */
typedef struct _SMP_ENABLE_DISABLE_ZONING_REQUEST
{
    U8              SMPFrameType;                       /* 0x00 */
    U8              Function;                           /* 0x01 */
    U8              ResponseLength;                     /* 0x02 */
    U8              RequestLength;                      /* 0x03 */
    U16             ExpanderChangeCount;                /* 0x04 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8              Reserved06                  :6;     /* 0x06 */
    U8              Save                        :2;
#else              /* little endian */
    U8              Save                        :2;     /* 0x06 */
    U8              Reserved06                  :6;
#endif
    U8              Reserved07;                         /* 0x07 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8              Reserved08                  :6;     /* 0x08 */
    U8              EnableDisableZoning         :2;
#else              /* little endian */
    U8              EnableDisableZoning         :2;     /* 0x08 */
    U8              Reserved08                  :6;
#endif
    U8              Reserved09;                         /* 0x09 */
    U16             Reserved0a;                         /* 0x0a */
} SMP_ENABLE_DISABLE_ZONING_REQUEST, *PTR_SMP_ENABLE_DISABLE_ZONING_REQUEST;

#define SMP_ENABLE_NO_CHANGE    (0x00)
#define SMP_ENABLE_ENABLE   (0x01)
#define SMP_ENABLE_DISABLE  (0x02)
#define SMP_ENABLE_DISABLE_RESERVED (0x03)

/*
 * SMP Enable / Disable Zoning response
 */

typedef struct _SMP_ENABLE_DISABLE_ZONING_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     CRC;
} SMP_ENABLE_DISABLE_ZONING_RESPONSE, *PTR_SMP_ENABLE_DISABLE_ZONING_RESPONSE;

/*
 * SMP Zone Activate request
 */
typedef struct _SMP_ZONE_ACTIVATE_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     Reserved06;                         /* 0x06 */
} SMP_ZONE_ACTIVATE_REQUEST, *PTR_SMP_ZONE_ACTIVATE_REQUEST;

/*
 * SMP Zone Activate response
 */

typedef struct _SMP_ZONE_ACTIVATE_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     CRC;
} SMP_ZONE_ACTIVATE_RESPONSE, *PTR_SMP_ZONE_ACTIVATE_RESPONSE;


/*
 * SMP Configure Zone Manager Password request */

#define SMP_ZONE_PASSWORD_LEN               (32)

typedef struct _SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved06                  :6;     /* 0x06 */
    U8                      Save                        :2;
#else              /* little endian */
    U8                      Save                        :2;     /* 0x06 */
    U8                      Reserved06                  :6;
#endif
    U8                      Reserved07;                         /* 0x07 */
    U8                      ZoneManagerPassword[SMP_ZONE_PASSWORD_LEN];      /* 0x08 - 0x27 */
    U8                      NewZoneManagerPassword[SMP_ZONE_PASSWORD_LEN];   /* 0x28 - 0x47 */

} SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_REQUEST, *PTR_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_REQUEST;

/*
 * SMP Configure Zone Manager Password response */

typedef struct _SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     CRC;
} SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_RESPONSE, *PTR_SMP_CONFIGURE_ZONE_MANAGER_PASSWORD_RESPONSE;


/*
 * SMP Zone Lock request
 */

typedef struct _SMP_ZONE_LOCK_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     ExpanderChangeCount;                /* 0x04 */
    U16                     ZoneLockInactivityTimeLimit;        /* 0x06 */
    U8                      ZonePassword[SMP_ZONE_PASSWORD_LEN];/* 0x08 */
} SMP_ZONE_LOCK_REQUEST, *PTR_SMP_ZONE_LOCK_REQUEST;

/*
 * SMP Zone Lock response
 */
typedef struct _SMP_ZONE_LOCK_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     Reserved04;                         /* 0x04 */
    SAS_ADDRESS             ActiveZoneManager;                  /* 0x08 */
    U32                     CRC;
} SMP_ZONE_LOCK_RESPONSE, *PTR_SMP_ZONE_LOCK_RESPONSE;

/*
 * SMP Zone Unlock request
 */
typedef struct _SMP_ZONE_UNLOCK_REQUEST
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      ResponseLength;                     /* 0x02 */
    U8                      RequestLength;                      /* 0x03 */
    U16                     Reserved04;                         /* 0x04 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved06                  :7;     /* 0x06 */
    U8                      ActivateRequired            :1;
#else              /* little endian */
    U8                      ActivateRequired            :1;     /* 0x06 */
    U8                      Reserved06                  :7;
#endif
    U8                      Reserved07;                         /* 0x07 */
} SMP_ZONE_UNLOCK_REQUEST, *PTR_SMP_ZONE_UNLOCK_REQUEST;

/*
 * SMP Zone Unlock response
 */
typedef struct _SMP_ZONE_UNLOCK_RESPONSE
{
    U8                      SMPFrameType;                       /* 0x00 */
    U8                      Function;                           /* 0x01 */
    U8                      FunctionResult;                     /* 0x02 */
    U8                      ResponseLength;                     /* 0x03 */
    U32                     CRC;
} SMP_ZONE_UNLOCK_RESPONSE, *PTR_SMP_ZONE_UNLOCK_RESPONSE;

/* Union of all the SMP Request Frames */
typedef union _SMP_REQUEST_UNION
{
    SMP_REQUEST                             SMPRequestGeneric;
    SMP_REPORT_GENERAL_REQUEST              SMPReportGeneralRequest;
    SMP_REPORT_MANUFACTURER_INFO_REQUEST    SMPReportManufacturerInfoRequest;
    SMP_REPORT_SELF_CONFIG_STATUS_REQUEST   SMPReportSelfConfigStatusRequest;
    SMP_DISCOVER_REQUEST                    SMPDiscoverRequest;
    SMP_REPORT_PHY_ERROR_LOG_REQUEST        SMPReportPhyErrorLogRequest;
    SMP_REPORT_PHY_SATA_REQUEST             SMPReportPhySataRequest;
    SMP_REPORT_ROUTE_INFO_REQUEST           SMPReportRouteInfoRequest;
    SMP_CONFIGURE_ROUTE_INFO_REQUEST        SMPConfigureRouteInfoRequest;
    SMP_REPORT_EXP_ROUTE_TABLE_REQUEST      SMPReportExpRouteTableRequest;
    SMP_PHY_CONTROL_REQUEST                 SMPPhyControlRequest;
    /* These are generic SGPIO Register R/W */
    SMP_WRITE_N_GPIO_REGISTERS_REQUEST      SMPWriteNGpioRegistersRequest;
    SMP_READ_N_GPIO_REGISTERS_REQUEST       SMPReadNGpioRegistersRequest;
    /* These are vendor unique for Sasquatch expander */
    SMP_LSIX12_WRITE_N_REGISTERS_REQUEST    SMPLsiX12WriteNRegistersRequest;
    SMP_LSIX12_READ_N_REGISTERS_REQUEST     SMPLsiX12ReadNRegistersRequest;
    SMP_ICL_DISCOVER_REQUEST                SMPIclDiscoverRequest;
    SMP_LSI_AHB_WRITE_N_REGISTERS_REQUEST   SMPLsiAhbWriteNRegistersRequest;
    SMP_LSI_AHB_READ_N_REGISTERS_REQUEST    SMPLsiAhbReadNRegistersRequest;
    /* Zoning Requests */
    SMP_CONFIGURE_ZONE_PHY_REQUEST          SMPConfigureZonePhyRequest;
    SMP_CONFIGURE_ZONE_PERMISSION_REQUEST   SMPConfigureZonePermissionRequest;
    SMP_REPORT_ZONE_PERMISSION_REQUEST      SMPReportZonePermissionRequest;
    SMP_ENABLE_DISABLE_ZONING_REQUEST       SMPEnableDisableZoningRequest;
    SMP_ZONE_ACTIVATE_REQUEST               SMPZoneActivateRequest;
    SMP_ZONE_LOCK_REQUEST                   SMPZoneLockRequest;
    SMP_ZONE_UNLOCK_REQUEST                 SMPZoneUnlockRequest;
} SMP_REQUEST_UNION, * PTR_SMP_REQUEST_UNION;

/* Union of all the SMP Response Frames */
typedef union _SMP_RESPONSE_UNION
{
    SMP_RESPONSE                             SMPResponseGeneric;
    SMP_REPORT_GENERAL_RESPONSE              SMPReportGeneralResponse;
    SMP_REPORT_MANUFACTURER_INFO_RESPONSE    SMPReportManufacturerInfoResponse;
    SMP_REPORT_SELF_CONFIG_STATUS_RESPONSE   SMPReportSelfConfigStatusResponse;
    SMP_DISCOVER_RESPONSE                    SMPDiscoverResponse;
    SMP_REPORT_PHY_ERROR_LOG_RESPONSE        SMPReportPhyErrorLogResponse;
    SMP_REPORT_PHY_SATA_RESPONSE             SMPReportPhySataResponse;
    SMP_REPORT_ROUTE_INFO_RESPONSE           SMPReportRouteInfoResponse;
    SMP_CONFIGURE_ROUTE_INFO_RESPONSE        SMPConfigureRouteInfoResponse;
    SMP_REPORT_EXP_ROUTE_TABLE_RESPONSE      SMPReportExpRouteTableResponse;
    SMP_PHY_CONTROL_RESPONSE                 SMPPhyControlResponse;
    /* These are vendor unique for Sasquatch expander */
    SMP_WRITE_N_GPIO_REGISTERS_RESPONSE      SMPWriteNGpioRegistersResponse;
    SMP_READ_N_GPIO_REGISTERS_RESPONSE       SMPReadNGpioRegistersResponse;
    /* These are vendor unique for Sasquatch expander */
    SMP_LSIX12_WRITE_N_REGISTERS_RESPONSE    SMPLsiX12WriteNRegistersResponse;
    SMP_LSIX12_READ_N_REGISTERS_RESPONSE     SMPLsiX12ReadNRegistersResponse;
    SMP_ICL_DISCOVER_RESPONSE                SMPIclDiscoverResponse;
    SMP_LSI_AHB_WRITE_N_REGISTERS_RESPONSE   SMPLsiAhbWriteNRegistersResponse;
    SMP_LSI_AHB_READ_N_REGISTERS_RESPONSE    SMPLsiAhbReadNRegistersResponse;
    /* Zoning */
    SMP_CONFIGURE_ZONE_PHY_RESPONSE          SMPConfigureZonePhyResponse;
    SMP_CONFIGURE_ZONE_PERMISSION_RESPONSE   SMPConfigureZonePermissionResponse;
    SMP_REPORT_ZONE_PERMISSION_RESPONSE      SMPReportZonePermissionResponse;
    SMP_ENABLE_DISABLE_ZONING_RESPONSE       SMPEnableDisableZoningResponse;
    SMP_ZONE_ACTIVATE_RESPONSE               SMPZoneActivateResponse;
    SMP_ZONE_LOCK_RESPONSE                   SMPZoneLockResponse;
    SMP_ZONE_UNLOCK_RESPONSE                 SMPZoneUnlockResponse;
} SMP_RESPONSE_UNION, * PTR_SMP_RESPONSE_UNION;

#endif /* End of #ifndef SAS_H_SOURCE */

