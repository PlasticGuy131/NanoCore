#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include <keyboard.h>
#include <port.h>

void (*keypress_callback)(Keypress);

static const int scancode_alpha_codes[26] = {0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
                                             0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C};

static const int scancode_symbol_codes[15] = {0x0C, 0x0D, 0x1A, 0x1B, 0x27,
                                              0x28, 0x29, 0x56, 0x33, 0x34,
                                              0x35, 0x37, 0x4A, 0x4E, 0x53};

static const char symbol_code_chars[15] = {'-', '=', '[', ']', ';',
                                           '\'', '`', '\\', ',', '.',
                                           '/', '*', '-', '+', '.' };

static const int scancode_extra_codes[4] = { 0x0E, 0x0F, 0x1C, 0x39 };

static const char extra_code_chars[4] = {'\b', '\t', '\n', ' '};

static unsigned scancode_to_code(int scancode)
{
    if (scancode <= 0xA) { return scancode; }
    if (scancode == 0xB) { return 1; }
    for (int i = 0; i < 26; i++) { if (scancode == scancode_alpha_codes[i]) { return i + 11; } }
    for (int i = 0; i < 15; i++)
    {
        if (scancode == scancode_symbol_codes[i]) { return i + 37; }
        else if (scancode < scancode_symbol_codes[i]) { break; }
    }
    for (int i = 0; i < 4; i++)
    {
        if (scancode == scancode_extra_codes[i]) { return i + 52; }
        else if (scancode < scancode_extra_codes[i]) { break; }
    }

    return 0;
}

void keyboard_register_callback(void (*callback)(Keypress)) { keypress_callback = callback; }
void keyboard_clear_callback() { keypress_callback = 0; }

int keyboard_char_code(char ascii)
{
    if (isdigit(ascii)) { return ascii - '0' + 1; }
    if (isupper(ascii)) { return ascii - 54; }
    for (int i = 0; i < 15; i++) { if (ascii == symbol_code_chars[i]) { return i + 37; } }
    for (int i = 0; i < 4; i++) { if (ascii == scancode_extra_codes[i]) { return i + 52; } }
    
    return 0;
}

char keyboard_keypress_char(Keypress keypress)
{
    if (keypress.code == 0) { return '\0'; }
    if (keypress.code <= 10) { return keypress.code - 1 + '0'; }
    if (keypress.code <= 36) { return keypress.code + 54; }
    if (keypress.code <= 51) { return symbol_code_chars[keypress.code - 37]; }
    if (keypress.code <= 55) { return scancode_extra_codes[keypress.code - 37]; }
    return '\0';
}

void keyboard_read_key()
{
    int scancode = inb(PS2_DATA);
    Keypress keypress;
    keypress.flags = 0;
    if (!(scancode & 0x80)) { keypress.flags |= KEY_FLAG_PRESSED; }
    scancode &= 0x7F;
    
    keypress.code = scancode_to_code(scancode);

    if (keypress_callback) { keypress_callback(keypress); }
}