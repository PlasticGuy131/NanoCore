#include <stdint.h>

typedef struct
{
    uint32_t lower;
    uint32_t upper;
} MultibootMemory;

typedef struct
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
    uint8_t fram;
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

    uint32_t syms_1;
    uint32_t syms_2;
    uint32_t syms_3;

    MultibootMMAP mmap;

    MultibootDrives drives;

    uint32_t config_table;

    uint32_t boot_loader_name;

    uint32_t apm_table;

    MultibootVBETable vbe_table;

    MultibootFramebufferTable framebuffer_table;
} MultibootInfo;