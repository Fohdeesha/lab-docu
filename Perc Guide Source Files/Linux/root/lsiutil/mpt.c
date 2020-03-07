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
/*
 * mpt -- simple driver for MPT adapters (FC, SCSI, and SAS/SATA)
 *
 * Written by Stephen F. Shirron, August 9, 2005
 */


#ifdef DISABLE_TIMEOUTS
static time_t local_time = 0;
#define time(x) (local_time)
#endif


#define NUM_REPLIES 7
#define REPLY_FREE_SIZE 8
#define REPLY_POST_SIZE 16
#define COMMAND_CONTEXT 0x111111
#define PASS_THRU_CONTEXT 0x555555


#define mpi1    (adap->mpt_version < MPI2_VERSION_02_00)
#define mpi2    (adap->mpt_version >= MPI2_VERSION_02_00)
#define mpi20   ((adap->mpt_version >= MPI2_VERSION_02_00) && (adap->mpt_version < MPI2_VERSION_02_05))
#define mpi25   (adap->mpt_version >= MPI2_VERSION_02_05)


typedef struct mpt_io_rep
{
    union
    {
        SCSIIOReply_t    default_reply;
        U32              reply[32];
    } mf;
} mpt_io_rep_t;


typedef struct mpt_shared
{
    mpt_io_rep_t     replies[NUM_REPLIES];
    U32              message[32];
    U32              free_queue[REPLY_FREE_SIZE];
    MPI2_DEFAULT_REPLY_DESCRIPTOR
                     post_queue[REPLY_POST_SIZE];
    U16              data[64];
    U32              config[256];
    U8               scratch[1024*1024 + 1024];  // 1MB + 1K - big enough for 1MB payload plus request/response
    U32              hrsm_value;
} mpt_shared_t;


typedef struct mpt_adap
{
    MPT_PORT        *port;
    HANDLE           partner_adap;
    char            *name;
    U16              vendor_id;
    U16              device_id;
    U8               revision_id;
    U8               segment_number;
    U8               bus_number;
    U8               device_function;
    int              mpt_version;
    int              hrsm_capable;
    U32              hrsm_value;
    U32              msg_context;
    int              restart_needed;
    int              port_enable_needed;
    int              config_active;
    int              command_active;
    mpt_shared_t    *shared;
    mpt_bus_addr_t   shared_ba;
    int              message_size;
    int              reply_depth;
    int              credits;
    int              port_count;
    int              block_size;
    int              port_type;
    int              host_id;
    U32              interrupt_mask;
    int              max_targets;
    int              bus_reset_needed;
    U32              capabilities[16];
    int              ioc_online;
    int              port_online;
    int              bootloader;
    int              loaddevice;
    U8              *fw_image;
    int              fw_image_size;
    int              fw_image_asked;
    U32              mem_size;
    U32              diagmem_size;
#if DOS
    U32              io_addr;
    U32              mem_addr;
    U32              mem_virt;
    U32              diagmem_addr;
    U32              diagmem_virt;
    CONTIGUOUS_MEMORY    shared_contig_mem;
#endif
#if EFI
    EFI_HANDLE       handle;
    EFI_PCI_IO_PROTOCOL *pci_io;
    void            *buffer_mapping;
    int              disconnected;
#endif
    int              io;
    int              mem;
    int              diagmem;
    int              free_index;
    int              post_index;
} mpt_adap_t;


#define rl(x) mpt_read32(adap, MPI_##x##_OFFSET, adap->mem)
#define wl(x,y) mpt_write32(adap, MPI_##x##_OFFSET, y, adap->mem)
#define rl2(x) mpt_read32(adap, MPI2_##x##_OFFSET, adap->mem)
#define wl2(x,y) mpt_write32(adap, MPI2_##x##_OFFSET, y, adap->mem)
#define rlio(x) mpt_read32(adap, MPI_##x##_OFFSET, adap->io)
#define wlio(x,y) mpt_write32(adap, MPI_##x##_OFFSET, y, adap->io)
#define rldiag(x,y) wl(TEST_BASE_ADDRESS, x & ~mask); y = mpt_read32(adap, x & mask, adap->diagmem)
#define wldiag(x,y) wl(TEST_BASE_ADDRESS, x & ~mask); mpt_write32(adap, x & mask, y, adap->diagmem)
U32 mpt_read32(mpt_adap_t *adap, int offset, int bar)
{
    U32 data;

#if DOS
    if (bar == adap->mem)
        data = *(U32 *)(adap->mem_virt + offset);
    else if (bar == adap->diagmem)
        data = *(U32 *)(adap->diagmem_virt + offset);
    else
        data = inpd(adap->io_addr + offset);
#endif
#if EFI
    if (bar == adap->mem || bar == adap->diagmem)
        adap->pci_io->Mem.Read(adap->pci_io, EfiPciIoWidthUint32, (char)bar, offset, 1, &data);
    else
        adap->pci_io->Io.Read(adap->pci_io, EfiPciIoWidthUint32, (char)bar, offset, 1, &data);
#endif

    return data;
}
void mpt_write32(mpt_adap_t *adap, int offset, U32 data, int bar)
{
#if DOS
    if (bar == adap->mem)
        *(U32 *)(adap->mem_virt + offset) = data;
    else if (bar == adap->diagmem)
        *(U32 *)(adap->diagmem_virt + offset) = data;
    else
        outpd(adap->io_addr + offset, data);
#endif
#if EFI
    if (bar == adap->mem || bar == adap->diagmem)
        adap->pci_io->Mem.Write(adap->pci_io, EfiPciIoWidthUint32, (char)bar, offset, 1, &data);
    else
        adap->pci_io->Io.Write(adap->pci_io, EfiPciIoWidthUint32, (char)bar, offset, 1, &data);
#endif
}
void mpt_write8(mpt_adap_t *adap, int offset, U8 data)
{
#if DOS
    *(U8 *)(adap->diagmem_virt + offset) = data;
#endif
#if EFI
    adap->pci_io->Mem.Write(adap->pci_io, EfiPciIoWidthUint8, (char)adap->diagmem, offset, 1, &data);
#endif
}
#define MPI_DEBUG_OFFSET 0x18
#define MPI_FC909_BUG_OFFSET 0x90
#define MPI_SAS1078_RESET_OFFSET 0x10FC


int mpt_adjust_delay(mpt_adap_t *adap, int n);
void mpt_delay(mpt_adap_t *adap, int n);
int mpt_get_doorbell(mpt_adap_t *adap);
int mpt_get_state(mpt_adap_t *adap);
int mpt_verify_enabled(mpt_adap_t *adap);
int mpt_verify_ready(mpt_adap_t *adap);
int mpt_verify_operational(mpt_adap_t *adap);
int mpt_wait_for_ready(mpt_adap_t *adap);
int mpt_restart(mpt_adap_t *adap);
int mpt_stop(mpt_adap_t *adap, int wait);
int mpt_start(mpt_adap_t *adap);
int mpt_port_online(mpt_adap_t *adap);
int mpt_wait_for_doorbell(mpt_adap_t *adap, time_t limit);
int mpt_wait_for_response(mpt_adap_t *adap, time_t limit);
int mpt_send_message(mpt_adap_t *adap, int length, time_t limit);
int mpt_receive_data(mpt_adap_t *adap, int length, time_t limit);
int mpt_issue_command_and_wait(mpt_adap_t *adap, int wait);
int mpt_issue_config_and_wait(mpt_adap_t *adap, int wait);
int mpt_issue_ioc_facts(mpt_adap_t *adap);
int mpt2_issue_ioc_facts(mpt_adap_t *adap);
int mpt_issue_ioc_init(mpt_adap_t *adap);
int mpt2_issue_ioc_init(mpt_adap_t *adap);
int mpt_initialize(mpt_adap_t *adap);
int mpt_issue_port_facts(mpt_adap_t *adap, int port);
int mpt2_issue_port_facts(mpt_adap_t *adap, int port);
int mpt_issue_port_enable(mpt_adap_t *adap, int port);
int mpt2_issue_port_enable(mpt_adap_t *adap, int port);
int mpt_issue_event_notification(mpt_adap_t *adap);
int mpt2_issue_event_notification(mpt_adap_t *adap);
int mpt_watchdog(mpt_adap_t *adap);
int mpt_init_device_capabilities(mpt_adap_t *adap);
int mpt_set_device_capabilities(mpt_adap_t *adap, int targ);
int mpt_set_on_bus_timer(mpt_adap_t *adap, int timeout);
int mpt_get_config_page(mpt_adap_t *adap, int type, int number, int address);
int mpt_set_config_page(mpt_adap_t *adap, int type, int number, int address);
int mpt_do_config_action(mpt_adap_t *adap, int action, int type, int number, int address, int length);
void mpt_bus_reset(mpt_adap_t *adap);
void mpt_interrupt(mpt_adap_t *adap);
void mpt2_interrupt(mpt_adap_t *adap);
int mpt_handle_reply(mpt_adap_t *adap, MPIDefaultReply_t *reply, U32 reply_ba);
void mpt2_handle_scsi_io_success(mpt_adap_t *adap, Mpi2DefaultReplyDescriptor_t *reply_desc);
void mpt2_handle_address_reply(mpt_adap_t *adap, Mpi2DefaultReplyDescriptor_t *reply_desc);
int mpt_fwdownloadboot(mpt_adap_t *adap);
int mpt_directdownload(mpt_adap_t *adap);


int
mpt_adjust_delay(mpt_adap_t *adap, int n)
{
    if (adap->revision_id == 0xff)
        return n * 10;

    return n;
}


void
mpt_delay(mpt_adap_t *adap, int n)
{
#if DOS
    int     i;

    for (i = 0; i < mpt_adjust_delay(adap, n); i++)
    {
        PciReadConfigWord(adap->bus_number, adap->device_function, PCI_CONFIG_DEVICE_ID);
    }
#endif
#if EFI
    udelay(mpt_adjust_delay(adap, n));
#endif
}


int
mpt_get_doorbell(mpt_adap_t *adap)
{
    return rl(DOORBELL);
}


int
mpt_get_state(mpt_adap_t *adap)
{
    return rl(DOORBELL) & MPI_IOC_STATE_MASK;
}


int
mpt_verify_enabled(mpt_adap_t *adap)
{
    if (rl(DIAGNOSTIC) & MPI_DIAG_DISABLE_ARM)
        if (mpt_stop(adap, TRUE))
            return mpt_restart(adap);

    return 1;
}


int
mpt_verify_ready(mpt_adap_t *adap)
{
    if (adap->restart_needed == TRUE)
        return 0;

    if ((rl(DOORBELL) & MPI_IOC_STATE_MASK) != MPI_IOC_STATE_READY)
        return 0;

    return 1;
}


int
mpt_verify_operational(mpt_adap_t *adap)
{
    if (adap->restart_needed == TRUE)
        return 0;

    if ((rl(DOORBELL) & MPI_IOC_STATE_MASK) != MPI_IOC_STATE_OPERATIONAL)
        return 0;

    return 1;
}


int
mpt_wait_for_ready(mpt_adap_t *adap)
{
    U32      doorbell;
    U32      diagnostic;
    int      state;
    time_t   limit = time(NULL) + mpt_adjust_delay(adap, 50);

    while (time(NULL) < limit)
    {
        doorbell = rl(DOORBELL);
        state = doorbell & MPI_IOC_STATE_MASK;
        if (state == MPI_IOC_STATE_FAULT)
        {
            printf("\n%s: MPT Firmware Fault, code %04x\n",
                   adap->name, doorbell & MPI_DOORBELL_DATA_MASK);
            if (wFlag)
                fprintf(logFile, "%s:  MPT Firmware Fault, code %04x\n",
                        adap->name, doorbell & MPI_DOORBELL_DATA_MASK);

            printf("\nDo you want to continue?  [Yes or No, default is No] ");

            if (getYesNoAnswer(0) != 1)
                break;

            printf("\n");

            adap->restart_needed = TRUE;
            return 0;
        }
        if (state == MPI_IOC_STATE_READY)
        {
            adap->restart_needed = FALSE;
            return 1;
        }
        diagnostic = rl(DIAGNOSTIC);
        if (diagnostic & MPI_DIAG_FLASH_BAD_SIG)
        {
            if (adap->fw_image == NULL)
            {
                printf("\n%s: FLASH does not contain a valid image\n", adap->name);
                if (wFlag)
                    fprintf(logFile, "%s:  FLASH does not contain a valid image\n",
                            adap->name);
            }
            adap->restart_needed = TRUE;
            return 0;
        }
        if (diagnostic & MPI_DIAG_DISABLE_ARM)
        {
            printf("\n%s: IOP ARM is disabled\n", adap->name);
            if (wFlag)
                fprintf(logFile, "%s:  IOP ARM is disabled\n",
                        adap->name);
            adap->restart_needed = TRUE;
            return 0;
        }
        DELAY(1);
    }

    return 0;
}


/*
 * mpt_restart - completely restart the IOC and any outstanding commands.
 */
int
mpt_restart(mpt_adap_t *adap)
{
    int      targ;

//  printf("mpt_restart called\n");

    if (!mpt_verify_ready(adap))
    {
        adap->restart_needed = FALSE;
        adap->bus_reset_needed = FALSE;

        if (!mpt_stop(adap, TRUE))
            return 0;
    }

    adap->config_active = FALSE;
    adap->command_active = FALSE;

    if (!mpt_start(adap))
        return 0;

    if (adap->port_type == MPI_PORTFACTS_PORTTYPE_SCSI)
    {
        mpt_init_device_capabilities(adap);

        for (targ = 0; targ < adap->max_targets; targ++)
        {
            mpt_set_device_capabilities(adap, targ);
        }
    }

    return 1;
}


/*
 * mpt_stop - stop the IOC (reset it).
 */
int
mpt_stop(mpt_adap_t *adap, int wait)
{
    time_t   limit = time(NULL) + mpt_adjust_delay(adap, 30);

//  printf("mpt_stop called\n");

#if EFI
    EFI_STATUS  status;

    if (adap->disconnected == FALSE)
    {
        EFI_PCI_IO_PROTOCOL     *pci_io;

        if (wait == FALSE)
        {
            return 1;
        }

        if (adap->loaddevice == TRUE)
        {
            printf("This chip controls the device that this utility was loaded from.\n");
            printf("After this command, the load device cannot be accessed again\n");
            printf("to open files, until this utility has been exited and restarted.\n\n");
        }

        if (!EFI_ERROR(BS->DisconnectController(adap->handle, NULL, NULL)))
        {
            adap->disconnected = TRUE;
            pci_io = adap->pci_io;
            status = pci_io->Attributes(pci_io, EfiPciIoAttributeOperationEnable,
                                        EFI_PCI_IO_ATTRIBUTE_IO |
                                        EFI_PCI_IO_ATTRIBUTE_MEMORY |
                                        EFI_PCI_IO_ATTRIBUTE_BUS_MASTER |
                                        EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE, NULL);

            if (status == EFI_UNSUPPORTED)
            {
                pci_io->Attributes(pci_io, EfiPciIoAttributeOperationEnable,
                                   EFI_PCI_IO_ATTRIBUTE_IO |
                                   EFI_PCI_IO_ATTRIBUTE_MEMORY |
                                   EFI_PCI_IO_ATTRIBUTE_BUS_MASTER, NULL);
            }
        }

        if (adap->partner_adap)
        {
            if (!EFI_ERROR(BS->DisconnectController(adap->partner_adap->handle, NULL, NULL)))
            {
                adap->partner_adap->disconnected = TRUE;
                pci_io = adap->partner_adap->pci_io;
                status = pci_io->Attributes(pci_io, EfiPciIoAttributeOperationEnable,
                                            EFI_PCI_IO_ATTRIBUTE_IO |
                                            EFI_PCI_IO_ATTRIBUTE_MEMORY |
                                            EFI_PCI_IO_ATTRIBUTE_BUS_MASTER |
                                            EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE, NULL);

                if (status == EFI_UNSUPPORTED)
                {
                    pci_io->Attributes(pci_io, EfiPciIoAttributeOperationEnable,
                                       EFI_PCI_IO_ATTRIBUTE_IO |
                                       EFI_PCI_IO_ATTRIBUTE_MEMORY |
                                       EFI_PCI_IO_ATTRIBUTE_BUS_MASTER, NULL);
                }
            }
        }
    }
#endif

    adap->port_online = FALSE;

    adap->interrupt_mask = MPI_HIM_RIM | MPI_HIM_DIM;
    wl(HOST_INTERRUPT_MASK, rl(HOST_INTERRUPT_MASK) | adap->interrupt_mask);

    /*
     * Reset the chip.
     */
    if (adap->device_id == MPI_MANUFACTPAGE_DEVICEID_FC909)
    {
        rl(FC909_BUG);    /* work around FC909 bug */
    }

    if (!(rl(DIAGNOSTIC) & MPI_DIAG_DRWE))
    {
        wl(WRITE_SEQUENCE, 0);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_KEY_VALUE_MASK);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_1ST_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_2ND_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_3RD_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_4TH_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_5TH_KEY_VALUE);
        DELAY(100);
    }

    if (!(rl(DIAGNOSTIC) & MPI_DIAG_RESET_HISTORY) ||
        ((rl(DOORBELL) & MPI_IOC_STATE_MASK) != MPI_IOC_STATE_RESET &&
         (rl(DOORBELL) & MPI_IOC_STATE_MASK) != MPI_IOC_STATE_READY))
    {
        if (adap->device_id == MPI_MANUFACTPAGE_DEVID_SAS1078)
        {
            wl(SAS1078_RESET, 0x7);
        }
        else
        {
            wl(DIAGNOSTIC, MPI_DIAG_RESET_ADAPTER);
        }
        DELAY(50000);
        wl(WRITE_SEQUENCE, 0);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_KEY_VALUE_MASK);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_1ST_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_2ND_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_3RD_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_4TH_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_5TH_KEY_VALUE);
        DELAY(100);
    }

    wl(DIAGNOSTIC, 0);

    if (wait == FALSE)
    {
        while (time(NULL) < limit)
        {
            if (rl(DOORBELL) & MPI_IOC_STATE_MASK)
                break;
            if (rl(DIAGNOSTIC) & MPI_DIAG_DISABLE_ARM)
                break;
            DELAY(100);
        }

        return 1;
    }

    if (rl(DIAGNOSTIC) & MPI_DIAG_RESET_ADAPTER)
    {
        printf("\n%s: Failed to clear RESET ADAPTER\n", adap->name);
        if (wFlag)
            fprintf(logFile, "%s:  Failed to clear RESET ADAPTER\n",
                    adap->name);
        adap->restart_needed = TRUE;
        return 0;
    }

    adap->bootloader = FALSE;

    if (mpt_wait_for_ready(adap))
        return 1;

    if (mpt_fwdownloadboot(adap))
        return 1;

    if ((rl(DOORBELL) & MPI_IOC_STATE_MASK) == MPI_IOC_STATE_RESET)
    {
        adap->restart_needed = FALSE;

        printf("\n%s: Failed to leave the RESET state\n", adap->name);
        if (wFlag)
            fprintf(logFile, "%s:  Failed to leave the RESET state\n",
                    adap->name);
    }
    else
    {
        adap->restart_needed = TRUE;

        printf("\n%s: Failed to reset properly\n", adap->name);
        if (wFlag)
            fprintf(logFile, "%s:  Failed to reset properly\n",
                    adap->name);
    }

    return 0;
}


/*
 * mpt_start - start the IOC (completely initialize it).
 */
int
mpt_start(mpt_adap_t *adap)
{
    int     port;

//  printf("mpt_start called\n");

    if (adap->bootloader == TRUE)
    {
        printf("\nBootLoader firmware cannot be made fully operational!\n");
        return 0;
    }

    if (!mpt_issue_ioc_facts(adap))
    {
        printf("%s: Failed to send IOCFacts\n", adap->name);
        return 0;
    }

    if (!mpt_issue_ioc_init(adap))
    {
        printf("%s: Failed to send IOCInit\n", adap->name);
        return 0;
    }

    if (!mpt_initialize(adap))
    {
        printf("%s: Failed to finish initialization of IOC\n", adap->name);
        return 0;
    }

    for (port = 0; port < adap->port_count; port++)
    {
        if (!mpt_issue_port_facts(adap, port))
        {
            printf("%s: Failed to send PortFacts to port %d\n", adap->name, port);
            continue;
        }
    }

    if (adap->port_enable_needed)
    {
        adap->port_online = mpt_port_online(adap);
    }

#if 0 // skip this for DOS and EFI, as it doesn't really help
    if (!mpt_issue_event_notification(adap))
    {
        printf("%s: failed to send EventNotification\n", adap->name);
    }
#endif

    if (adap->port_type == MPI_PORTFACTS_PORTTYPE_SCSI)
    {
        if (!mpt_set_on_bus_timer(adap, 1000))
        {
            printf("%s: Failed to set SCSI On Bus Timer\n", adap->name);
        }
    }

    return 1;
}


/*
 * mpt_port_online - bring the port online.
 */
int
mpt_port_online(mpt_adap_t *adap)
{
    int     port;

//  printf("mpt_port_online called\n");

    for (port = 0; port < adap->port_count; port++)
    {
        if (adap->port_type == MPI_PORTFACTS_PORTTYPE_SAS)
        {
            //TODO: MPI12.5 - This is where it crashes
            if (doSasChangeWwid(adap->port, 1) == 1)
            {
                printf("\nThe SAS WWID is zero -- the port will not be enabled\n");
                printf("\nUse menu option 18 then option 99 to fix this problem\n");
                continue;
            }
        }

        if (!mpt_issue_port_enable(adap, port))
        {
            printf("%s: Failed to send PortEnable to port %d\n", adap->name, port);
            continue;
        }
    }

    return 1;
}


int
mpt_wait_for_doorbell(mpt_adap_t *adap, time_t limit)
{
    U32      doorbell;
    int      state;

    while (time(NULL) < limit)
    {
        if (adap->restart_needed == TRUE)
        {
            return 0;
        }
        doorbell = rl(DOORBELL);
        state = doorbell & MPI_IOC_STATE_MASK;
        if (state == MPI_IOC_STATE_FAULT)
        {
            printf("\n%s: MPT Firmware Fault, code %04x\n",
                   adap->name, doorbell & MPI_DOORBELL_DATA_MASK);
            if (wFlag)
                fprintf(logFile, "%s:  MPT Firmware Fault, code %04x\n",
                        adap->name, doorbell & MPI_DOORBELL_DATA_MASK);

            printf("\nDo you want to continue?  [Yes or No, default is No] ");

            if (getYesNoAnswer(0) != 1)
                break;

            printf("\n");

            adap->restart_needed = TRUE;
            return 0;
        }
        if (rl(HOST_INTERRUPT_STATUS) & MPI_HIS_DOORBELL_INTERRUPT)
        {
            return 1;
        }
        DELAY(1);
    }

    printf("%s: Failed in mpt_wait_for_doorbell, Doorbell = %08x\n",
           adap->name, doorbell);
    return 0;
}


int
mpt_wait_for_response(mpt_adap_t *adap, time_t limit)
{
    U32      doorbell;
    int      state;

    while (time(NULL) < limit)
    {
        if (adap->restart_needed == TRUE)
        {
            return 0;
        }
        doorbell = rl(DOORBELL);
        state = doorbell & MPI_IOC_STATE_MASK;
        if (state == MPI_IOC_STATE_FAULT)
        {
            printf("\n%s: MPT Firmware Fault, code %04x\n",
                   adap->name, doorbell & MPI_DOORBELL_DATA_MASK);
            if (wFlag)
                fprintf(logFile, "%s:  MPT Firmware Fault, code %04x\n",
                        adap->name, doorbell & MPI_DOORBELL_DATA_MASK);

            printf("\nHit Enter to continue ");

            getYesNoAnswer(0);

            printf("\n");

            adap->restart_needed = TRUE;
            return 0;
        }
        if (!(rl(HOST_INTERRUPT_STATUS) & MPI_HIS_IOP_DOORBELL_STATUS))
        {
            return 1;
        }
        DELAY(1);
    }

    printf("%s: Failed in mpt_wait_for_response, Doorbell = %08x\n",
           adap->name, doorbell);
    return 0;
}


int
mpt_send_message(mpt_adap_t *adap, int length, time_t limit)
{
    U32      doorbell;
    U32     *message = adap->shared->message;
    int      state;
    int      i;

    doorbell = rl(DOORBELL);
    state = doorbell & MPI_IOC_STATE_MASK;
    if (state == MPI_IOC_STATE_FAULT)
    {
        printf("\n%s: MPT Firmware Fault, code %04x\n",
               adap->name, doorbell & MPI_DOORBELL_DATA_MASK);
        if (wFlag)
            fprintf(logFile, "%s:  MPT Firmware Fault, code %04x\n",
                    adap->name, doorbell & MPI_DOORBELL_DATA_MASK);

        printf("\nHit Enter to continue ");

        getYesNoAnswer(0);

        printf("\n");

        adap->restart_needed = TRUE;
        return 0;
    }
    if (state == MPI_IOC_STATE_RESET)
    {
        adap->restart_needed = TRUE;
        return 0;
    }
    if (doorbell & MPI_DOORBELL_USED)
    {
        printf("%s: Doorbell already active\n", adap->name);
        adap->restart_needed = TRUE;
        return 0;
    }
    wl(HOST_INTERRUPT_STATUS, 0);
    wl(DOORBELL, (MPI_FUNCTION_HANDSHAKE << MPI_DOORBELL_FUNCTION_SHIFT) |
                  ((length / 4) << MPI_DOORBELL_ADD_DWORDS_SHIFT));
    if (!mpt_wait_for_doorbell(adap, limit))
        return 0;
    wl(HOST_INTERRUPT_STATUS, 0);
    if (!mpt_wait_for_response(adap, limit))
        return 0;
    for (i = 0; i < length / 4; i++)
    {
        wl(DOORBELL, get32x(message[i]));
        if (!mpt_wait_for_response(adap, limit))
            return 0;
    }

    return 1;
}


int
mpt_receive_data(mpt_adap_t *adap, int length, time_t limit)
{
    int                  i;
    int                  real_length;
    U16                  value;
    U16                 *data = adap->shared->data;
    MPIDefaultReply_t   *reply = (MPIDefaultReply_t *)data;
    U16                  status;

    bzero(data, length);
    real_length = 4;    /* enough for the header to start, fix up later */
    for (i = 0; i < real_length / 2; i++)
    {
        if (!mpt_wait_for_doorbell(adap, limit))
            return 0;
        value = (U16)rl(DOORBELL);
        if (i == 1)
            real_length = (value & ~0xff00) * 4;
        if (i < length / 2)
            data[i] = get16x(value);
        wl(HOST_INTERRUPT_STATUS, 0);
    }
    if (!mpt_wait_for_doorbell(adap, limit))
        return 0;
    wl(HOST_INTERRUPT_STATUS, 0);

    status = get16(reply->IOCStatus) & MPI_IOCSTATUS_MASK;
    if (status == MPI_IOCSTATUS_SUCCESS ||
        status == MPI_IOCSTATUS_CONFIG_INVALID_PAGE ||
        status == MPI_IOCSTATUS_CONFIG_INVALID_DATA ||
        status == MPI_IOCSTATUS_CONFIG_INVALID_ACTION)
        return 1;
    else
        return 0;
}


int
mpt_issue_command_and_wait(mpt_adap_t *adap, int wait)
{
    U8       function = ((MPIHeader_t *)adap->shared->message)->Function;
    int      handle;
    time_t   limit = time(NULL) + wait;

    adap->command_active = TRUE;

    if (mpi1)
    {
        if (function == MPI_FUNCTION_SCSI_TASK_MGMT)
        {
            if (mpt_send_message(adap, sizeof(SCSITaskMgmt_t), limit) != 1)
            {
                printf("mpt_send_message failed!\n");
                return 0;
            }
        }
        else
        {
            wl(REQUEST_QUEUE, (U32)adap->shared_ba + (U32)offsetof(mpt_shared_t, message));
        }
    }
    else
    {
        if (function == MPI_FUNCTION_SCSI_TASK_MGMT)
        {
            /*  SCSI Task Mgmt requests have to use high priority  */
            wl2(REQUEST_DESCRIPTOR_POST_LOW, (1 << 16) + MPI2_REQ_DESCRIPT_FLAGS_HIGH_PRIORITY);
            wl2(REQUEST_DESCRIPTOR_POST_HIGH, 0);
        }
        else if (function == MPI_FUNCTION_SCSI_IO_REQUEST)
        {
            handle = get16(((Mpi2SCSIIORequest_t *)adap->shared->message)->DevHandle);

            /*  SCSI requests have to use SCSI IO and supply a handle  */
            wl2(REQUEST_DESCRIPTOR_POST_LOW, (1 << 16) + MPI2_REQ_DESCRIPT_FLAGS_SCSI_IO);
            wl2(REQUEST_DESCRIPTOR_POST_HIGH, handle << 16);
        }
        else if (function == MPI_FUNCTION_RAID_SCSI_IO_PASSTHROUGH)
        {
            handle = get16(((Mpi2SCSIIORequest_t *)adap->shared->message)->DevHandle);

            /*  RAID Passthru requests have to use default descriptor and supply a handle  */
            wl2(REQUEST_DESCRIPTOR_POST_LOW, (1 << 16) + MPI2_REQ_DESCRIPT_FLAGS_DEFAULT_TYPE);
            wl2(REQUEST_DESCRIPTOR_POST_HIGH, handle << 16);
        }
        else
        {
            wl2(REQUEST_DESCRIPTOR_POST_LOW, (1 << 16) + MPI2_REQ_DESCRIPT_FLAGS_DEFAULT_TYPE);
            wl2(REQUEST_DESCRIPTOR_POST_HIGH, 0);
        }
    }

    while (time(NULL) < limit)
    {
        if (mpt_watchdog(adap))
        {
            break;
        }
        mpt_interrupt(adap);
        if (adap->command_active == TRUE)
        {
            DELAY(1);
        }
        else
        {
            break;
        }
    }

    if (adap->command_active == TRUE)
    {
        if (time(NULL) >= limit)
            printf("timeout occurred!\n");
        return 0;
    }

    return 1;
}


int
mpt_issue_config_and_wait(mpt_adap_t *adap, int wait)
{
    time_t   limit = time(NULL) + wait;

    adap->config_active = TRUE;

    if (mpi1)
    {
        wl(REQUEST_QUEUE, (U32)adap->shared_ba + (U32)offsetof(mpt_shared_t, message));
    }
    else
    {
        wl2(REQUEST_DESCRIPTOR_POST_LOW, (1 << 16) + MPI2_REQ_DESCRIPT_FLAGS_DEFAULT_TYPE);
        wl2(REQUEST_DESCRIPTOR_POST_HIGH, 0);
    }

    while (time(NULL) < limit)
    {
        if (adap->restart_needed == TRUE)
        {
            return 0;
        }
        mpt_interrupt(adap);
        if (adap->config_active == TRUE)
        {
            DELAY(1);
        }
        else
        {
            break;
        }
    }

    if (adap->config_active == TRUE)
        return 0;

    return 1;
}


/*
 * mpt_issue_ioc_facts - issue an IOCFacts command to the IOC.
 */
int
mpt_issue_ioc_facts(mpt_adap_t *adap)
{
    IOCFacts_t          *request = (IOCFacts_t *)adap->shared->message;
    IOCFactsReply_t     *reply = (IOCFactsReply_t *)adap->shared->data;
    time_t               limit = time(NULL) + 10;
    int                  t;

//  printf("%s: sending IOCFacts\n", adap->name);

    bzero(request, sizeof(*request));

    request->Function = MPI_FUNCTION_IOC_FACTS;

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / IOCFacts failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / IOCFacts failed\n", adap->name);
        return 0;
    }

    adap->mpt_version = get16(reply->MsgVersion);

    if (mpi2)
    {
        return mpt2_issue_ioc_facts(adap);
    }

    if (reply->WhoInit == MPI_WHOINIT_PCI_PEER)
    {
        printf("%s: WhoInit is PCI Peer!\n", adap->name);
        return 0;
    }

//  printf("BlockSize = %x\n", reply->BlockSize);
//  printf("ReplyQueueDepth = %x\n", get16(reply->ReplyQueueDepth));
//  printf("RequestFrameSize = %x\n", get16(reply->RequestFrameSize));
//  printf("GlobalCredits = %x\n", get16(reply->GlobalCredits));
//  printf("NumberOfPorts = %x\n", reply->NumberOfPorts);

//  if (get16(reply->MsgVersion) < MPI_VERSION_01_02)
//      printf("%s: MPT Version = %04x, Firmware Version = %04x\n",
//             adap->name, get16(reply->MsgVersion),
//             get16(reply->Reserved_0101_FWVersion));
//  else
//      printf("%s: MPT Version = %04x, Firmware Version = %08x\n",
//             adap->name, get16(reply->MsgVersion),
//             get32(reply->FWVersion.Word));

    adap->message_size = get16(reply->RequestFrameSize) * 4;
    adap->reply_depth = get16(reply->ReplyQueueDepth);
    adap->credits = get16(reply->GlobalCredits);
    adap->port_count = reply->NumberOfPorts;
    adap->block_size = reply->BlockSize;

    adap->hrsm_capable = FALSE;
    if (get16(reply->MsgVersion) >= MPI_VERSION_01_05)
        if (get32(reply->IOCCapabilities) & MPI_IOCFACTS_CAPABILITY_REPLY_HOST_SIGNAL)
            adap->hrsm_capable = TRUE;
    if (adap->device_id == MPI_MANUFACTPAGE_DEVID_SAS1064  ||
        adap->device_id == MPI_MANUFACTPAGE_DEVID_SAS1064E ||
        adap->device_id == MPI_MANUFACTPAGE_DEVID_SAS1066  ||
        adap->device_id == MPI_MANUFACTPAGE_DEVID_SAS1066E ||
        adap->device_id == MPI_MANUFACTPAGE_DEVID_SAS1068  ||
        adap->device_id == MPI_MANUFACTPAGE_DEVID_SAS1068E)
    {
        U32     version = get32(reply->FWVersion.Word);

        if (version < 0x00040100)
            adap->hrsm_capable = FALSE;
        if (version >= 0x01000000 && version < 0x01050000)
            adap->hrsm_capable = FALSE;
    }

    if (reply->MaxDevices)
        if (adap->max_targets > reply->MaxDevices)
            adap->max_targets = reply->MaxDevices;

    if (adap->port_type == MPI_PORTFACTS_PORTTYPE_SCSI)
        if (adap->max_targets > 16)
            adap->max_targets = 16;

    return 1;
}


int
mpt2_issue_ioc_facts(mpt_adap_t *adap)
{
    Mpi2IOCFactsRequest_t   *request = (Mpi2IOCFactsRequest_t *)adap->shared->message;
    Mpi2IOCFactsReply_t     *reply = (Mpi2IOCFactsReply_t *)adap->shared->data;
    time_t                   limit = time(NULL) + 10;
    int                      t;

    bzero(request, sizeof(*request));

    request->Function = MPI2_FUNCTION_IOC_FACTS;

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / IOCFacts failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / IOCFacts failed\n", adap->name);
        return 0;
    }

    if (reply->WhoInit == MPI2_WHOINIT_PCI_PEER)
    {
        printf("%s: WhoInit is PCI Peer!\n", adap->name);
        return 0;
    }

//  printf("MaxReplyDescriptorPostQueueDepth = %x\n", get16(reply->MaxReplyDescriptorPostQueueDepth));
//  printf("IOCRequestFrameSize = %x\n", get16(reply->IOCRequestFrameSize));
//  printf("RequestCredit = %x\n", get16(reply->RequestCredit));
//  printf("NumberOfPorts = %x\n", reply->NumberOfPorts);

//  printf("%s: MPT Version = %04x, Firmware Version = %08x\n",
//         adap->name, get16(reply->MsgVersion),
//         get32(reply->FWVersion.Word));

    adap->message_size = get16(reply->IOCRequestFrameSize) * 4;
    adap->reply_depth = get16(reply->MaxReplyDescriptorPostQueueDepth);
    adap->credits = get16(reply->RequestCredit);
    adap->port_count = reply->NumberOfPorts;

    adap->hrsm_capable = FALSE;

    if (get16(reply->MaxTargets))
        if (adap->max_targets > get16(reply->MaxTargets))
            adap->max_targets = get16(reply->MaxTargets);

    return 1;
}


/*
 * mpt_issue_ioc_init - issue an IOCInit command to the IOC.
 */
int
mpt_issue_ioc_init(mpt_adap_t *adap)
{
    IOCInit_t       *request = (IOCInit_t *)adap->shared->message;
    IOCInitReply_t  *reply = (IOCInitReply_t *)adap->shared->data;
    time_t           limit = time(NULL) + 30;
    int              t;
#if EFI
   _U32              high = set32((U32)(adap->shared_ba >> 32));
#endif

//  printf("%s: sending IOCInit\n", adap->name);

    if (mpi2)
    {
        return mpt2_issue_ioc_init(adap);
    }

    bzero(request, sizeof(*request));

    request->Function = MPI_FUNCTION_IOC_INIT;
    request->WhoInit = MPI_WHOINIT_HOST_DRIVER;
    request->MaxDevices = adap->max_targets;
    request->MaxBuses = 1;
    request->MsgContext = set32(COMMAND_CONTEXT);
    request->ReplyFrameSize = set16(sizeof(adap->shared->replies[0]));
#if EFI
    request->HostMfaHighAddr = high;
    request->SenseBufferHighAddr = high;
#endif
    request->MsgVersion = set16(MPI_VERSION);
    request->HeaderVersion = set16(MPI_HEADER_VERSION);

    if (adap->hrsm_capable == TRUE)
    {
        adap->hrsm_value = 0;
        request->Flags |= MPI_IOCINIT_FLAGS_REPLY_FIFO_HOST_SIGNAL;
        request->ReplyFifoHostSignalingAddr =
            set32((U32)adap->shared_ba +
                  (U32)offsetof(mpt_shared_t, hrsm_value));
    }

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / IOCInit failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / IOCInit failed\n", adap->name);
        return 0;
    }
    reply = reply;

    return 1;
}


int
mpt2_issue_ioc_init(mpt_adap_t *adap)
{
    Mpi2IOCInitRequest_t    *request = (Mpi2IOCInitRequest_t *)adap->shared->message;
    Mpi2IOCInitReply_t      *reply = (Mpi2IOCInitReply_t *)adap->shared->data;
    time_t                   limit = time(NULL) + 30;
    int                      t;
    time_t                   now;
    uint64_t                 nowMsec;
#if EFI
   _U32                      high = set32((U32)(adap->shared_ba >> 32));
#endif

    bzero(request, sizeof(*request));

    request->Function = MPI2_FUNCTION_IOC_INIT;
    request->WhoInit = MPI2_WHOINIT_SYSTEM_BIOS;
    request->SystemRequestFrameSize = set16(sizeof(adap->shared->message) / 4);
    request->ReplyDescriptorPostQueueDepth = set16(REPLY_POST_SIZE);
    request->ReplyFreeQueueDepth = set16(REPLY_FREE_SIZE);
    request->SystemRequestFrameBaseAddress.Low =
        set32((U32)adap->shared_ba + offsetof(mpt_shared_t, message) - sizeof(adap->shared->message));
    request->ReplyDescriptorPostQueueAddress.Low =
        set32((U32)adap->shared_ba + offsetof(mpt_shared_t, post_queue));
    request->ReplyFreeQueueAddress.Low =
        set32((U32)adap->shared_ba + offsetof(mpt_shared_t, free_queue));
#if EFI
    request->SystemReplyAddressHigh = high;
    request->SenseBufferAddressHigh = high;
    request->SystemRequestFrameBaseAddress.High = high;
    request->ReplyDescriptorPostQueueAddress.High = high;
    request->ReplyFreeQueueAddress.High = high;
#endif
    request->MsgVersion = set16(MPI2_VERSION);
    request->HeaderVersion = set16(MPI2_HEADER_VERSION);

    time(&now);
    nowMsec = ((uint64_t)(now)) * 1000;
    request->TimeStamp.Low = set32((U32)(nowMsec & 0x00000000FFFFFFFF));
    request->TimeStamp.High = set32((U32)((nowMsec & 0xFFFFFFFF00000000) >> 32));

    adap->free_index = 0;
    adap->post_index = 0;

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / IOCInit failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / IOCInit failed\n", adap->name);
        return 0;
    }
    reply = reply;

    return 1;
}


/*
 * mpt_initialize - finish initialization of IOC.
 */
int
mpt_initialize(mpt_adap_t *adap)
{
    U32      doorbell;
    int      i;
    int      state;
    time_t   limit = time(NULL) + 10;

//  printf("%s: initializing IOC\n", adap->name);

    while (time(NULL) < limit)
    {
        doorbell = rl(DOORBELL);
        state = doorbell & MPI_IOC_STATE_MASK;
        if (state == MPI_IOC_STATE_OPERATIONAL)
            break;
        DELAY(1);
    }

    if (state != MPI_IOC_STATE_OPERATIONAL)
    {
        if (state == MPI_IOC_STATE_FAULT)
        {
            printf("\n%s: MPT Firmware Fault, code %04x\n",
                   adap->name, doorbell & MPI_DOORBELL_DATA_MASK);
            if (wFlag)
                fprintf(logFile, "%s:  MPT Firmware Fault, code %04x\n",
                        adap->name, doorbell & MPI_DOORBELL_DATA_MASK);

            printf("\nHit Enter to continue ");

            getYesNoAnswer(0);

            printf("\n");
        }
        adap->restart_needed = TRUE;
        return 0;
    }

    if (mpi1)
    {
        for (i = 0; i < NUM_REPLIES; i++)
        {
            wl(REPLY_QUEUE, (U32)adap->shared_ba + (U32)offsetof(mpt_shared_t, replies[i]));
        }
    }
    else
    {
        for (i = 0; i < NUM_REPLIES; i++)
        {
            adap->shared->free_queue[i] =
                (U32)adap->shared_ba + (U32)offsetof(mpt_shared_t, replies[i]);
        }
        adap->free_index = i;
        wl2(REPLY_FREE_HOST_INDEX, i);
        bzero(adap->shared->post_queue, sizeof(adap->shared->post_queue));
        for (i = 0; i < REPLY_POST_SIZE; i++)
        {
            adap->shared->post_queue[i].ReplyFlags = MPI2_RPY_DESCRIPT_FLAGS_UNUSED;
        }
    }

    adap->interrupt_mask &= ~MPI_HIM_RIM;
//  wl(HOST_INTERRUPT_MASK, adap->interrupt_mask);

    return 1;
}


/*
 * mpt_issue_port_facts - issue a PortFacts command to the IOC.
 */
int
mpt_issue_port_facts(mpt_adap_t *adap, int port)
{
    PortFacts_t         *request = (PortFacts_t *)adap->shared->message;
    PortFactsReply_t    *reply = (PortFactsReply_t *)adap->shared->data;
    time_t               limit = time(NULL) + 10;
    int                  t;

//  printf("%s: sending PortFacts to port %d\n", adap->name, port);

    if (mpi2)
    {
        return mpt2_issue_port_facts(adap, port);
    }

    bzero(request, sizeof(*request));

    request->Function = MPI_FUNCTION_PORT_FACTS;
    request->PortNumber = port;
    request->MsgContext = set32(COMMAND_CONTEXT);

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / PortFacts failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / PortFacts failed\n", adap->name);
        return 0;
    }

    adap->port_type = reply->PortType;
    adap->host_id = get16(reply->PortSCSIID);

    return 1;
}


int
mpt2_issue_port_facts(mpt_adap_t *adap, int port)
{
    Mpi2PortFactsRequest_t  *request = (Mpi2PortFactsRequest_t *)adap->shared->message;
    Mpi2PortFactsReply_t    *reply = (Mpi2PortFactsReply_t *)adap->shared->data;
    time_t                   limit = time(NULL) + 10;
    int                      t;

    bzero(request, sizeof(*request));

    request->Function = MPI2_FUNCTION_PORT_FACTS;
    request->PortNumber = port;

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / PortFacts failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / PortFacts failed\n", adap->name);
        return 0;
    }

    adap->port_type = reply->PortType;

    return 1;
}


/*
 * mpt_issue_port_enable - issue a PortEnable command to the IOC.
 */
int
mpt_issue_port_enable(mpt_adap_t *adap, int port)
{
    PortEnable_t        *request = (PortEnable_t *)adap->shared->message;
    MPIDefaultReply_t   *reply = (MPIDefaultReply_t *)adap->shared->data;
    time_t               limit = time(NULL) + 120;
    int                  t;

//  printf("%s: sending PortEnable to port %d\n", adap->name, port);

    if (mpi2)
    {
        return mpt2_issue_port_enable(adap, port);
    }

    bzero(request, sizeof(*request));

    request->Function = MPI_FUNCTION_PORT_ENABLE;
    request->PortNumber = port;
    request->MsgContext = set32(COMMAND_CONTEXT);

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / PortEnable failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / PortEnable failed\n", adap->name);
        return 0;
    }
    reply = reply;

    return 1;
}


int
mpt2_issue_port_enable(mpt_adap_t *adap, int port)
{
    Mpi2PortEnableRequest_t     *request = (Mpi2PortEnableRequest_t *)adap->shared->message;
    Mpi2PortEnableReply_t       *reply = (Mpi2PortEnableReply_t *)adap->shared->data;
    time_t                       limit = time(NULL) + 120;
    int                          t;

    bzero(request, sizeof(*request));

    request->Function = MPI2_FUNCTION_PORT_ENABLE;
    request->VP_ID = port;

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / PortEnable failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / PortEnable failed\n", adap->name);
        return 0;
    }
    reply = reply;

    return 1;
}


/*
 * mpt_issue_event_notification - issue an EventNotification command to the IOC.
 */
int
mpt_issue_event_notification(mpt_adap_t *adap)
{
    EventNotification_t         *request = (EventNotification_t *)adap->shared->message;
    EventNotificationReply_t    *reply = (EventNotificationReply_t *)adap->shared->data;
    time_t                       limit = time(NULL) + 10;
    int                          t;

//  printf("%s: sending EventNotification\n", adap->name);

    if (mpi2)
    {
        return mpt2_issue_event_notification(adap);
    }

    bzero(request, sizeof(*request));

    request->Function = MPI_FUNCTION_EVENT_NOTIFICATION;
    request->Switch = MPI_EVENT_NOTIFICATION_SWITCH_ON;
    request->MsgContext = set32(COMMAND_CONTEXT);

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / EventNotification failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / EventNotification failed\n", adap->name);
        return 0;
    }
    reply = reply;

    return 1;
}


int
mpt2_issue_event_notification(mpt_adap_t *adap)
{
    Mpi2EventNotificationRequest_t  *request = (Mpi2EventNotificationRequest_t *)adap->shared->message;
    Mpi2EventNotificationReply_t    *reply = (Mpi2EventNotificationReply_t *)adap->shared->data;
    time_t                           limit = time(NULL) + 10;
    int                              t;

    bzero(request, sizeof(*request));

    request->Function = MPI2_FUNCTION_EVENT_NOTIFICATION;

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / EventNotification failed\n", adap->name);
        return 0;
    }

    t = mpt_receive_data(adap, sizeof(*reply), limit);

    logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

    if (!t)
    {
        printf("%s: mpt_receive_data / EventNotification failed\n", adap->name);
        return 0;
    }
    reply = reply;

    return 1;
}


/*
 * mpt_watchdog - routine that acts as a watchdog.
 */
int
mpt_watchdog(mpt_adap_t *adap)
{
    U32      doorbell;
    int      state;

    if (mpi2)  // move code to mpt_interrupt, so we don't poll during DMA
    {
        doorbell = rl(DOORBELL);
        state = doorbell & MPI_IOC_STATE_MASK;
        if (state != MPI_IOC_STATE_OPERATIONAL)
        {
            if (state == MPI_IOC_STATE_FAULT)
            {
                printf("%s: MPT Firmware Fault, code %04x\n",
                       adap->name, doorbell & MPI_DOORBELL_DATA_MASK);
                if (wFlag)
                    fprintf(logFile, "%s:  MPT Firmware Fault, code %04x\n",
                            adap->name, doorbell & MPI_DOORBELL_DATA_MASK);

                printf("\nHit Enter to Continue ");

                getYesNoAnswer(0);

                printf("\n");
            }
            adap->restart_needed = TRUE;
        }
    }

    if (adap->restart_needed == TRUE)
    {
        mpt_restart(adap);
        return 1;
    }
    else if (adap->bus_reset_needed == TRUE)
    {
        adap->bus_reset_needed = FALSE;
        mpt_bus_reset(adap);
    }

    return 0;
}


/*
 * mpt_init_device_capabilities - initialize SCSI device capabilities.
 */
int
mpt_init_device_capabilities(mpt_adap_t *adap)
{
    SCSIPortPage0_t     *scsi_port_0 = (SCSIPortPage0_t *)adap->shared->config;
    SCSIPortPage2_t     *scsi_port_2 = (SCSIPortPage2_t *)adap->shared->config;
    int                  init_capabilities;
    int                  init_sync_period;
    int                  targ_capabilities;
    int                  targ_sync_period;
    int                  targ_device_flags;
    int                  targ;

    if (!mpt_get_config_page(adap, MPI_CONFIG_PAGETYPE_SCSI_PORT, 0, 0))
    {
        printf("%s: Failed to get SCSI Port Page 0\n", adap->name);
        return 0;
    }

    init_capabilities = get32(scsi_port_0->Capabilities);

    init_sync_period = MPI_SCSIPORTPAGE0_CAP_GET_MIN_SYNC_PERIOD(init_capabilities);

    if (!mpt_get_config_page(adap, MPI_CONFIG_PAGETYPE_SCSI_PORT, 2, 0))
    {
        printf("%s: Failed to get SCSI Port Page 2\n", adap->name);
        return 0;
    }

    for (targ = 0; targ < adap->max_targets; targ++)
    {
        targ_sync_period = scsi_port_2->DeviceSettings[targ].SyncFactor;
        targ_device_flags = get16(scsi_port_2->DeviceSettings[targ].DeviceFlags);
        targ_capabilities = init_capabilities;
        if (targ_sync_period == 0 || targ_sync_period > init_sync_period)
        {
            targ_capabilities &= ~MPI_SCSIDEVPAGE1_RP_MIN_SYNC_PERIOD_MASK;
            targ_capabilities |= targ_sync_period << MPI_SCSIDEVPAGE1_RP_SHIFT_MIN_SYNC_PERIOD;
        }
        if (targ_device_flags & MPI_SCSIPORTPAGE2_DEVICE_WIDE_DISABLE)
        {
            targ_capabilities &= ~MPI_SCSIDEVPAGE1_RP_WIDE;
        }
        adap->capabilities[targ] = targ_capabilities;
    }

    return 1;
}


/*
 * mpt_set_device_capabilities - set SCSI device capabilities.
 */
int
mpt_set_device_capabilities(mpt_adap_t *adap, int targ)
{
    SCSIDevicePage1_t   *scsi_device_1 = (SCSIDevicePage1_t *)adap->shared->config;

//  printf("%s: setting device capabilities\n", adap->name);

    bzero(scsi_device_1, sizeof(*scsi_device_1));

    /*
     * Set the control bits we know we want, as well as the maximum
     * synchronous offset and minimum synchronous period.
     */
    scsi_device_1->RequestedParameters = set32(adap->capabilities[targ]);

    if (!mpt_set_config_page(adap, MPI_CONFIG_PAGETYPE_SCSI_DEVICE, 1, targ))
    {
        printf("%s: Failed to set SCSI Device Page 1 for %d\n",
               adap->name, targ);
        return 0;
    }

    return 1;
}


/*
 * mpt_set_on_bus_timer - set SCSI port on-bus-timer value.
 */
int
mpt_set_on_bus_timer(mpt_adap_t *adap, int timeout)
{
    SCSIPortPage1_t     *scsi_port_1 = (SCSIPortPage1_t *)adap->shared->config;
    U32                  config;

//  printf("%s: setting on-bus-timer\n", adap->name);

    bzero(scsi_port_1, sizeof(*scsi_port_1));

    config = adap->host_id | (1 << (adap->host_id + 16));
    scsi_port_1->Configuration = set32(config);
    /*
     * Convert from milliseconds to 1.6 microsecond units.
     */
    scsi_port_1->OnBusTimerValue = set32(timeout * 1000*10/16);

    if (!mpt_set_config_page(adap, MPI_CONFIG_PAGETYPE_SCSI_PORT, 1, 0))
    {
        printf("%s: Failed to set SCSI Port Page 1\n", adap->name);
    }

    return 1;
}


/*
 * mpt_get_config_page - get a configuration page.
 */
int
mpt_get_config_page(mpt_adap_t *adap, int type, int number, int address)
{
    ConfigReply_t   *reply = (ConfigReply_t *)adap->shared->data;
    int              t;
    int              ioc_status;
    int              length;

    t = mpt_do_config_action(adap, MPI_CONFIG_ACTION_PAGE_HEADER, type, number, address, 0);
    ioc_status = get16(reply->IOCStatus) & MPI_IOCSTATUS_MASK;
    length = reply->Header.PageLength;
    if (!t || ioc_status != MPI_IOCSTATUS_SUCCESS || length == 0)
    {
        if (ioc_status != MPI_IOCSTATUS_CONFIG_INVALID_PAGE)
        {
            printf("%s: Failed to get config page header\n",
                   adap->name);
            printf("%s:   IOCStatus = %04x, PageLength = %x\n",
                   adap->name, ioc_status, length);
            printf("%s:   type = %d, number = %d, address = %x\n",
                   adap->name, type, number, address);
        }
        return 0;
    }

    t = mpt_do_config_action(adap, MPI_CONFIG_ACTION_PAGE_READ_CURRENT, type, number, address, length);
    ioc_status = get16(reply->IOCStatus) & MPI_IOCSTATUS_MASK;
    if (!t || ioc_status != MPI_IOCSTATUS_SUCCESS)
    {
        if (ioc_status != MPI_IOCSTATUS_CONFIG_INVALID_PAGE)
        {
            printf("%s: Failed to get config page\n",
                   adap->name);
            printf("%s:   IOCStatus = %04x, PageLength = %x\n",
                   adap->name, ioc_status, length);
            printf("%s:   type = %d, number = %d, address = %x\n",
                   adap->name, type, number, address);
        }
        return 0;
    }

    return 1;
}


/*
 * mpt_set_config_page - set a configuration page.
 */
int
mpt_set_config_page(mpt_adap_t *adap, int type, int number, int address)
{
    ConfigReply_t   *reply = (ConfigReply_t *)adap->shared->data;
    int              t;
    int              ioc_status;
    int              length;

    t = mpt_do_config_action(adap, MPI_CONFIG_ACTION_PAGE_HEADER, type, number, address, 0);
    ioc_status = get16(reply->IOCStatus) & MPI_IOCSTATUS_MASK;
    length = reply->Header.PageLength;
    if (!t || ioc_status != MPI_IOCSTATUS_SUCCESS || length == 0)
    {
        if (ioc_status != MPI_IOCSTATUS_CONFIG_INVALID_PAGE)
        {
            printf("%s: Failed to get config page header\n",
                   adap->name);
            printf("%s:   IOCStatus = %04x, PageLength = %x\n",
                   adap->name, ioc_status, length);
            printf("%s:   type = %d, number = %d, address = %x\n",
                   adap->name, type, number, address);
        }
        return 0;
    }

    *(ConfigPageHeader_t *)adap->shared->config= reply->Header;

    t = mpt_do_config_action(adap, MPI_CONFIG_ACTION_PAGE_WRITE_CURRENT, type, number, address, length);
    ioc_status = get16(reply->IOCStatus) & MPI_IOCSTATUS_MASK;
    if (!t || ioc_status != MPI_IOCSTATUS_SUCCESS)
    {
        if (ioc_status != MPI_IOCSTATUS_CONFIG_INVALID_PAGE)
        {
            printf("%s: Failed to set config page\n",
                   adap->name);
            printf("%s:   IOCStatus = %04x, PageLength = %x\n",
                   adap->name, ioc_status, length);
            printf("%s:   type = %d, number = %d, address = %x\n",
                   adap->name, type, number, address);
        }
        return 0;
    }

    return 1;
}


int
mpt_do_config_action(mpt_adap_t *adap, int action, int type, int number, int address, int length)
{
    Config_t        *request = (Config_t *)adap->shared->message;
    ConfigReply_t   *reply = (ConfigReply_t *)adap->shared->data;
    SGESimple64_t   *sge_simple = (SGESimple64_t *)&request->PageBufferSGE;
    time_t           limit = time(NULL) + 10;
    int              t;
    U32              flags;

    bzero(request, sizeof(*request));
    bzero(reply, sizeof(*reply));

    request->Function = MPI_FUNCTION_CONFIG;
    request->Action = action;
    request->MsgContext = set32(COMMAND_CONTEXT);
    if (action == MPI_CONFIG_ACTION_PAGE_WRITE_CURRENT ||
        action == MPI_CONFIG_ACTION_PAGE_WRITE_NVRAM)
    {
        request->Header = *(ConfigPageHeader_t *)adap->shared->config;
    }
    else
    {
        request->Header.PageType = type;
        request->Header.PageNumber = number;
        request->Header.PageLength = length;
    }
    request->PageAddress = set32(address);
    if (length)
    {
        flags = (length * 4) |
            MPI_SGE_SET_FLAGS(MPI_SGE_FLAGS_SIMPLE_ELEMENT    |
                              MPI_SGE_FLAGS_LAST_ELEMENT      |
                              MPI_SGE_FLAGS_64_BIT_ADDRESSING |
                              MPI_SGE_FLAGS_END_OF_BUFFER     |
                              MPI_SGE_FLAGS_END_OF_LIST);
        if (action == MPI_CONFIG_ACTION_PAGE_WRITE_CURRENT ||
            action == MPI_CONFIG_ACTION_PAGE_WRITE_NVRAM)
        {
            flags |= MPI_SGE_SET_FLAGS(MPI_SGE_FLAGS_HOST_TO_IOC);
        }
        else
        {
            flags |= MPI_SGE_SET_FLAGS(MPI_SGE_FLAGS_IOC_TO_HOST);
        }
        sge_simple->FlagsLength = set32(flags);
        sge_simple->Address.Low = set32((U32)adap->shared_ba + offsetof(mpt_shared_t, config));
#if EFI
        sge_simple->Address.High = set32((U32)(adap->shared_ba >> 32));
#endif
    }
    else if (mpi2)
    {
        sge_simple->FlagsLength = set32(
            MPI_SGE_SET_FLAGS(MPI_SGE_FLAGS_SIMPLE_ELEMENT    |
                              MPI_SGE_FLAGS_LAST_ELEMENT      |
                              MPI_SGE_FLAGS_64_BIT_ADDRESSING |
                              MPI_SGE_FLAGS_END_OF_BUFFER     |
                              MPI_SGE_FLAGS_END_OF_LIST));
    }


    if (mpt_verify_operational(adap))
    {
        logMptCommandReq(adap->port, request, sizeof(*request));

        t = mpt_issue_config_and_wait(adap, 10);

        logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

        if (!t)
        {
            if ((get16(reply->IOCStatus) & MPI_IOCSTATUS_MASK) != MPI_IOCSTATUS_CONFIG_INVALID_PAGE)
            {
                printf("%s: mpt_issue_config_and_wait / Config failed!\n", adap->name);
            }
            return 0;
        }

        return 1;
    }

    if (mpt_verify_ready(adap))
    {
        logMptCommandReq(adap->port, request, sizeof(*request));

        if (!mpt_send_message(adap, sizeof(*request), limit))
        {
            printf("%s: mpt_send_message / Config failed\n", adap->name);
            return 0;
        }

        t = mpt_receive_data(adap, sizeof(*reply), limit);

        logMptCommandRep(adap->port, request, sizeof(*request), reply, sizeof(*reply), t);

        if (!t)
        {
            if ((get16(reply->IOCStatus) & MPI_IOCSTATUS_MASK) != MPI_IOCSTATUS_CONFIG_INVALID_PAGE)
            {
                printf("%s: mpt_receive_data / Config failed\n", adap->name);
            }
            return 0;
        }

        return 1;
    }

    return 0;
}


void
mpt_bus_reset(mpt_adap_t *adap)
{
    SCSITaskMgmt_t  *request = (SCSITaskMgmt_t *)adap->shared->message;
    time_t           limit = time(NULL) + 10;

//  printf("%s: sending SCSITaskMgmt (ResetBus)\n", adap->name);

    /*
     * Use a SCSI Task Management request to reset the bus.
     */
    bzero(request, sizeof(*request));

    request->Function = MPI_FUNCTION_SCSI_TASK_MGMT;
    request->TaskType = MPI_SCSITASKMGMT_TASKTYPE_RESET_BUS;
    request->MsgFlags = MPI_SCSITASKMGMT_MSGFLAGS_LIP_RESET_OPTION;

    logMptCommandReq(adap->port, request, sizeof(*request));

    if (!mpt_send_message(adap, sizeof(*request), limit))
    {
        printf("%s: mpt_send_message / SCSITaskMgmt failed\n", adap->name);
        adap->restart_needed = TRUE;
    }
}


void
mpt_interrupt(mpt_adap_t *adap)
{
    U32                  context;
    U32                  reply_ba;
    MPIDefaultReply_t   *reply;
    U32                  doorbell;
    int                  state;

//  printf("mpt_interrupt called\n");

    if (mpi2)
    {
        mpt2_interrupt(adap);
        return;
    }

    if (adap->hrsm_capable)
    {
        if (adap->hrsm_value == adap->shared->hrsm_value)
            return;
        adap->hrsm_value = adap->shared->hrsm_value;
    }

    doorbell = rl(DOORBELL);
    state = doorbell & MPI_IOC_STATE_MASK;
    if (state != MPI_IOC_STATE_OPERATIONAL)
    {
        if (state == MPI_IOC_STATE_FAULT)
        {
            printf("%s: MPT Firmware Fault, code %04x\n",
                   adap->name, doorbell & MPI_DOORBELL_DATA_MASK);
            if (wFlag)
                fprintf(logFile, "%s:  MPT Firmware Fault, code %04x\n",
                        adap->name, doorbell & MPI_DOORBELL_DATA_MASK);

            printf("\nHit Enter to continue ");

            getYesNoAnswer(0);

            printf("\n");
        }
        adap->restart_needed = TRUE;
    }

    while ((context = rl(REPLY_QUEUE)) != 0xffffffff)
    {
        if (context & MPI_CONTEXT_REPLY_A_BIT)
        {
            reply_ba = context << 1;
            reply = (MPIDefaultReply_t *)((U8 *)adap->shared + reply_ba - (U32)adap->shared_ba);
            if (mpt_handle_reply(adap, reply, reply_ba))
                wl(REPLY_QUEUE, reply_ba);
            continue;
        }
        if (context == PASS_THRU_CONTEXT)
        {
            if (adap->command_active == TRUE)
            {
                SCSIIOReply_t       *scsiRep = (SCSIIOReply_t *)adap->shared->data;

                adap->command_active = FALSE;
                bzero(scsiRep, sizeof(*scsiRep));
            }
            continue;
        }
        printf("%s: Invalid reply, Context = %08x\n", adap->name, reply);
    }
}


void
mpt2_interrupt(mpt_adap_t *adap)
{
    int                              index;
    Mpi2DefaultReplyDescriptor_t    *reply_desc;
    int                              reply_type;
    int                              count;

    index = adap->post_index;
    count = 0;
    while (TRUE)
    {
        reply_desc = &adap->shared->post_queue[index];
        reply_type = reply_desc->ReplyFlags & MPI2_RPY_DESCRIPT_FLAGS_TYPE_MASK;
        if (reply_type == MPI2_RPY_DESCRIPT_FLAGS_SCSI_IO_SUCCESS)
        {
            mpt2_handle_scsi_io_success(adap, reply_desc);
        }
        else if (reply_type == MPI2_RPY_DESCRIPT_FLAGS_ADDRESS_REPLY)
        {
            mpt2_handle_address_reply(adap, reply_desc);
        }
        else if (reply_type == MPI2_RPY_DESCRIPT_FLAGS_UNUSED)
        {
            break;
        }
        else
        {
            printf("%s: Invalid reply, ReplyType = %x\n", adap->name, reply_type);
        }
        reply_desc->ReplyFlags = MPI2_RPY_DESCRIPT_FLAGS_UNUSED;
        index++;
        if (index == REPLY_POST_SIZE)
            index = 0;
        count++;
    }

    if (count)
    {
        adap->post_index = index;
        wl2(REPLY_POST_HOST_INDEX, index);
    }
}


int
mpt_handle_reply(mpt_adap_t *adap, MPIDefaultReply_t *reply, U32 reply_ba)
{
    U32                          context;
    EventNotificationReply_t    *event;
    EventDataScsi_t             *ed_scsi;
    EventAckReply_t             *event_ack;
    int                          targ;
    int                          repost;

    repost = 1;
    context = get32(reply->MsgContext);

    if (get16(reply->IOCStatus) & MPI_IOCSTATUS_FLAG_LOG_INFO_AVAILABLE)
    {
        printf("%s: Function = %02x, IOCStatus = %04x, IOCLogInfo = %08x\n",
               adap->name, reply->Function, get16(reply->IOCStatus),
               get32(reply->IOCLogInfo));
    }

    if (context == PASS_THRU_CONTEXT)
    {
        if (adap->command_active == TRUE)
        {
            adap->command_active = FALSE;
            bcopy(reply, adap->shared->data, reply->MsgLength * 4);
        }
        return repost;
    }

    switch (reply->Function)
    {
    case MPI_FUNCTION_CONFIG:
        if (adap->config_active == TRUE)
        {
            adap->config_active = FALSE;
            bcopy(reply, adap->shared->data, reply->MsgLength * 4);
        }
        break;
    case MPI_FUNCTION_EVENT_NOTIFICATION:
        logMptCommandRep(adap->port, NULL, 0, reply, sizeof(*reply), 1);
        event = (EventNotificationReply_t *)reply;
        switch (get32(event->Event))
        {
        case MPI_EVENT_NONE:
            break;
        case MPI_EVENT_STATE_CHANGE:
            adap->restart_needed = TRUE;
            break;
        case MPI_EVENT_ON_BUS_TIMER_EXPIRED:
            ed_scsi = (EventDataScsi_t *)event->Data;
            targ = ed_scsi->TargetID;
            printf("%s: ON BUS TIMER EXPIRED for SCSI target %d\n", adap->name, targ);
            adap->bus_reset_needed = TRUE;
            break;
        }
        if (event->AckRequired)
        {
           _U32          t1;
           _U32          t2;
            EventAck_t  *event_ack;

            t1 = event->Event;
            t2 = event->EventContext;
            /*
             * Use the EventNotify reply as the EventAck command.
             */
            event_ack = (EventAck_t *)event;
            bzero(event_ack, sizeof(*event_ack));
            event_ack->Function = MPI_FUNCTION_EVENT_ACK;
            event_ack->MsgContext = set32(reply_ba);
            event_ack->Event = t1;
            event_ack->EventContext = t2;
            wl(REQUEST_QUEUE, get32(event_ack->MsgContext));
            /*
             * Signal that the reply should not be given back to
             * the IOC yet, since it's being used as the EventAck
             * command right now.
             */
            repost = 0;
        }
        break;
    case MPI_FUNCTION_EVENT_ACK:
        event_ack = (EventAckReply_t *)reply;
        wl(REPLY_QUEUE, get32(event_ack->MsgContext));
        break;
    default:
        printf("%s: Invalid reply, Function = %02x, IOCStatus = %04x\n",
               adap->name, reply->Function, get16(reply->IOCStatus));
#if 1
{
        int  i;
        U32 *p = (U32 *)reply;

        for (i = 0; i < reply->MsgLength; i++)
            printf("reply[%02x] = %08x\n", i*4, get32x(p[i]));
}
#endif
        break;
    }

    return repost;
}


void
mpt2_handle_scsi_io_success(mpt_adap_t *adap, Mpi2DefaultReplyDescriptor_t *reply_desc)
{
    Mpi2SCSIIOSuccessReplyDescriptor_t  *scsi_io_success;
    U32                                  context;

    scsi_io_success = (Mpi2SCSIIOSuccessReplyDescriptor_t *)reply_desc;
    context = adap->msg_context;
    if (get16(scsi_io_success->SMID) == 1)
        adap->msg_context = 0;

    if (context == PASS_THRU_CONTEXT)
    {
        if (adap->command_active == TRUE)
        {
            Mpi2SCSIIOReply_t       *scsiRep = (Mpi2SCSIIOReply_t *)adap->shared->data;

            adap->command_active = FALSE;
            bzero(scsiRep, sizeof(*scsiRep));
        }
        return;
    }
    printf("%s: Invalid SCSI IO SUCCESS reply, Context = %08x\n", adap->name, context);
}


void
mpt2_handle_address_reply(mpt_adap_t *adap, Mpi2DefaultReplyDescriptor_t *reply_desc)
{
    Mpi2AddressReplyDescriptor_t    *address_reply;
    MPI2DefaultReply_t              *reply;
    U32                              context;
    U32                              reply_ba;
    int                              index;

    address_reply = (Mpi2AddressReplyDescriptor_t *)reply_desc;

    reply_ba = get32(address_reply->ReplyFrameAddress);
    reply = (MPI2DefaultReply_t *)((U8 *)adap->shared + reply_ba - (U32)adap->shared_ba);
    context = adap->msg_context;
    if (get16(address_reply->SMID) == 1)
        adap->msg_context = 0;

    if (get16(reply->IOCStatus) & MPI2_IOCSTATUS_FLAG_LOG_INFO_AVAILABLE)
    {
        printf("%s: Function = %02x, IOCStatus = %04x, IOCLogInfo = %08x\n",
               adap->name, reply->Function, get16(reply->IOCStatus),
               get32(reply->IOCLogInfo));
    }

    if (context == PASS_THRU_CONTEXT)
    {
        if (adap->command_active == TRUE)
        {
            adap->command_active = FALSE;
            bcopy(reply, adap->shared->data, reply->MsgLength * 4);
        }
        goto repost;
    }

    switch (reply->Function)
    {
    case MPI2_FUNCTION_CONFIG:
        if (adap->config_active == TRUE)
        {
            adap->config_active = FALSE;
            bcopy(reply, adap->shared->data, reply->MsgLength * 4);
        }
        break;
    default:
        printf("%s: Invalid reply, Function = %02x, IOCStatus = %04x\n",
               adap->name, reply->Function, get16(reply->IOCStatus));
#if 1
{
        int  i;
        U32 *p = (U32 *)reply;

        for (i = 0; i < reply->MsgLength; i++)
            printf("reply[%02x] = %08x\n", i*4, get32x(p[i]));
}
#endif
        break;
    }

repost:
    index = adap->free_index;
    adap->shared->free_queue[index] = reply_ba;
    index++;
    if (index == REPLY_FREE_SIZE)
        index = 0;
    adap->free_index = index;
    wl2(REPLY_FREE_HOST_INDEX, index);
}


int mpt_fwdownloadboot(mpt_adap_t *adap)
{
    U16                      deviceId = adap->device_id;
    MpiFwHeader_t           *fwHeader;
    MpiExtImageHeader_t     *extImageHeader;
    U32                     *image;
    U8                      *nextImage;
    U32                      addr;
    U32                      size;
    U32                      next;
    U32                      data;
    U32                      resetAddr;
    U32                      resetData;
    int                      t;

    if (adap->fw_image == NULL)
    {
        char     name[256];
        int      n;

        if (adap->fw_image_asked == TRUE)
            return 0;

        adap->fw_image_asked = TRUE;

        printf("\nThe firmware on this %s appears broken!\n", adap->port->chipNameRev);
        printf("A valid firmware image is required to make this chip operational...\n\n");

        n = getFileName(name, sizeof name, stdin, "firmware", 99);
        if (n > 0)
        {
            if (readFile(name, &adap->fw_image, &adap->fw_image_size) != 1)
            {
                printf("\nThe firmware image could not be read\n");
                return 0;
            }
        }
        else
        {
            printf("\nThe chip cannot be made operational\n");
            return 0;
        }
    }

    if (deviceId == MPI_MANUFACTPAGE_DEVICEID_FC909 ||
        deviceId == MPI_MANUFACTPAGE_DEVICEID_FC919 ||
        deviceId == MPI_MANUFACTPAGE_DEVICEID_FC929)
    {
        return mpt_directdownload(adap);
    }

    wl(WRITE_SEQUENCE, 0);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_KEY_VALUE_MASK);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_1ST_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_2ND_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_3RD_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_4TH_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_5TH_KEY_VALUE);

    wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM      |
                   MPI_DIAG_PREVENT_IOC_BOOT |
                   MPI_DIAG_CLEAR_FLASH_BAD_SIG);
    DELAY(100);

    if (deviceId == MPI_MANUFACTPAGE_DEVICEID_FC949E)
    {
        wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM         |
                       MPI_DIAG_PREVENT_IOC_BOOT    |
                       MPI_DIAG_CLEAR_FLASH_BAD_SIG |
                       MPI_DIAG_RW_ENABLE);

        wlio(DIAG_RW_ADDRESS, 0x40100064);
        data = rlio(DIAG_RW_DATA);

        wlio(DIAG_RW_ADDRESS, 0x40100064);
        wlio(DIAG_RW_DATA, data | 0x20000);

        DELAY(100);
    }

    wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM      |
                   MPI_DIAG_PREVENT_IOC_BOOT |
                   MPI_DIAG_RESET_ADAPTER);
    DELAY(50000);

    wl(WRITE_SEQUENCE, 0);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_KEY_VALUE_MASK);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_1ST_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_2ND_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_3RD_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_4TH_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_5TH_KEY_VALUE);

    wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM         |
                   MPI_DIAG_PREVENT_IOC_BOOT    |
                   MPI_DIAG_CLEAR_FLASH_BAD_SIG |
                   MPI_DIAG_RW_ENABLE);

    if (deviceId == MPI_MANUFACTPAGE_DEVICEID_FC949E)
    {
        wlio(DIAG_RW_ADDRESS, 0x40100064);
        wlio(DIAG_RW_DATA, data);
    }

    fwHeader = (MpiFwHeader_t *)adap->fw_image;

    image = (U32 *)fwHeader;

    addr = get32(fwHeader->LoadStartAddress);
    size = get32(fwHeader->ImageSize);

    next = get32(fwHeader->NextImageHeaderOffset);

    resetAddr = get32(fwHeader->IopResetRegAddr);
    resetData = get32(fwHeader->IopResetVectorValue);

    while (next != 0)
    {
        nextImage = (U8 *)fwHeader + next;

        extImageHeader = (MpiExtImageHeader_t *)nextImage;

        if (extImageHeader->ImageType == MPI_EXT_IMAGE_TYPE_BOOTLOADER)
        {
            printf("Found a BootLoader, using only that part of the image!\n");

            adap->bootloader = TRUE;

            image = (U32 *)extImageHeader;

            addr = get32(extImageHeader->LoadStartAddress);
            size = get32(extImageHeader->ImageSize);

            next = 0;

            resetData = addr + sizeof(MpiExtImageHeader_t);

            break;
        }

        next = get32(extImageHeader->NextImageHeaderOffset);
    }

    if (addr == get32(fwHeader->LoadStartAddress) &&
        size == get32(fwHeader->ImageSize))
    {
        next = get32(fwHeader->NextImageHeaderOffset);
    }

    while (1)
    {
        wlio(DIAG_RW_ADDRESS, addr);

        while (size)
        {
            data = *image++;

            wlio(DIAG_RW_DATA, data);

            size -= 4;
        }

        if (next == 0)
        {
            break;
        }

        nextImage = (U8 *)fwHeader + next;

        extImageHeader = (MpiExtImageHeader_t *)nextImage;

        image = (U32 *)extImageHeader;

        addr = get32(extImageHeader->LoadStartAddress);
        size = get32(extImageHeader->ImageSize);

        next = get32(extImageHeader->NextImageHeaderOffset);
    }

    wlio(DIAG_RW_ADDRESS, resetAddr);
    wlio(DIAG_RW_DATA, resetData);

    if (deviceId == MPI_MANUFACTPAGE_DEVID_53C1030)
    {
        wlio(DIAG_RW_ADDRESS, 0x3f000000);
        data = rlio(DIAG_RW_DATA);

        data |= 0x40000000;

        wlio(DIAG_RW_ADDRESS, 0x3f000000);
        wlio(DIAG_RW_DATA, data);
    }

    if (deviceId == MPI_MANUFACTPAGE_DEVICEID_FC919X ||
        deviceId == MPI_MANUFACTPAGE_DEVICEID_FC929X)
    {
        wlio(DIAG_RW_ADDRESS, 0x3e000000);
        data = rlio(DIAG_RW_DATA);
    }

    if (deviceId == MPI_MANUFACTPAGE_DEVICEID_FC939X ||
        deviceId == MPI_MANUFACTPAGE_DEVICEID_FC949X ||
        deviceId == MPI_MANUFACTPAGE_DEVICEID_FC949E)
    {
        wlio(DIAG_RW_ADDRESS, 0x3d000000);
        data = rlio(DIAG_RW_DATA);
    }

    if (adap->bootloader)
    {
        wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM |
                       MPI_DIAG_CLEAR_FLASH_BAD_SIG);
        DELAY(100);

        wl(DIAGNOSTIC, 0);
        DELAY(100);

        sleep(1);
    }

    wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM |
                   MPI_DIAG_CLEAR_FLASH_BAD_SIG);
    DELAY(100);

    wl(DIAGNOSTIC, 0);
    DELAY(100);

    t = mpt_wait_for_ready(adap);

    if (wFlag)
        fprintf(logFile, "%s:  Firmware Download Boot:  %s\n",
                adap->name, t ? "PASS" : "FAIL");

    if (t)
        return 1;

    printf("\n%s: FWDownloadBoot failed!\n", adap->name);
    return 0;
}


int
mpt_directdownload(mpt_adap_t *adap)
{
    U16                      deviceId = adap->device_id;
    U32                      mask = adap->diagmem_size - 1;
    MpiFwHeader_t           *fwHeader;
    U32                     *image;
    U32                      addr;
    U32                      size;
    U32                      data;
    int                      t;

    wl(WRITE_SEQUENCE, 0);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_KEY_VALUE_MASK);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_1ST_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_2ND_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_3RD_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_4TH_KEY_VALUE);
    wl(WRITE_SEQUENCE, MPI_WRSEQ_5TH_KEY_VALUE);

    wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM);
    DELAY(100);

    /*  If FlashBadSig is set, the download doesn't seem to work if we reset
     *  the chip here.
     *
     *  If FlashBadSig isn't set, the download doesn't seem to work if we
     *  don't reset the chip here.
     */
    data = rl(DIAGNOSTIC);
    if (!(data & MPI_DIAG_FLASH_BAD_SIG))
    {
        wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM | MPI_DIAG_RESET_ADAPTER);
        DELAY(50000);

        wl(WRITE_SEQUENCE, 0);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_KEY_VALUE_MASK);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_1ST_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_2ND_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_3RD_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_4TH_KEY_VALUE);
        wl(WRITE_SEQUENCE, MPI_WRSEQ_5TH_KEY_VALUE);
    }

    wl(DIAGNOSTIC, MPI_DIAG_DISABLE_ARM | MPI_DIAG_MEM_ENABLE);

    addr = 0x50004;
    rldiag(addr, data);
    data &= ~0x80000000;
    wldiag(addr, data);

    if (deviceId == MPI_MANUFACTPAGE_DEVICEID_FC929)
    {
        addr = 0x51004;
        rldiag(addr, data);
        data &= ~0x80000000;
        wldiag(addr, data);
    }

    if (rl(DIAGNOSTIC) & MPI_DIAG_FLASH_BAD_SIG)
    {
        addr = 0x40000;
        rldiag(addr, data);
        data |= 0x40000000;
        wldiag(addr, data);
    }

    fwHeader = (MpiFwHeader_t *)adap->fw_image;

    image = (U32 *)fwHeader;

    addr = get32(fwHeader->LoadStartAddress);
    size = get32(fwHeader->ImageSize);

    while (size)
    {
        data = *image++;

        wldiag(addr, data);

        addr += 4;
        size -= 4;
    }

    addr = get32(fwHeader->IopResetRegAddr);
    data = get32(fwHeader->IopResetVectorValue);

    wldiag(addr, data);

    addr = 0x40000;
    rldiag(addr, data);
    data |= 0x40000000;
    wldiag(addr, data);

    wl(DIAGNOSTIC, 0);
    DELAY(100);

    t = mpt_wait_for_ready(adap);

    if (wFlag)
        fprintf(logFile, "%s:  Firmware Direct Download:  %s\n",
                adap->name, t ? "PASS" : "FAIL");

    if (t)
        return 1;

    printf("\n%s: DirectDownload failed!\n", adap->name);
    return 0;
}


/* vi: set sw=4 ts=4 sts=4 et :iv */
