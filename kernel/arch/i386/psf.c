extern char _binary_font_psf_start;

#include <stdbool.h>
#include <stdint.h>

static const uint16_t PSF1_FONT_MAGIC = 0x36;

static const uint8_t PSF1_MODE_512 = 0x01;
static const uint8_t PSF1_MODE_HAS_TAB = 0x02;
static const uint8_t PSF1_MODE_HAS_SEQ = 0x04;

typedef struct
{
    uint16_t magic;
    uint8_t font_mode;
    uint8_t character_size;
} PSF_Header;

PSF_Header* psf_get_header()
{
    return (PSF_Header*)&_binary_font_psf_start;
}

uint16_t* psf_setup_font(uint16_t* unicode)
{
    PSF_Header* font = (PSF_Header*)&_binary_font_psf_start;
    uint16_t* s = (uint16_t*)(&_binary_font_psf_start + (sizeof(PSF_Header) / sizeof(uint16_t)));

    if (!(font->font_mode & PSF1_MODE_HAS_TAB))
    {
        unicode = NULL;
        return s;
    }

    size_t len = (font->font_mode & PSF1_MODE_512) ? 512 : 256;
    uint16_t glyph = 0;
    bool ignore = false;
    while (glyph < len)
    {
        uint16_t uc = (uint16_t)(s[0]);
        if (uc == 0xFFFF)
        {
            glyph++;
            ignore = false;
        }
        else if (uc == 0xFFFE)
        {
            ignore = true;
        }
        else if (!ignore)
        {
            unicode[uc] = glyph;
        }
        s++;
    }
    return s;
}