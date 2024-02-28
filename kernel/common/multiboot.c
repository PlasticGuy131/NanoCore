#include <stdint.h>

typedef struct
{
    uint32_t lower;
    uint32_t upper;
} MultibootMemory;

/*typedef struct
{
    uint32_t count;
    uint32_t addr;
} MultibootModules;

typedef struct
{
    uint32_t length;
    uint32_t addr;
} MultibootMMAP;

typedef struct
{
    uint32_t length;
    uint32_t addr;
} MultibootDrives;

typedef struct
{
    uint32_t control_info;
    uint32_t mode_info;
    uint16_t mode;
    uint16_t interface_seg;
    uint16_t interface_off;
    uint16_t interface_len;
} MultibootVBETable;

typedef struct
{
    uint32_t addr1;
    uint32_t addr2;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type;
    uint8_t colour_type_1;
    uint8_t colour_type_2;
    uint8_t colour_type_3;
    uint8_t colour_type_4;
    uint8_t colour_type_5;
    uint8_t colour_type_6;
} MultibootFramebufferTable;

typedef struct
{
    uint32_t flags;

    MultibootMemory memory;

    uint32_t boot_device;

    uint32_t cmdline;

    MultibootModules mods;

    uint32_t syms_1;
    uint32_t syms_2;
    uint32_t syms_3;
    uint32_t syms_4;

    MultibootMMAP mmap;

    MultibootDrives drives;

    uint32_t config_table;

    uint32_t boot_loader_name;

    uint32_t apm_table;

    MultibootVBETable vbe_table;

    MultibootFramebufferTable framebuffer_table;
} MultibootInfo;*/

typedef struct
{
  multiboot_uint32_t tabsize;
  multiboot_uint32_t strsize;
  multiboot_uint32_t addr;
  multiboot_uint32_t reserved;
} multiboot_aout_symbol_table;

typedef struct
{
  multiboot_uint32_t num;
  multiboot_uint32_t size;
  multiboot_uint32_t addr;
  multiboot_uint32_t shndx;
} multiboot_elf_section_header_table;

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
        multiboot_aout_symbol_table_t aout_sym;
        multiboot_elf_section_header_table_t elf_sec;
    } u;

    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;


    multiboot_uint32_t boot_loader_name;

    multiboot_uint32_t apm_table;

    multiboot_uint32_t vbe_control_info;
    multiboot_uint32_t vbe_mode_info;
    multiboot_uint16_t vbe_mode;
    multiboot_uint16_t vbe_interface_seg;
    multiboot_uint16_t vbe_interface_off;
    multiboot_uint16_t vbe_interface_len;

    multiboot_uint64_t framebuffer_addr;
    multiboot_uint32_t framebuffer_pitch;
    multiboot_uint32_t framebuffer_width;
    multiboot_uint32_t framebuffer_height;
    multiboot_uint8_t framebuffer_bpp;
    multiboot_uint8_t framebuffer_type;
    union
    {
        struct
        {
        multiboot_uint32_t framebuffer_palette_addr;
        multiboot_uint16_t framebuffer_palette_num_colors;
        };
        struct
        {
        multiboot_uint8_t framebuffer_red_field_position;
        multiboot_uint8_t framebuffer_red_mask_size;
        multiboot_uint8_t framebuffer_green_field_position;
        multiboot_uint8_t framebuffer_green_mask_size;
        multiboot_uint8_t framebuffer_blue_field_position;
        multiboot_uint8_t framebuffer_blue_mask_size;
        };
    };
} multiboot_info;