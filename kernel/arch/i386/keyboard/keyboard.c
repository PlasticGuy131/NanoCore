#include <ctype.h>
#include <stdbool.h>

#include <keyboard.h>
#include <port.h>

void (*keypress_callback)(Keypress);

static const int convert[26] = {0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
                                0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14, 0x16, 0x27, 0x11, 0x2D, 0x15, 0x2C};

static bool scancode_pass_on(int scancode) { return (scancode <= 0x58); }

static unsigned scancode_to_code(int scancode)
{
    if (scancode <= 0xA) { return scancode; }
    if (scancode == 0xB) { return 1; }
    for (int i = 0; i < 26; i++) { if (scancode == convert[i]) { return i + 11; } }

    return 0;
}

void keyboard_register_callback(void (*callback)(Keypress)) { keypress_callback = callback; }
void keyboard_clear_callback() { keypress_callback = 0; }

int keyboard_ascii_code(char ascii)
{
    if (isdigit(ascii)) { return ascii - '0' + 1; }
    if (isupper(ascii)) { return ascii - 54; }
    return 0;
}

char keyboard_keypress_ascii(Keypress keypress)
{
    if (keypress.code <= 10) { return keypress.code - 1 + '0'; }
    if (keypress.code <= 36) { return keypress.code + 54; }
    return '\0';
}

void keyboard_read_key()
{
    int scancode = inb(PS2_DATA);
    if (scancode_pass_on(scancode))
    {
        Keypress keypress;
        keypress.code = scancode_to_code(scancode);
        keypress.flags = 0;

        if (keypress_callback) { keypress_callback(keypress); }
    }
}