#include <system.h>

struct GDT_sector_descriptor GDT_sect_desc[SEGMENT_DESCRIPTORS];

uint8_t GDT_table_entries[SEGMENT_DESCRIPTORS * 8];
/*
GDT Sectors:

   0   , 0          , 0      null descriptor
   0   , 0xffffffff , 0x9a   kernel code (DPL_0)
   0   , 0xffffffff , 0x92   kernel data (DPL_0)
   0   , 0xffffffff , 0xFA   user   code (DPL_3)
   0   , 0xffffffff , 0xF2   user   data (DPL_3)
   &TSS, sizeof(TSS), 0x89   -task state segment  - not impl. yet


   TODO: add user code & data, add TSS segment.
   as of 5/6/16 DPL 3 and TSS segments are not implemented.
*/

void gdt_init()
{
	//supposedly disables interrupts...
	interrupt_block();
	//check if they are actually disabled...
	if (are_interrupts_enabled())
		printf("ERROR: interrupts are enabled!");

	//set up our GDT sector desctriptors...
	GDT_sect_desc[0].base =0;
	GDT_sect_desc[0].limit=0;
	GDT_sect_desc[0].type =0;

	GDT_sect_desc[1].base =0;
	GDT_sect_desc[1].limit=0xffffffff;
	GDT_sect_desc[1].type =SECTOR_CODE_DPL_0;

	GDT_sect_desc[2].base =0;
	GDT_sect_desc[2].limit=0xffffffff;
	GDT_sect_desc[2].type =SECTOR_DATA_DPL_0;

	GDT_sect_desc[3].base =0;
	GDT_sect_desc[3].limit=0xffffffff;
	GDT_sect_desc[3].type =SECTOR_CODE_DPL_3;

	GDT_sect_desc[4].base =0;
	GDT_sect_desc[4].limit=0xffffffff;
	GDT_sect_desc[4].type =SECTOR_DATA_DPL_3;

	GDT_sect_desc[5].base =0;
	GDT_sect_desc[5].limit=0;
	GDT_sect_desc[5].type =SECTOR_TSS_DPL_3;

	//zero our GDT descriptors (the actual descriptors, not the structs describing them)...
	for (int i = 0; i < SEGMENT_DESCRIPTORS * 8; i++) {
		GDT_table_entries[i]=0;
	}

	//encode segment table for GDT...
	encode_GDT_entry(&GDT_table_entries[0 *8], GDT_sect_desc[0]);
	encode_GDT_entry(&GDT_table_entries[1 *8], GDT_sect_desc[1]);
	encode_GDT_entry(&GDT_table_entries[2 *8], GDT_sect_desc[2]);
	encode_GDT_entry(&GDT_table_entries[3 *8], GDT_sect_desc[3]);
	encode_GDT_entry(&GDT_table_entries[4 *8], GDT_sect_desc[4]);
	encode_GDT_entry(&GDT_table_entries[5 *8], GDT_sect_desc[5]);

	//tell CPU where our table is stored (in assembly... )
	setGDT((uint32_t) &GDT_table_entries, sizeof(GDT_table_entries)-1);
	//load segment descr. into actual segments (in assembly...)
	reloadSegments();

	printf("gdt: ready\n");
}

void load_TSS(uint32_t* TSS, uint32_t TSS_size)
{

	GDT_sect_desc[5].base = (uint32_t)TSS;
	GDT_sect_desc[5].limit=TSS_size;
	GDT_sect_desc[5].type =SECTOR_TSS_DPL_3;
	
	GDT_table_entries[5]=0;

	encode_GDT_entry(&GDT_table_entries[5 *8], GDT_sect_desc[5]);

	//tell CPU where our table is stored (in assembly... )
	setGDT((uint32_t) &GDT_table_entries, sizeof(GDT_table_entries)-1);
	//load segment descr. into actual segments (in assembly...)
	reloadSegments();

	setTSS();
}

void encode_GDT_entry(uint8_t *target, struct GDT_sector_descriptor source)
{
	//check the limit to make sure that it can be encoded
	if ((source.limit > 65536) && (source.limit & 0xFFF) != 0xFFF) {
		printf("\nERROR: GDT sector descriptor limit value is invalid!\n");
	}
	if (source.limit > 65536) {
		//adjust granularity if required
		source.limit = source.limit >> 12;
		target[6] = 0xC0;
	} else {
		target[6] = 0x40;
	}

	//encode the limit
	target[0] = source.limit & 0xFF;
	target[1] = (source.limit >> 8) & 0xFF;
	target[6] |= (source.limit >> 16) & 0xF;

	//encode the base
	target[2] = (source.base)       & 0xFF;
	target[3] = (source.base >> 8)  & 0xFF;
	target[4] = (source.base >> 16) & 0xFF;
	target[7] = (source.base >> 24) & 0xff;

	//and... type
	target[5] = source.type;
}
