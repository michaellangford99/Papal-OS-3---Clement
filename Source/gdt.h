#ifndef GDT_H
#define GDT_H

#define SEGMENT_DESCRIPTORS    3
#define SECTOR_CODE_DPL_0      0x9A
#define SECTOR_DATA_DPL_0      0x92
#define SECTOR_CODE_DPL_3      0x00 // not yet impl.
#define SECTOR_DATA_DPL_3      0x00 // not yet impl.
#define SECTOR_TSS_DPL_0       0x89

struct GDT_sector_descriptor
{
	uint32_t base;
	uint32_t limit;
	uint8_t type;
};

void gdt_init();
void encode_GDT_entry(uint8_t *target, struct GDT_sector_descriptor source);
extern void setGDT(uint32_t base, uint16_t size);
extern void reloadSegments();
#endif
