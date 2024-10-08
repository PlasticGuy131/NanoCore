#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include <keyboard.h>
#include <port.h>

#define LEFT_CTRL 0x1D
#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define LEFT_ALT 0x38
#define CAPS_LOCK 0x3A
#define NUM_LOCK 0x45
#define SCROLL_LOCK 0x46

#define RIGHT_CONTROL 0xE01D

static const unsigned char NUMBER_CODE_SYMBOLS[10] = { ')', '!', '\"', 163 /* £ */, '$', '%', '^', '&', '*', '(' };

static const int SCANCODE_ALPHA_CODES[26] = { 0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
                                              0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C };

static const int SCANCODE_SYMBOL_CODES[16] = { 0x0C, 0x0D, 0x1A, 0x1B,
                                               0x27, 0x28, 0x29, 0x2B,
                                               0x33, 0x34, 0x35, 0x37,
                                               0x4A, 0x4E, 0x53, 0x56 };

static const char SYMBOL_CODE_CHARS[16] = { '-', '=', '[', ']',
                                            ';', '\'', '`', '#',
                                            ',', '.', '/', '*',
                                            '-', '+', '.', '\\' };

static const unsigned char SYMBOL_CODE_SHIFT_CHARS[16] = { '_', '+', '{', '}', ':', '@',
                                                           172 /* ¬ */, '~', '<', '>',
                                                           '?', '*', '-', '+', 127 /* del */, '|' };

static const int SCANCODE_EXTRA_CODES[4] = { 0x0E, 0x0F, 0x1C, 0x39 };

static const char EXTRA_CODE_CHARS[4] = { '\b', '\t', '\n', ' ' };

static const int SCANCODE_CONTROL_CODES[7] = { LEFT_CTRL, LEFT_SHIFT, RIGHT_SHIFT, LEFT_ALT, CAPS_LOCK, NUM_LOCK, SCROLL_LOCK };

static const int EXT_SCANCODE_EXTRA_CODES[2] = { 0xE01C, 0xE053 };

static const char EXT_EXTRA_CODE_CHARS[2] = { '\n', 127 /* del */ };

static const int EXT_SCANCODE_CONTROL_CODES[14] = { RIGHT_CONTROL, 0xE038, 0xE047, 0xE048,
                                                    0xE049, 0xE04B, 0xE04D, 0xE04F, 0xE050,
                                                    0xE051, 0xE052, 0xE05B, 0xE05C, 0xE05D };

static const int EXT_SCANCODE_ACPI_CODES[3] = { 0xE05E, 0xE05F, 0xE063 };

static const int EXT_SCANCODE_MULTI_CODES[18] = { 0xE010, 0xE019, 0xE020, 0xE021, 0xE022, 0xE024,
                                                  0xE02E, 0xE030, 0xE032, 0xE065, 0xE066, 0xE067,
                                                  0xE068, 0xE069, 0xE06A, 0xE06B, 0xE06C, 0xE06D };

static bool extended = false;

static int shifts = 0;
static int controls = 0;
static bool alt = false;
static bool caps_lock = false;
static bool num_lock = false;
static bool scroll_lock = false;

static void (*keypress_callback)(Keypress);

static unsigned scancode_to_code(int scancode)
{
    if (scancode == 0x1) { return KEYCODE_ESCAPE; }
    if (scancode <= 0xA) { return scancode; }
    if (scancode == 0xB) { return 1; }
    for (int i = 0; i < 26; i++) { if (scancode == SCANCODE_ALPHA_CODES[i]) { return i + 11; } }
    for (int i = 0; i < 16; i++)
    {
        if (scancode == SCANCODE_SYMBOL_CODES[i]) { return i + 37; }
        else if (scancode < SCANCODE_SYMBOL_CODES[i]) { break; }
    }
    for (int i = 0; i < 4; i++)
    {
        if (scancode == SCANCODE_EXTRA_CODES[i]) { return i + 53; }
        else if (scancode < SCANCODE_EXTRA_CODES[i]) { break; }
    }
    for (int i = 0; i < 8; i++)
    {
        if (scancode == SCANCODE_CONTROL_CODES[i]) { return i + 58; }
        else if (scancode < SCANCODE_CONTROL_CODES[i]) { break; }
    }
    if (scancode >= 0x3B && scancode <= 0x44) { return scancode + 6; }
    if (scancode >= 0x57 && scancode <= 0x58) { return scancode - 12; }

    if (scancode == 0xE035) { return KEYCODE_KEYPAD_SLASH; }
    for (int i = 0; i < 2; i++)
    {
        if (scancode == EXT_SCANCODE_EXTRA_CODES[i]) { return i + 78; }
        else if (scancode < EXT_SCANCODE_EXTRA_CODES[i]) { break; }
    }
    for (int i = 0; i < 14; i++)
    {
        if (scancode == EXT_SCANCODE_CONTROL_CODES[i]) { return i + 80; }
        else if (scancode < EXT_SCANCODE_CONTROL_CODES[i]) { break; }
    }
    for (int i = 0; i < 3; i++)
    {
        if (scancode == EXT_SCANCODE_ACPI_CODES[i]) { return i + 94; }
        else if (scancode < EXT_SCANCODE_ACPI_CODES[i]) { break; }
    }
    for (int i = 0; i < 18; i++)
    {
        if (scancode == EXT_SCANCODE_MULTI_CODES[i]) { return i + 97; }
        else if (scancode < EXT_SCANCODE_MULTI_CODES[i]) { break; }
    }


    return 0;
}

void keyboard_register_callback(void (*callback)(Keypress)) { keypress_callback = callback; }
void keyboard_clear_callback() { keypress_callback = 0; }

int keyboard_char_code(char ascii)
{
    if (isdigit(ascii)) { return ascii - '0' + 1; }
    if (isupper(ascii)) { return ascii - 54; }
    for (int i = 0; i < 16; i++) { if (ascii == SYMBOL_CODE_CHARS[i]) { return i + 37; } }
    for (int i = 0; i < 4; i++) { if (ascii == SCANCODE_EXTRA_CODES[i]) { return i + 53; } }
    if (ascii == 127) { return KEYCODE_DELETE; }

    return 0;
}

char keyboard_keypress_char(Keypress keypress)
{
    if (keypress.code == 0) { return '\0'; }
    if (keypress.code <= 10)
    {
        if (keypress.flags & KEY_FLAG_SHIFT) { return NUMBER_CODE_SYMBOLS[keypress.code - 1]; }
        return keypress.code - 1 + '0';
    }
    if (keypress.code <= 36)
    {
        char ch = keypress.code + 54;

        bool upper = false;
        if (keypress.flags & KEY_FLAG_CAPS_LOCK) { upper = true; }
        if (keypress.flags & KEY_FLAG_SHIFT) { upper = !upper; }
        return upper ? ch : tolower(ch);
    }
    if (keypress.code <= 52)
    {
        if (keypress.flags & KEY_FLAG_SHIFT) { return SYMBOL_CODE_SHIFT_CHARS[keypress.code - 37]; }
        return SYMBOL_CODE_CHARS[keypress.code - 37];
    }
    if (keypress.code <= 56) { return EXTRA_CODE_CHARS[keypress.code - 53]; }

    if (keypress.code == KEYCODE_KEYPAD_SLASH) { return '/'; }
    if (keypress.code >= 78 && keypress.code <= 79) { return EXT_EXTRA_CODE_CHARS[keypress.code - 78]; }

    return '\0';
}

void keyboard_read_key()
{
    int scancode = inb(PS2_DATA);
    if (scancode == 0xE0)
    {
        extended = true;
        return;
    }

    if (extended)
    {
        scancode |= 0xE000;
        extended = false;
    }

    if (keypress_callback)
    {
        Keypress keypress;
        keypress.flags = 0;
        if (scancode & 0x80)
        {
            scancode &= 0x7F;
            if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) { shifts--; }
            else if (scancode == LEFT_CTRL) { controls--; }
            else if (scancode == LEFT_ALT) { alt = false; }
        }
        else
        {
            keypress.flags |= KEY_FLAG_PRESSED;
            if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) { shifts++; }
            else if (scancode == LEFT_CTRL || scancode == RIGHT_CONTROL) { controls++; }
            else if (scancode == LEFT_ALT) { alt = true; }
            else if (scancode == CAPS_LOCK) { caps_lock = !caps_lock; }
            else if (scancode == NUM_LOCK) { num_lock = !num_lock; }
            else if (scancode == SCROLL_LOCK) { scroll_lock = !scroll_lock; }
        }

        keypress.code = scancode_to_code(scancode);

        if (shifts) { keypress.flags |= KEY_FLAG_SHIFT; }
        if (controls) { keypress.flags |= KEY_FLAG_CTRL; }
        if (alt) { keypress.flags |= KEY_FLAG_ALT; }
        if (caps_lock) { keypress.flags |= KEY_FLAG_CAPS_LOCK; }
        if (num_lock) { keypress.flags |= KEY_FLAG_NUM_LOCK; }
        if (scroll_lock) { keypress.flags |= KEY_FLAG_SCROLL_LOCK; }

        keypress_callback(keypress);
    }
}