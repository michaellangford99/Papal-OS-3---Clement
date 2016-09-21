#include "system.h"
#include "FS/fs_types.h"

const int ata_base[4] = {ATA_BASE0,ATA_BASE0,ATA_BASE1,ATA_BASE1};
int ata_interrupt_active = 0;

void ata_interrupt(struct x86_registers *regs)
{
	ata_interrupt_active=1;
	
	#ifdef DEBUG_ATA
		printf("gs        %d\n", regs->gs);
		printf("fs        %d\n", regs->fs);
		printf("es        %d\n", regs->es);
		printf("ds        %d\n", regs->ds);
		
		printf("edi       %d\n", regs->edi);
		printf("esi       %d\n", regs->esi);
		printf("ebp       %d\n", regs->ebp);
		printf("esp       %d\n", regs->esp);
		printf("ebx       %d\n", regs->ebx);
		printf("edx       %d\n", regs->edx);
		printf("ecx       %d\n", regs->ecx);
		printf("eax       %d\n", regs->eax);
		
		printf("int_no    %d\n", regs->int_no);
		printf("err_code  %d\n", regs->err_code);
		
		printf("eip       %d\n", regs->eip);
		printf("cs        %d\n", regs->cs);
		printf("eflags    %d\n", regs->eflags);
		printf("useresp   %d\n", regs->useresp);
		printf("ss        %d\n", regs->ss);
	#endif
}

void ata_reset( int id )
{
	
	outb(ATA_CONTROL_RESET,ata_base[id]+ATA_CONTROL);
	clock_wait(1);
	outb(0,ata_base[id]+ATA_CONTROL);
	clock_wait(1);
}

int ata_wait( int id, int mask, int state )
{
	clock_t start, elapsed;
	int t;

	start = clock_read();

	while(1) {
		t = inb(ata_base[id]+ATA_STATUS);
		if((t&mask)==state) {
			return 1;
		}
		if(t&ATA_STATUS_ERR) {
			printf("....ata: error\n");
			ata_reset(id);
			return 0;
		}
		elapsed = clock_diff(start,clock_read());
		if(elapsed.seconds>ATA_TIMEOUT) {
			printf("....ata: timeout\n");
			ata_reset(id);
			return 0;
		}
		clock_wait(0);
	}
}

void ata_pio_read( int id, void *buffer, int size )
{
	uint16_t *wbuffer = (uint16_t*)buffer;
	while(size>0) {
		*wbuffer = inw(ata_base[id]+ATA_DATA);
		wbuffer++;
		size-=2;
	}
}

void ata_pio_write( int id, const void *buffer, int size )
{
	uint16_t *wbuffer = (uint16_t*)buffer;
	while(size>0) {
		outw(*wbuffer,ata_base[id]+ATA_DATA);
		wbuffer++;
		size-=2;
	}
}

int ata_begin( int id, int command, int nblocks, int offset )
{
	int base = ata_base[id];
	int sector, clow, chigh, flags;

	// enable error correction and linear addressing
	flags = ATA_FLAGS_ECC|ATA_FLAGS_LBA|ATA_FLAGS_SEC;

	// turn on the slave bit for odd-numbered drives
	if(id%2) flags |= ATA_FLAGS_SLV;

	// slice up the linear address in order to fit in the arguments
	sector  = (offset>> 0) & 0xff;
	clow    = (offset>> 8) & 0xff;
	chigh   = (offset>>16) & 0xff;
	flags  |= (offset>>24) & 0x0f;

	// wait for the disk to calm down
	if(!ata_wait(id,ATA_STATUS_BSY,0)) return 0;

	// get the attention of the proper disk
	outb(flags,base+ATA_FDH);

	// wait again for the disk to indicate ready
	// special case: ATAPI identification does not raise RDY flag

	int ready;
	if(command==ATAPI_COMMAND_IDENTIFY) {
		ready = ata_wait(id,ATA_STATUS_BSY,0);
	} else {
		ready = ata_wait(id,ATA_STATUS_BSY|ATA_STATUS_RDY,ATA_STATUS_RDY);
	}

	if(!ready) return 0;

	// send the arguments
	outb(0,base+ATA_CONTROL);
	outb(nblocks,base+ATA_COUNT);
	outb(sector,base+ATA_SECTOR);
	outb(clow,base+ATA_CYL_LO);
	outb(chigh,base+ATA_CYL_HI);
	outb(flags,base+ATA_FDH);

	// execute the command
	outb(command,base+ATA_COMMAND);

	return 1;
}

int ata_read_unlocked( int id, void *buffer, int nblocks, int offset )
{
	int i;
	if(!ata_begin(id,ATA_COMMAND_READ,nblocks,offset)) return 0;
	for(i=0;i<nblocks;i++) {
		if(!ata_wait(id,ATA_STATUS_DRQ,ATA_STATUS_DRQ)) return 0;
		ata_pio_read(id,buffer,ATA_BLOCKSIZE);
		buffer = ((char*)buffer)+ATA_BLOCKSIZE;
		offset++;
	}
	if(!ata_wait(id,ATA_STATUS_BSY,0)) return 0;
	return nblocks;
}

int ata_read( int id, void *buffer, int nblocks, int offset )
{
	int result;
	
	result = ata_read_unlocked(id,buffer,nblocks,offset);
	
	return result;
}

int atapi_begin( int id, void *data, int length )
{
	int base = ata_base[id];
	int flags;

	// enable error correction and linear addressing
	flags = ATA_FLAGS_ECC|ATA_FLAGS_LBA|ATA_FLAGS_SEC;

	// turn on the slave bit for odd-numbered drives
	if(id%2) flags |= ATA_FLAGS_SLV;

	// wait for the disk to calm down
	if(!ata_wait(id,ATA_STATUS_BSY,0)) return 0;

	// get the attention of the proper disk
	outb(flags,base+ATA_FDH);

	// wait again for the disk to indicate ready
	if(!ata_wait(id,ATA_STATUS_BSY,0)) return 0;

	// send the arguments
	outb(0,base+ATAPI_FEATURE);
	outb(0,base+ATAPI_IRR);
	outb(0,base+ATAPI_SAMTAG);
	outb(length&0xff,base+ATAPI_COUNT_LO);
	outb(length>>8,base+ATAPI_COUNT_HI);

	// execute the command
	outb(ATAPI_COMMAND_PACKET,base+ATA_COMMAND);

	// wait for ready
	if(!ata_wait(id,ATA_STATUS_BSY|ATA_STATUS_DRQ,ATA_STATUS_DRQ)) { }

	// send the ATAPI packet
	ata_pio_write(id,data,length);

	return 1;
}

int atapi_read_unlocked( int id, void *buffer, int nblocks, int offset )
{
	uint8_t packet[12];
	int length=sizeof(packet);
	int i;

	packet[0] = SCSI_READ10;
	packet[1] = 0;
	packet[2] = offset>>24;
	packet[3] = offset>>16;
	packet[4] = offset>>8;
	packet[5] = offset>>0;
	packet[6] = 0;
	packet[7] = nblocks>>8;
	packet[8] = nblocks>>0;
	packet[9] = 0;
	packet[10] = 0;
	packet[11] = 0;

	if(!atapi_begin(id,packet,length)) return 0;


	for(i=0;i<nblocks;i++) {
		if(!ata_wait(id,ATA_STATUS_DRQ,ATA_STATUS_DRQ)) return 0;
		ata_pio_read(id,buffer,ATAPI_BLOCKSIZE);
		buffer = ((char*)buffer)+ATAPI_BLOCKSIZE;
		offset++;
	}

	return 1;
}

int atapi_read( int id, void *buffer, int nblocks, int offset )
{
	int result;
	
	result = atapi_read_unlocked(id,buffer,nblocks,offset);
	
	return result;
}

int ata_write_unlocked( int id, const void *buffer, int nblocks, int offset )
{
	int i;
	if(!ata_begin(id,ATA_COMMAND_WRITE,nblocks,offset)) return 0;
	for(i=0;i<nblocks;i++) {
		if(!ata_wait(id,ATA_STATUS_DRQ,ATA_STATUS_DRQ)) return 0;
		ata_pio_write(id,buffer,ATA_BLOCKSIZE);
		buffer = ((char*)buffer)+ATA_BLOCKSIZE;
		offset++;
	}
	if(!ata_wait(id,ATA_STATUS_BSY,0)) return 0;
	return nblocks;
}

int ata_write( int id, void *buffer, int nblocks, int offset )
{
	int result;
	
	result = ata_write_unlocked(id,buffer,nblocks,offset);
	
	return result;
}

/*
ata_probe sends an IDENTIFY DEVICE command to the device.
If a device is connected, it will respond with 512 bytes
of identifying data, described on page 48 of the ATA-3 standard.
If no response comes within the timeout window, we assume
the the device is simply not connected.
*/

int ata_identify( int id, int command, void *buffer )
{
	if(!ata_begin(id,command,0,0)) return 0;
	if(!ata_wait(id,ATA_STATUS_DRQ,ATA_STATUS_DRQ)) return 0;
	ata_pio_read(id,buffer,512);
	return 1;
}

int ata_probe( int id, int *nblocks, int *blocksize, char *name )
{
		
	uint16_t buffer[256];
	char *cbuffer = (char*)buffer;

	/*
	First check for 0xff in the controller status register,
	which would indicate that there is nothing attached.
	*/

	uint8_t t = inb(ata_base[id]+ATA_STATUS);
	if(t==0xff) {
		printf("....ata unit %d: nothing attached\n",id);
		return 0;
	}
	/* Now reset the unit to check for register signatures. */
	ata_reset(id);

	/* Clear the buffer to receive the identify data. */
	memset(cbuffer,0,512);

	if(ata_identify(id,ATA_COMMAND_IDENTIFY,cbuffer)) {

		*nblocks = (buffer[1]*buffer[3]*buffer[6]);
		*blocksize = 512;

 	} else if(ata_identify(id,ATAPI_COMMAND_IDENTIFY,cbuffer)) {

		// XXX use SCSI sense to get media size
		*nblocks = 337920;
		*blocksize = 2048;

	} else {
		printf("....ata u%d: identify command failed\n",id);
		return 0;
	}

	/* Now byte-swap the data so as the generate byte-ordered strings */
	uint32_t i;
	for(i=0;i<512;i+=2) {
		t = cbuffer[i];
		cbuffer[i] = cbuffer[i+1];
		cbuffer[i+1] = t;
	}
	cbuffer[256]=0;

	/* Vendor supplied name is at byte 54 */

	strcpy(name,&cbuffer[54]);
	name[40] = 0;

	printf("....ata u%d: %s %d MB %s\n",
		id,
		(*blocksize)==512 ? "ata disk" : "atapi cdrom",
		(*nblocks)*(*blocksize)/1024/1024 + 1,
		name);

	return 1;
}

int ata_probe_silent( int id, int *nblocks, int *blocksize, char *name, int *device_type, int *capacity) {
		
	uint16_t buffer[256];
	char *cbuffer = (char*)buffer;

	/*
	First check for 0xff in the controller status register,
	which would indicate that there is nothing attached.
	*/

	uint8_t t = inb(ata_base[id]+ATA_STATUS);
	if(t==0xff) {
		return 0;
	}
	/* Now reset the unit to check for register signatures. */
	ata_reset(id);

	/* Clear the buffer to receive the identify data. */
	memset(cbuffer,0,512);

	if(ata_identify(id,ATA_COMMAND_IDENTIFY,cbuffer)) {

		*nblocks = (buffer[1]*buffer[3]*buffer[6]);
		*blocksize = 512;

 	} else if(ata_identify(id,ATAPI_COMMAND_IDENTIFY,cbuffer)) {

		// XXX use SCSI sense to get media size
		*nblocks = 337920;
		*blocksize = 2048;

	} else {
		return 0;
	}

	/* Now byte-swap the data so as the generate byte-ordered strings */
	uint32_t i;
	for(i=0;i<512;i+=2) {
		t = cbuffer[i];
		cbuffer[i] = cbuffer[i+1];
		cbuffer[i+1] = t;
	}
	cbuffer[256]=0;

	/* Vendor supplied name is at byte 54 */

	strcpy(name,&cbuffer[54]);
	name[40] = 0;

	(*blocksize==2048) ? (*device_type = ATA_HDD) : (*device_type = ATAPI_CDROM);
	*capacity = (*nblocks)*(*blocksize)/1024/1024 + 1;

	return 1;
}

void ata_init()
{
	int i;
	int nblocks;
	int blocksize=0;
	char longname[256];

	irq_set_handler(ATA_IRQ0,ata_interrupt);
	irq_set_handler(ATA_IRQ1,ata_interrupt);

	for(i=0;i<4;i++) {
		ata_probe(i,&nblocks,&blocksize,longname);
	}
	
	printf("ata: ready\n");
}

