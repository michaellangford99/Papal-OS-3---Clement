#ifndef GDT_H
#define GDT_H

//constants for GDT
#define SEGMENT_DESCRIPTORS    6
#define SECTOR_CODE_DPL_0      0x9A
#define SECTOR_DATA_DPL_0      0x92
#define SECTOR_CODE_DPL_3      0xFA
#define SECTOR_DATA_DPL_3      0xF2
#define SECTOR_TSS_DPL_0       0x89

struct GDT_sector_descriptor
{
	uint32_t base;
	uint32_t limit;
	uint8_t type;
};

struct {
	int16_t	prev;
	int16_t	reserved;
	int32_t	esp0;
	int16_t	ss0;
	int16_t	reserved0;
	int32_t	esp1;
	int16_t	ss1;
	int16_t	reserved1;
	int32_t	esp2;
	int16_t	ss2;
	int16_t	reserved2;
	int32_t	cr3;
	int32_t	eip;
	int32_t	eflags;
	int32_t	eax;
	int32_t	ecx;
	int32_t	edx;
	int32_t	ebx;
	int32_t	esp;
	int32_t	ebp;
	int32_t	esi;
	int32_t	edi;
	int16_t	es;
	int16_t	reserved3;
	int16_t	cs;
	int16_t	reserved4;
	int16_t	ss;
	int16_t	reserved5;
	int16_t	ds;
	int16_t	reserved6;
	int16_t	fs;
	int16_t	reserved7;
	int16_t	gs;
	int16_t	reserved8;
	int16_t	ldt;
	int16_t	reserved9;
	int16_t	t;
	int16_t	iomap;
} __attribute__((packed)) TSS_descriptor;

void gdt_init();
void load_TSS(uint32_t* TSS, uint32_t TSS_size);
void encode_GDT_entry(uint8_t *target, struct GDT_sector_descriptor source);
extern void setGDT(uint32_t base, uint16_t size);
extern void reloadSegments();
#endif
