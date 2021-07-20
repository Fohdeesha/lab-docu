/***************************************************************************
 *                                                                         *
 *  Copyright 2003 LSI Logic Corporation.  All rights reserved.            *
 *                                                                         *
 *  This file is confidential and a trade secret of LSI Logic.  The        *
 *  receipt of or possession of this file does not convey any rights to    *
 *  reproduce or disclose its contents or to manufacture, use, or sell     *
 *  anything it may describe, in whole, or in part, without the specific   *
 *  written consent of LSI Logic Corporation.                              *
 *                                                                         *
 ***************************************************************************
 *
 *           Name:  ATA.H
 *          Title:  ATA and SATA Device structures and definitions
 *     Programmer:  ----------------
 *  Creation Date:  February 26, 2002
 *
 *  Version History
 *  ---------------
 *
 *  Last Updated
 *  -------------
 *  Version         %version: 24 %
 *  Date Updated    %date_modified: %
 *  Programmer      %created_by: mfry %
 *
 *  Date      Who   Description
 *  --------  ---   -------------------------------------------------------
 *  02/26/02  GWK   Initial version
 *  01/24/03  GWK   Updated for SAS 1064
 *  08/20/08  KAM   updated for SDM big endian platform and gcc bitfields
 *
 *
 * Description
 * ------------
 * This include file contains generic ATA and SATA structures and definitions.
 *
 *-------------------------------------------------------------------------
 */

/* If this header file has not been included yet */
#ifndef ATA_H_SOURCE
#define ATA_H_SOURCE



/* Status Register Bit Definitions */
#define ATA_STATUS_ERR                          (0x01)
#define ATA_STATUS_DRQ                          (0x08)
#define ATA_STATUS_CMD_DEP                      (0x10)
#define ATA_STATUS_DF                           (0x20)
#define ATA_STATUS_DRDY                         (0x40)
#define ATA_STATUS_BSY                          (0x80)
#define ATA_STATUS_ERR_DF                       (ATA_STATUS_ERR | ATA_STATUS_DF)
/* Atapi Status Register Bit Definitions */
#define ATAPI_STATUS_CHK                        (0x01)
#define ATAPI_STATUS_DRQ                        (0x08)
#define ATAPI_STATUS_SERV                       (0x10)
#define ATAPI_STATUS_DMRD                       (0x20)
#define ATAPI_STATUS_DRDY                       (0x40)
#define ATAPI_STATUS_BSY                        (0x80)

/* Atapi Interrupt Reason Register Bit Definitions */
#define ATAPI_INTERRUPT_REASON_CD               (0x01)
#define ATAPI_INTERRUPT_REASON_IO               (0x02)
#define ATAPI_INTERRUPT_REASON_REL              (0x04)
#define ATAPI_INTERRUPT_REASON_TAG_SHIFT        (3)
#define ATAPI_INTERRUPT_REASON_TAG_MASK         (0xF8)

/* Error Register Bit Definitions */
#define ATA_ERR_ICRC                            (0x80) /* ultra DMA CRC */
#define ATA_ERR_UNC                             (0x40) /* uncorrectable error */
#define ATA_ERR_WP                              (0x40) /* write-protect bit */
#define ATA_ERR_MC                              (0x20) /* media changed */
#define ATA_ERR_IDNF                            (0x10) /* id not found */
#define ATA_ERR_MCR                             (0x08) /* media change requested */
#define ATA_ERR_ABRT                            (0x04) /* aborted command */
#define ATA_ERR_NM                              (0x02) /* no media present */

/* Atapi Error Register Bit Definitions */
#define ATAPI_ERR_ILI                           (0x01)
#define ATAPI_ERR_EOM                           (0x02)
#define ATAPI_ERR_ABRT                          (0x04)
#define ATAPI_ERR_SENSE_KEY_SHIFT               (3)
#define ATAPI_ERR_SENSE_KEY_TAG_MASK            (0xF8)

/* Device & Device/Head Register Bit Definitions */
#define ATA_DEVICE_LBA                          (0x40)

/* Device Control Register Bit Definitions */
#define ATA_DEVICE_CONTROL_SRST                 (0x04)
#define ATA_DEVICE_CONTROL_NIEN                 (0x02)

/*
 * define the ATA command set
 */
#define ATA_CMD_CHECK_POWER_MODE                                0xE5
#define ATA_CMD_DEVICE_CONFIGURATION                            0xB1
#define ATA_CMD_DOWNLOAD_MICROCODE                              0x92
#define ATA_CMD_EXECUTE_DEVICE_DIAGNOSTIC                       0x90
#define ATA_CMD_FLUSH_CACHE                                     0xE7
#define ATA_CMD_FLUSH_CACHE_EXT                                 0xEA
#define ATA_CMD_GET_MEDIA_STATUS                                0xDA
#define ATA_CMD_IDENTIFY_DEVICE                                 0xEC
#define ATA_CMD_IDENTIFY_PACKET_DEVICE                          0xA1
#define ATA_CMD_IDLE                                            0xE3
#define ATA_CMD_IDLE_IMMEDIATE                                  0xE1
#define ATA_CMD_INITIALIZE_DEVICE_PARAMETERS                    0x91
#define ATA_CMD_MEDIA_EJECT                                     0xED
#define ATA_CMD_MEDIA_LOCK                                      0xDE
#define ATA_CMD_MEDIA_UNLOCK                                    0xDF
#define ATA_CMD_NATIVE_MAX_ADDRESS                              0xF8
#define ATA_CMD_NATIVE_MAX_ADDRESS_EXT                          0x27
#define ATA_CMD_NOP                                             0x00
#define ATA_CMD_PACKET                                          0xA0
#define ATA_CMD_READ_BUFFER                                     0xE4
#define ATA_CMD_READ_DMA                                        0xC8
#define ATA_CMD_READ_DMA_EXT                                    0x25
#define ATA_CMD_READ_DMA_QUEUED                                 0xC7
#define ATA_CMD_READ_DMA_QUEUED_EXT                             0x26
#define ATA_CMD_READ_FPDMA_QUEUED                               0x60
#define ATA_CMD_READ_MULTIPLE                                   0xC4
#define ATA_CMD_READ_MULTIPLE_EXT                               0x29
#define ATA_CMD_READ_LOG_EXT                                    0x2F
#define ATA_CMD_READ_SECTORS                                    0x20
#define ATA_CMD_READ_SECTORS_EXT                                0x24
#define ATA_CMD_READ_LONG                                       0x22 /* Obsoleted in ATA-4 */
#define ATA_CMD_READ_VERIFY_SECTORS                             0x40
#define ATA_CMD_READ_VERIFY_SECTORS_EXT                         0x42
#define ATA_CMD_SECURITY_DISABLE_PASSWORD                       0xF6
#define ATA_CMD_SECURITY_ERASE_PREPARE                          0xF3
#define ATA_CMD_SECURITY_ERASE_UNIT                             0xF4
#define ATA_CMD_SECURITY_FREEZE_LOCK                            0xF5
#define ATA_CMD_SECURITY_SET_PASSWORD                           0xF1
#define ATA_CMD_SECURITY_UNLOCK                                 0xF2
#define ATA_CMD_SEEK                                            0x70
#define ATA_CMD_SET_FEATURES                                    0xEF
#define ATA_CMD_SET_MAX                                         0xF9
#define ATA_CMD_SET_MAX_EXT                                     0x37
#define ATA_CMD_SET_MULTIPLE_MODE                               0xC6
#define ATA_CMD_SLEEP                                           0xE6
#define ATA_CMD_SMART                                           0xB0
#define ATA_CMD_SMART_READ_DATA                                 0xD0
#define ATA_CMD_SMART_ENABLE_DISABLE_AUTOSAVE                   0xD2
#define ATA_CMD_SMART_SAVE_ATTRIBUTE_VALUES                     0xD3
#define ATA_CMD_SMART_EXECUTE_OFFLINE_IMMEDIATE                 0xD4
#define ATA_CMD_SMART_READ_LOG                                  0xD5
#define ATA_CMD_SMART_WRITE_LOG                                 0xD6
#define ATA_CMD_SMART_ENABLE_OPERATIONS                         0xD8
#define ATA_CMD_SMART_DISABLE_OPERATIONS                        0xD9
#define ATA_CMD_SMART_RETURN_STATUS                             0xDA
#define ATA_CMD_STANDBY                                         0xE2
#define ATA_CMD_STANDBY_IMMEDIATE                               0xE0
#define ATA_CMD_WRITE_BUFFER                                    0xE8
#define ATA_CMD_WRITE_DMA                                       0xCA
#define ATA_CMD_WRITE_DMA_EXT                                   0x35
#define ATA_CMD_WRITE_DMA_QUEUED                                0xCC
#define ATA_CMD_WRITE_DMA_QUEUED_EXT                            0x36
#define ATA_CMD_WRITE_FPDMA_QUEUED                              0x61
#define ATA_CMD_WRITE_MULTIPLE                                  0xC5
#define ATA_CMD_WRITE_MULTIPLE_EXT                              0x39
#define ATA_CMD_WRITE_SECTORS                                   0x30
#define ATA_CMD_WRITE_SECTORS_EXT                               0x34
#define ATA_CMD_WRITE_LONG                                      0x32 /* Obsoleted in ATA-4 */


/*
 * define SET_FEATURES sub-commands
 */
#define ATA_CMD_SF_SET_TRANSFER_MODE                            0x03
#define ATA_CMD_SF_WRITE_CACHE_ENABLE                           0x02
#define ATA_CMD_SF_WRITE_CACHE_DISABLE                          0x82
#define ATA_CMD_SF_ENABLE_READ_LOOK_AHEAD                       0xAA
#define ATA_CMD_SF_DISABLE_READ_LOOK_AHEAD                      0x55
#define ATA_CMD_SF_ENABLE_REVERT_PWR_ON_DFLTS                   0xCC
#define ATA_CMD_SF_DISABLE_REVERT_PWR_ON_DFLTS                  0x66
#define ATA_CMD_SF_ENABLE_SATA_FEATURE                          0x10
#define ATA_CMD_SF_DISABLE_SATA_FEATURE                         0x90

#define SATA_CMD_SF_NON_ZERO_BUFF_OFST_IN_DMA_SETUP_FIS         0x01
#define SATA_CMD_SF_DMA_SETUP_FIS_AUTO_ACTV_OPTIMIZATION        0x02
#define SATA_CMD_SF_GUARANTEED_IN_ORDER_DATA_DELIVERY           0x04
#define SATA_CMD_SF_ASYNC_NOTIFICATION                          0x05
#define SATA_CMD_SF_SOFTWARE_SETTINGS_PRESERVATION              0x06


/* Transfer mode settings for SET_FEATURES command */
#define PIO_DEFAULT                                             0x0
#define PIO_MODE3                                               0xB
#define PIO_MODE4                                               0xC

#define MDMA_MODE0                                              0x20
#define MDMA_MODE1                                              0x21
#define MDMA_MODE2                                              0x22

#define UDMA_MODE0                                              0x40
#define UDMA_MODE1                                              0x41
#define UDMA_MODE2                                              0x42
#define UDMA_MODE3                                              0x43
#define UDMA_MODE4                                              0x44
#define UDMA_MODE5                                              0x45
#define UDMA_MODE6                                              0x46

/* Error register definitions for the EXECUTE DEVICE DIAGNOSTIC command */
#define ATA_ERROR_DIAGNOSTIC_CODE_DEVICE0_PASSED                0x01

/* These signatures are left in the task file registers after an
 * EXECUTE DEVICE DIAGNOSTIC command, and at a few other times
 */
#define COMMAND_BLOCK_SIGNATURE_ATAPI                           (0x00EB1401)
#define SECTOR_COUNT_SIGNATURE_ATAPI                            (0x01)
#define COMMAND_BLOCK_SIGNATURE_ATA                             (0x00000001)
#define SECTOR_COUNT_SIGNATURE_ATA                              (0x01)


/* CHECK POWER MODE results in Sector Count register */
#define ATA_CHECK_POWER_MODE_STANDBY                            (0x00)
#define ATA_CHECK_POWER_MODE_IDLE                               (0x80)
#define ATA_CHECK_POWER_MODE_ACTIVE_OR_IDLE                     (0xFF)

/* DOWNLOAD MICROCODE SUB COMMANDS */
#define ATA_DNLD_MIC_CODE_IMMED_TEMP                            (0x01)
#define ATA_DNLD_MIC_CODE_IMMED_FUTURE                          (0x07)

/*
 * define typical IDE timeout values
 *
 * GES 02/09/01:  These values will need some tweaking!!
 */
#define ATA_TIMEOUT_GEN                                         30      /* generic timeout is 30 seconds */
#define ATA_TIMEOUT_IDENTIFY                                    2       /* timeout for IDENTIFY commands */
#define ATA_TIMEOUT_READ_BUFFER                                 2       /* timeout for read buffer */
#define ATA_TIMEOUT_DIAG                                        5       /* timeout for diagnostics command */
#define ATA_TIMEOUT_STANDBY_IDLE                                30      /* timeout for start/stop unit commands */
#define ATA_TIMEOUT_FLUSH_CACHE                                 30      /* cache flush timeout */
#define ATA_TIMEOUT_CHECK_POWER                                 2       /* timeout for CHECK POWER MODE */
#define ATA_TIMEOUT_DOWNLOAD_MICROCODE                          120     /* wait up to 2 minute */


/*
 * define the IDENTIFY DEVICE structure
 */
typedef struct _ATA_IDENTIFY_DEVICE_DATA
{
    U16  GeneralConfiguration;                                  /* 0 */
    U16  LogicalCylinders;                                      /* 1 */
    U16  SpecificConfiguration;                                 /* 2 */
    U16  LogicalHeads;                                          /* 3 */
    U16  Retired4to5[2];                                        /* 4-5 */
    U16  LogicalSectorsPerLogicalTrack;                         /* 6 */
    U16  Reserved7to8[2];                                       /* 7-8 */
    U16  Retired3;                                              /* 9 */
    U16  SerialNumber[10];                                      /* 10-19 */
    U16  Retired20to21[2];                                      /* 20-21 */
    U16  Obsolete22;                                            /* 22 */
    U16  FirmwareRevision[4];                                   /* 23-26 */
    U16  ModelNumber[20];                                       /* 27-46*/
    U16  MaxMultipleSize;                                       /* 47 */
    U16  Reserved48;                                            /* 48 */
    U16  Capabilities[2];                                       /* 49-50 */
    U16  Obsolete51to52[2];                                     /* 51-52 */
    U16  ValidWords;                                            /* 53 */
    U16  CurrentLogicalCylinders;                               /* 54 */
    U16  CurrentLogicalHeads;                                   /* 55 */
    U16  CurrentLogicalSectorsPerTrack;                         /* 56 */
    U16  CurrentCapacityInSectors[2];                           /* 57-58 */
    U16  CurrentMultipleSize;                                   /* 59 */
    U16  TotalUserSectors[2];                                   /* 60-61 */
    U16  Obsolete62;                                            /* 62 */
    U16  MultiwordDmaMode;                                      /* 63 */
    U16  PioModesSupported;                                     /* 64 */
    U16  MinimumMultiwordDmaTransferCycleTime;                  /* 65 */
    U16  ManufacturerRecommendedMultiwordDmaTransferCycleTime;  /* 66 */
    U16  MinumumPioTransferCycleTimeWithoutFlowControl;         /* 67 */
    U16  MinumumPioTransferCycleTimeWithFlowControl;            /* 68 */
    U16  Reserved69to70[2];                                     /* 69-70 */
    U16  Reserved71to74[4];                                     /* 71-74 */
    U16  QueueDepth;                                            /* 75 */
    U16  SerialATACapabilities;                                 /* 76 */
    U16  Reserved77;                                            /* 77 */
    U16  SerialFeaturesSupported[2];                            /* 78-79 */
    U16  MajorVersionNumber;                                    /* 80 */
    U16  MinorVersionNumber;                                    /* 81 */
    U16  CommandSetsSupported[3];                               /* 82-84 */
    U16  CommandSetsEnabled[2];                                 /* 85-86 */
    U16  CommandSetDefaults;                                    /* 87 */
    U16  UltraDmaModes;                                         /* 88 */
    U16  TimeForSecurityErase;                                  /* 89 */
    U16  TimeForEnahncedSecurityErase;                          /* 90 */
    U16  CurrentAdvancedPowerManagementValue;                   /* 91 */
    U16  MasterPasswordRevisionCode;                            /* 92 */
    U16  HardwareResetResult;                                   /* 93 */
    U16  AcousticManagement;                                    /* 94 */ /* GWK Reserved in ATA-5 */
    U16  Reserved95to99[5];                                     /* 95-99 */
    U16  MaximumLBAfor48bitAddressing[4];                       /* 100-103 */ /* GWK Reserved in ATA-5 */
    U16  Reserved104to107[4];                                   /* 104-107 */
    U16  WorldWideName[4];                                      /* 108-111 */
    U16  Reserved112to126[15];                                  /* 112-126 */
    U16  RemovableMediaStatusNotification;                      /* 127 */
    U16  SecurityStatus;                                        /* 128 */
    U16  VendorSpecific[31];                                    /* 129-159 */
    U16  CFApowerMode;                                          /* 160 */
    U16  Reserved161to175[15];                                  /* 161-175 */
    U16  Reserves176to254[79];                                  /* 176-254 */
    U16  IntegrityWord;                                         /* 255 */
} ATA_IDENTIFY_DEVICE_DATA, *PTR_ATA_IDENTIFY_DEVICE_DATA;

#define ATA_IDENTIFY_DATA_FIRMWARE_REVISION_LENGTH_IN_BYTES     8
#define ATA_IDENTIFY_DATA_MODEL_NUMBER_LENGTH_IN_BYTES          40
#define ATA_IDENTIFY_DATA_SERIAL_NUMBER_LENGTH_IN_BYTES         20

#define ATA_IDENTIFY_DATA_MAJOR_VERSION_ATA_ATAPI_7             (0x0080)
#define ATA_IDENTIFY_DATA_MAJOR_VERSION_ATA_ATAPI_8             (0x0100)

#define ATA_IDENTIFY_DATA_COMMAND_SET_DEFAULT_WWN               (0x0100)
#define ATA_IDENTIFY_DATA_WRITE_CACHE_ENABLED                   (0x0020)
#define ATA_IDENTIFY_DATA_LOOK_AHEAD_ENABLED                    (0x0040)
#define ATA_IDENTIFY_DATA_SMART_FEATURE_ENABLED                 (0x0001)
#define ATA_IDENTIFY_DATA_SATA_SUP_FEAT_SFT_SET_PRES            (0x0040)

/*
 * define the IDENTIFY PACKET DEVICE structure for ATAPI
 */
typedef struct _ATA_IDENTIFY_PACKET_DEVICE_DATA
{
    U16  GeneralConfiguration;                                  /* 0 */
    U16  Reserved01;                                            /* 1 */
    U16  UniqueConfiguration;                                   /* 2 */
    U16  Reserved03to09[7];                                     /* 3-9 */
    U16  SerialNumber[10];                                      /* 10-19 */
    U16  Retired20to22[3];                                      /* 20-22 */
    U16  FirmwareRevision[4];                                   /* 23-26 */
    U16  ModelNumber[20];                                       /* 27-46*/
    U16  Reserved47to48[2];                                     /* 47-48 */
    U16  Capabilities[2];                                       /* 49-50 */
    U16  Obsolete51to52[2];                                     /* 51-52 */
    U16  ValidWords;                                            /* 53 */
    U16  Reserved54to62[9];                                     /* 54-62 */
    U16  MultiwordDmaMode;                                      /* 63 */
    U16  PioModesSupported;                                     /* 64 */
    U16  MinimumMultiwordDmaTransferCycleTime;                  /* 65 */
    U16  ManufacturerRecommendedMultiwordDmaTransferCycleTime;  /* 66 */
    U16  MinumumPioTransferCycleTimeWithoutFlowControl;         /* 67 */
    U16  MinumumPioTransferCycleTimeWithFlowControl;            /* 68 */
    U16  Reserved69to70[2];                                     /* 69-70 */
    U16  TypicalTimePacketCmdToBusRelease;                      /* 71 */
    U16  TypicalTimeServiceToBSYCleared;                        /* 72 */
    U16  Reserved73to74[2];                                     /* 73-74 */
    U16  QueueDepth;                                            /* 75 */
    U16  SerialATACapabilities;                                 /* 76 */
    U16  Reserved77to79[3];                                     /* 77-79 */
    U16  MajorVersionNumber;                                    /* 80 */
    U16  MinorVersionNumber;                                    /* 81 */
    U16  CommandSetsSupported[3];                               /* 82-84 */
    U16  CommandSetsEnabled[2];                                 /* 85-86 */
    U16  CommandSetDefaults;                                    /* 87 */
    U16  UltraDmaModes;                                         /* 88 */
    U16  Reserved89to92[4];                                     /* 89-92 */
    U16  HardwareResetResult;                                   /* 93 */
    U16  AcousticManagement;                                    /* 94 */ /* GWK Reserved in ATA-5 */
    U16  Reserved95to124[30];                                   /* 95-124 */
    U16  AtapeByteCount0Behavior;                               /* 125 */
    U16  Obsolete126;                                           /* 126 */
    U16  RemovableMediaStatusNotification;                      /* 127 */
    U16  SecurityStatus;                                        /* 128 */
    U16  VendorSpecific129to159[31];                            /* 129-159 */
    U16  ReservedForCFA160to175[16];                            /* 160-175 */
    U16  Reserved176to254[79];                                  /* 176-254 */
    U16  IntegrityWord;                                         /* 255 */
} ATA_IDENTIFY_PACKET_DEVICE_DATA, *PTR_ATA_IDENTIFY_PACKET_DEVICE_DATA;

#define ATA_IDENTIFY_PACKET_DATA_FIRMWARE_REVISION_LENGTH_IN_BYTES     8
#define ATA_IDENTIFY_PACKET_DATA_MODEL_NUMBER_LENGTH_IN_BYTES          40
#define ATA_IDENTIFY_PACKET_DATA_SERIAL_NUMBER_LENGTH_IN_BYTES         20




/******************************************************************************/
/*                                                                            */
/*      S A T A   S P E C I F I C   D E F I N I T I O N S                     */
/*                                                                            */
/******************************************************************************/

/* FIS Types */
#define FIS_TYPE_REGISTER_HOST_TO_DEVICE                        0x27
#define FIS_TYPE_REGISTER_DEVICE_TO_HOST                        0x34
#define FIS_TYPE_SET_DEVICE_BITS_DEVICE_TO_HOST                 0xA1
#define FIS_TYPE_DMA_ACTIVATE_DEVICE_TO_HOST                    0x39
#define FIS_TYPE_DMA_SETUP_BIDIRECTIONAL                        0x41
#define FIS_TYPE_BIST_ACTIVATE_BIDIRECTIONAL                    0x58
#define FIS_TYPE_PIO_SETUP_DEVICE_TO_HOST                       0x5F
#define FIS_TYPE_DATA_BIDIRECTIONAL                             0x46

/* FIS Command/Control Bit Definitions */
#define FIS_COMMAND_CONTROL_COMMAND                             0x1
#define FIS_COMMAND_CONTROL_CONTROL                             0x0

/* FIS Interrupt Bit Definitions */
#define FIS_INTERRUPT_TRUE                                      0x1
#define FIS_INTERRUPT_FALSE                                     0x0

/* FIS Direction Bit Definitions */
#define FIS_DIRECTION_TRANSMITTER_TO_RECEIVER                   0x1
#define FIS_DIRECTION_RECEIVER_TO_TRANSMITTER                   0x0

/*
 * This is the mapping of how the various fields in the host to device FIS are used
 * for CHS, LBA28-bit,and LBA48-bit modes. Use the union on the register FIS
 * types to select.
 *
 *
 *          CHS                    LBA28                       LBAEXP48
 *          ---                    -----                       --------
 * 0x00 U8  FISType;               U8  FISType;                U8  FISType;
 * 0x01 U32 Reserved01Bits0to6 :7; U32 Reserved01Bits0to6 :7;  U32 Reserved01Bits0to6 :7;
 *      U32 CommandBit         :1; U32 CommandBit         :1;  U32 CommandBit         :1;
 * 0x02 U8  Command;               U8  Command;                U8  Command;
 * 0x03 U8  Features;              U8  Features;               U8  Features;
 * 0x04 U8  SectorNumber;          U8  LBALow_0_7;             U8  LBALow_0_7;
 * 0x05 U8  CylLow;                U8  LBAMid_8_15;            U8  LBAMid_8_15;
 * 0x06 U8  CylHigh;               U8  LBAHigh_16_23;          U8  LBAHigh_16_23;
 * 0x07 U8  DeviceHead;            U8  DeviceAndLBA24_27;      U8  Device;
 * 0x08 U8  SectorNumberExp;       U8  SectorNumberExp;        U8  LBALowExp_24_31;
 * 0x09 U8  CylLowExp;             U8  CylLowExp;              U8  LBAMidExp_32_39;
 * 0x0A U8  CylHighExp;            U8  CylHighExp;             U8  LBAHighExp_40_47;
 * 0x0B U8  FeaturesExp;           U8  FeaturesExp;            U8  FeaturesExp;
 * 0x0C U8  SectorCount;           U8  SectorCount;            U8  SectorCount0_7;
 * 0x0D U8  SectorCountExp;        U8  SectorCountExp;         U8  SectorCountExp8_15;
 * 0x0E U8  Reserved0E;            U8  Reserved0E;             U8  Reserved0E;
 * 0x0F U8  Control;               U8  Control;                U8  Control;
 * 0x10 U32 Reserved10to14;        U32 Reserved10to14;         U32 Reserved10to14;
 */

/*
 * SATA Register - Host To Device FIS for CHS addressing
 */
typedef struct _REGISTER_HOST_TO_DEVICE_FIS_CHS
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      CommandBit                      :1; /* 0x01 */
    U8                      Reserved01Bits0to6              :7;
#else              /* little endian */
    U8                      Reserved01Bits0to6              :7; /* 0x01 */
    U8                      CommandBit                      :1;
#endif
    U8                      Command;                            /* 0x02 */
    U8                      Features;                           /* 0x03 */
    U8                      SectorNumber;                       /* 0x04 */
    U8                      CylLow;                             /* 0x05 */
    U8                      CylHigh;                            /* 0x06 */
    U8                      DeviceHead;                         /* 0x07 */
    U8                      SectorNumberExp;                    /* 0x08 */
    U8                      CylLowExp;                          /* 0x09 */
    U8                      CylHighExp;                         /* 0x0A */
    U8                      FeaturesExp;                        /* 0x0B */
    U8                      SectorCount;                        /* 0x0C */
    U8                      SectorCountExp;                     /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Control;                            /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_HOST_TO_DEVICE_FIS_CHS, *PTR_REGISTER_HOST_TO_DEVICE_FIS_CHS;

/*
 * SATA Register - Host To Device FIS for LBA 28-bit addressing
 */
typedef struct _REGISTER_HOST_TO_DEVICE_FIS_LBA28
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      CommandBit                      :1; /* 0x01 */
    U8                      Reserved01Bits0to6              :7;
#else              /* little endian */
    U8                      Reserved01Bits0to6              :7; /* 0x01 */
    U8                      CommandBit                      :1;
#endif
    U8                      Command;                            /* 0x02 */
    U8                      Features;                           /* 0x03 */
    U8                      LBALow_0_7;                         /* 0x04 */
    U8                      LBAMid_8_15;                        /* 0x05 */
    U8                      LBAHigh_16_23;                      /* 0x06 */
    U8                      DeviceAndLBA24_27;                  /* 0x07 */
    U8                      SectorNumberExp;                    /* 0x08 */
    U8                      CylLowExp;                          /* 0x09 */
    U8                      CylHighExp;                         /* 0x0A */
    U8                      FeaturesExp;                        /* 0x0B */
    U8                      SectorCount;                        /* 0x0C */
    U8                      SectorCountExp;                     /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Control;                            /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_HOST_TO_DEVICE_FIS_LBA28, *PTR_REGISTER_HOST_TO_DEVICE_FIS_LBA28;

/*
 * SATA Register - Host To Device FIS for LBA 48-bit addressing
 */
typedef struct _REGISTER_HOST_TO_DEVICE_FIS_LBA48
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      CommandBit                      :1; /* 0x01 */
    U8                      Reserved01Bits0to6              :7;
#else              /* little endian */
    U8                      Reserved01Bits0to6              :7; /* 0x01 */
    U8                      CommandBit                      :1;
#endif
    U8                      Command;                            /* 0x02 */
    U8                      Features;                           /* 0x03 */
    U8                      LBALow_0_7;                         /* 0x04 */
    U8                      LBAMid_8_15;                        /* 0x05 */
    U8                      LBAHigh_16_23;                      /* 0x06 */
    U8                      Device;                             /* 0x07 */
    U8                      LBALowExp_24_31;                    /* 0x08 */
    U8                      LBAMidExp_32_39;                    /* 0x09 */
    U8                      LBAHighExp_40_47;                   /* 0x0A */
    U8                      FeaturesExp;                        /* 0x0B */
    U8                      SectorCount0_7;                     /* 0x0C */
    U8                      SectorCountExp8_15;                 /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Control;                            /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_HOST_TO_DEVICE_FIS_LBA48, *PTR_REGISTER_HOST_TO_DEVICE_FIS_LBA48;

/*
 * SATA Register - Host To Device FIS for Packet (ATAPI) Commands
 */
typedef struct REGISTER_HOST_TO_DEVICE_FIS_PACKET
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      CommandBit                      :1; /* 0x01 */
    U8                      Reserved01Bits0to6              :7;
#else              /* little endian */
    U8                      Reserved01Bits0to6              :7; /* 0x01 */
    U8                      CommandBit                      :1;
#endif
    U8                      Command;                            /* 0x02 */
    U8                      Features;                           /* 0x03 */
    U8                      LBALow_0_7;                         /* 0x04 */
    U8                      ByteCountLow;                       /* 0x05 */
    U8                      ByteCountHigh;                      /* 0x06 */
    U8                      DeviceSelect;                       /* 0x07 */
    U8                      LBALowExp_24_31;                    /* 0x08 */
    U8                      LBAMidExp_32_39;                    /* 0x09 */
    U8                      LBAHighExp_40_47;                   /* 0x0A */
    U8                      FeaturesExp;                        /* 0x0B */
    U8                      SectorCount0_7;                     /* 0x0C */
    U8                      SectorCountExp8_15;                 /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Control;                            /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_HOST_TO_DEVICE_FIS_PACKET, *PTR_REGISTER_HOST_TO_DEVICE_FIS_PACKET;

/*
 * SATA Register - Host To Device FIS
 */
typedef union _REGISTER_HOST_TO_DEVICE_FIS
{
    REGISTER_HOST_TO_DEVICE_FIS_CHS     Chs;
    REGISTER_HOST_TO_DEVICE_FIS_LBA28   Lba28;
    REGISTER_HOST_TO_DEVICE_FIS_LBA48   Lba48;
    REGISTER_HOST_TO_DEVICE_FIS_PACKET  Packet;
} REGISTER_HOST_TO_DEVICE_FIS, *PTR_REGISTER_HOST_TO_DEVICE_FIS;


/*
 * SATA Register - Device To Host FIS for Chs addressing
 */
typedef struct _REGISTER_DEVICE_TO_HOST_FIS_CHS
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved01Bit7                  :1; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bits0to5              :6;
#else              /* little endian */
    U8                      Reserved01Bits0to5              :6; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bit7                  :1;
#endif
    U8                      Status;                             /* 0x02 */
    U8                      Error;                              /* 0x03 */
    U8                      SectorNumber;                       /* 0x04 */
    U8                      CylLow;                             /* 0x05 */
    U8                      CylHigh;                            /* 0x06 */
    U8                      DeviceHead;                         /* 0x07 */
    U8                      SectorNumberExp;                    /* 0x08 */
    U8                      CylLowExp;                          /* 0x09 */
    U8                      CylHighExp;                         /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U8                      SectorCount;                        /* 0x0C */
    U8                      SectorCountExp;                     /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Reseved0F;                          /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_DEVICE_TO_HOST_FIS_CHS, *PTR_REGISTER_DEVICE_TO_HOST_FIS_CHS;

/*
 * SATA Register - Device To Host FIS for LBA 28-bit
 */
typedef struct _REGISTER_DEVICE_TO_HOST_FIS_LBA28
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved01Bit7                  :1; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bits0to5              :6;
#else              /* little endian */
    U8                      Reserved01Bits0to5              :6; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bit7                  :1;
#endif
    U8                      Status;                             /* 0x02 */
    U8                      Error;                              /* 0x03 */
    U8                      LBALow_0_7;                         /* 0x04 */
    U8                      LBAMid_8_15;                        /* 0x05 */
    U8                      LBAHigh_16_23;                      /* 0x06 */
    U8                      DeviceAndLBA24_27;                  /* 0x07 */
    U8                      SectorNumberExp;                    /* 0x08 */
    U8                      CylLowExp;                          /* 0x09 */
    U8                      CylHighExp;                         /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U8                      SectorCount;                        /* 0x0C */
    U8                      SectorCountExp;                     /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Reseved0F;                          /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_DEVICE_TO_HOST_FIS_LBA28, *PTR_REGISTER_DEVICE_TO_HOST_FIS_LBA28;

/*
 * SATA Register - Device To Host FIS for LBA 48-bit
 */
typedef struct _REGISTER_DEVICE_TO_HOST_FIS_LBA48
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved01Bit7                  :1; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bits0to5              :6;
#else              /* little endian */
    U8                      Reserved01Bits0to5              :6; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bit7                  :1;
#endif
    U8                      Status;                             /* 0x02 */
    U8                      Error;                              /* 0x03 */
    U8                      LBALow_0_7;                         /* 0x04 */
    U8                      LBAMid_8_15;                        /* 0x05 */
    U8                      LBAHigh_16_23;                      /* 0x06 */
    U8                      Device;                             /* 0x07 */
    U8                      LBALowExp_24_31;                    /* 0x08 */
    U8                      LBAMidExp_32_39;                    /* 0x09 */
    U8                      LBAHighExp_40_47;                   /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U8                      SectorCount0_7;                     /* 0x0C */
    U8                      SectorCountExp8_15;                 /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Reseved0F;                          /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_DEVICE_TO_HOST_FIS_LBA48, *PTR_REGISTER_DEVICE_TO_HOST_FIS_LBA48;

/*
 * SATA Register - Device To Host FIS for Packet (ATAPI) Commands
 */
typedef struct _REGISTER_DEVICE_TO_HOST_FIS_PACKET
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved01Bit7                  :1; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bits0to5              :6;
#else              /* little endian */
    U8                      Reserved01Bits0to5              :6; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bit7                  :1;
#endif
    U8                      AtapiStatus;                        /* 0x02 */
    U8                      Error;                              /* 0x03 */
    U8                      LBALow_0_7;                         /* 0x04 */
    U8                      ByteCountLow;                       /* 0x05 */
    U8                      ByteCountHigh;                      /* 0x06 */
    U8                      DeviceSelect;                       /* 0x07 */
    U8                      LBALowExp_24_31;                    /* 0x08 */
    U8                      LBAMidExp_32_39;                    /* 0x09 */
    U8                      LBAHighExp_40_47;                   /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U8                      InterruptReason;                    /* 0x0C */
    U8                      SectorCountExp8_15;                 /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      Reseved0F;                          /* 0x0F */
    U32                     Reserved10to14;                     /* 0x10 */
} REGISTER_DEVICE_TO_HOST_FIS_PACKET, *PTR_REGISTER_DEVICE_TO_HOST_FIS_PACKET;

/*
 * SATA Register - Device To Host FIS
 */
typedef union _REGISTER_DEVICE_TO_HOST_FIS
{
  REGISTER_DEVICE_TO_HOST_FIS_CHS       Chs;
  REGISTER_DEVICE_TO_HOST_FIS_LBA28     Lba28;
  REGISTER_DEVICE_TO_HOST_FIS_LBA48     Lba48;
  REGISTER_DEVICE_TO_HOST_FIS_PACKET    Packet;
} REGISTER_DEVICE_TO_HOST_FIS, *PTR_REGISTER_DEVICE_TO_HOST_FIS;

/*
 * SATA Set Device Bits - Device To Host FIS
 */
typedef struct _SET_DEVICE_BITS_DEVICE_TO_HOST_FIS
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Notification                    :1; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Reserved01Bits0to5              :6;
    U8                      Reserved02Bit7                  :1; /* 0x02 */
    U8                      StatusHi                        :3;
    U8                      Reserved02Bit3                  :1;
    U8                      StatusLo                        :3;
#else              /* little endian */
    U8                      Reserved01Bits0to5              :6; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Notification                    :1;
    U8                      StatusLo                        :3; /* 0x02 */
    U8                      Reserved02Bit3                  :1;
    U8                      StatusHi                        :3;
    U8                      Reserved02Bit7                  :1;
#endif
    U8                      Error;                              /* 0x03 */
    U32                     SActive;                            /* 0x04 */
} SET_DEVICE_BITS_DEVICE_TO_HOST_FIS, *PTR_SET_DEVICE_BITS_DEVICE_TO_HOST_FIS;

/*
 * SATA DMA Activate - Device To Host FIS
 */
typedef struct _DMA_ACTIVATE_DEVICE_TO_HOST_FIS
{
    U8                      FISType;                            /* 0x00 */
    U8                      Reserved01;                         /* 0x01 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
} DMA_ACTIVATE_DEVICE_TO_HOST_FIS, *PTR_DMA_ACTIVATE_DEVICE_TO_HOST_FIS;

/*
 * SATA DMA Setup (Bi-directional) FIS
 */
typedef struct _DMA_SETUP_FIS
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved01Bit7                  :1; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Direction                       :1;
    U8                      Reserved01Bits0to4              :5;
#else              /* little endian */
    U8                      Reserved01Bits0to4              :5; /* 0x01 */
    U8                      Direction                       :1;
    U8                      Interrupt                       :1;
    U8                      Reserved01Bit7                  :1;
#endif
    U8                      Reserved02;                         /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     DMABufferIdentifierLow;             /* 0x04 */
    U32                     DMABufferIdentifierHigh;            /* 0x08 */
    U32                     Reserved0Cto0F;                     /* 0x0C */
    U32                     DMABufferOffset;                    /* 0x10 */
    U32                     DMATransferCount;                   /* 0x14 */
    U32                     Reserved18;                         /* 0x18 */
} DMA_SETUP_FIS, *PTR_DMA_SETUP_FIS;

/*
 * SATA BIST Activate FIS - I don't believe we suppor this ?
 */

/*
 * SATA PIO Setup - Device To Host FIS
 */
typedef struct _PIO_SETUP_DEVICE_TO_HOST_FIS
{
    U8                      FISType;                            /* 0x00 */
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      Reserved01Bit7                  :1; /* 0x01 */
    U8                      Interrupt                       :1;
    U8                      Direction                       :1;
    U8                      Reserved01Bits0to4              :5;
#else              /* little endian */
    U8                      Reserved01Bits0to4              :5; /* 0x01 */
    U8                      Direction                       :1;
    U8                      Interrupt                       :1;
    U8                      Reserved01Bit7                  :1;
#endif
    U8                      Status;                             /* 0x02 */
    U8                      Error;                              /* 0x03 */
    U8                      SectorNumber;                       /* 0x04 */
    U8                      CylLow;                             /* 0x05 */
    U8                      CylHigh;                            /* 0x06 */
    U8                      DeviceHead;                         /* 0x07 */
    U8                      SectorNumberExp;                    /* 0x08 */
    U8                      CylLowExp;                          /* 0x09 */
    U8                      CylHighExp;                         /* 0x0A */
    U8                      Reserved0B;                         /* 0x0B */
    U8                      SectorCount;                        /* 0x0C */
    U8                      SectorCountExp;                     /* 0x0D */
    U8                      Reserved0E;                         /* 0x0E */
    U8                      E_Status;                           /* 0x0F */
    U16                     TransferCount;                      /* 0x10 */
    U16                     Reserved12to13;                     /* 0x12 */
} PIO_SETUP_DEVICE_TO_HOST_FIS, *PTR_PIO_SETUP_DEVICE_TO_HOST_FIS;

/* PIO Setup Direction Bit */
#define PIO_SETUP_DIRECTION_HOST_TO_DEVICE                      (0)
#define PIO_SETUP_DIRECTION_DEVICE_TO_HOST                      (1)

/*
 * SATA Data - (Bi-directional) FIS
 */
typedef struct _DATA_FIS
{
    U8                      FISType;                            /* 0x00 */
    U8                      Reserved01;                         /* 0x01 */
    U8                      Reserved02;                         /* 0x02 */
    U8                      Reserved03;                         /* 0x03 */
    U32                     Data[1];                            /* 0x04 */
    /* Note the data is variable in length */
} DATA_FIS, *PTR_DATA_FIS;

/* This is a union of all the SATA FIS's */
typedef union _PL_SATA_FIS_UNION
{
    REGISTER_HOST_TO_DEVICE_FIS         HtoDFIS;
    REGISTER_DEVICE_TO_HOST_FIS         DtoHFIS;
    SET_DEVICE_BITS_DEVICE_TO_HOST_FIS  SetDeviceDtoHFIS;
    DMA_ACTIVATE_DEVICE_TO_HOST_FIS     DmaActivateDtoHFIS;
    DMA_SETUP_FIS                       DmaSetupFIS;
    PIO_SETUP_DEVICE_TO_HOST_FIS        PioSetupDtoHFIS;
    DATA_FIS                            DataFIS;
} PL_SATA_FIS_UNION, * PTR_PL_SATA_FIS_UNION;


typedef struct _READ_LOG_EXT_PAGE_10_DATA
{
#if defined( __LSIUTIL_BIG_ENDIAN__ )
    U8                      NQ                              :1; /* 0x00 */
    U8                      Reserved00Bit5to6               :2;
    U8                      Tag                             :5;
#else              /* little endian */
    U8                      Tag                             :5; /* 0x00 */
    U8                      Reserved00Bit5to6               :2;
    U8                      NQ                              :1;
#endif
    U8                      Reserved01;                         /* 0x01 */
    U8                      Status;                             /* 0x02 */
    U8                      Error;                              /* 0x03 */
    U8                      SectorNumber;                       /* 0x04 */
    U8                      CylinderLow;                        /* 0x05 */
    U8                      CylinderHigh;                       /* 0x06 */
    U8                      DevHead;                            /* 0x07 */
    U8                      SectorNumberExp;                    /* 0x08 */
    U8                      CylinderLowExp;                     /* 0x09 */
    U8                      CylinderHighExp;                    /* 0x0A */
    U8                      Reserved11;                         /* 0x0B */
    U8                      SectorCount;                        /* 0x0C */
    U8                      SectorCountExp;                     /* 0x0D */
    U8                      Reserved14to511[1];                 /* 0x0E */
} READ_LOG_EXT_PAGE_10_DATA, *PTR_READ_LOG_EXT_PAGE_10_DATA;


#endif /* End of #if ATA_H_SOURCE */

