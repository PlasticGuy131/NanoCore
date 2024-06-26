#ifndef MULTIBOOT_H_INCLUDED
#define MULTIBOOT_H_INCLUDED 1

#include <stdint.h>

#define MULTIBOOT_FLAG_MMAP (1 << 6)
#define MULTIBOOT_FLAG_FRAMEBUFFER (1 << 12)

enum Multiboot_Framebuffer_Type
{
    TYPE_INDEXED = 0,
    TYPE_RGB = 1,
    TYPE_EGA = 2,
};

typedef struct
{
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
  uint32_t reserved;
} Multiboot_AOUT_Symbol_Table;

typedef struct
{
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
} Multiboot_ELF_Section_Header_Table;

typedef struct
{
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;
    uint32_t cmdline;

    uint32_t mods_count;
    uint32_t mods_addr;

    union
    {
        Multiboot_AOUT_Symbol_Table aout_sym;
        Multiboot_ELF_Section_Header_Table elf_sec;
    } u;

    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;

    uint32_t boot_loader_name;

    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    union
    {
        struct
        {
            uint32_t framebuffer_palette_addr;
            uint16_t framebuffer_palette_num_colors;
        };
        struct
        {
            uint8_t framebuffer_red_field_position;
            uint8_t framebuffer_red_mask_size;
            uint8_t framebuffer_green_field_position;
            uint8_t framebuffer_green_mask_size;
            uint8_t framebuffer_blue_field_position;
            uint8_t framebuffer_blue_mask_size;
        };
    };
} Multiboot_Info;

static const uint32_t MULTIBOOT_MAGIC = 0x2BADB002;

#endif