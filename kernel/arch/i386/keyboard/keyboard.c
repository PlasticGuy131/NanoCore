#include <stdbool.h>

#include <keyboard.h>
#include <port.h>

void (*keypress_callback)(Keypress);

static bool scancode_pass_on(int scancode) { return (scancode <= 0x58); }

static unsigned scancode_to_code(int scancode)
{
    if (scancode <= 0xA) { return scancode; }
    if (scancode == 0xB) { return 1; }
}

void keyboard_register_callback(void (*callback)(Keypress)) { keypress_callback = callback; }
void keyboard_clear_callback() { keypress_callback = 0; }

int keyboard_ascii_code(char ascii)
{
    if (isdigit(ascii)) { return ascii - '0' + 1; }
    if (isupper(ascii)) { return ascii - 54; }
}

char keyboard_keypress_ascii(Keypress keypress)
{
    if (keypress.code <= 10) { return keypress.code - 1 + '0'; }
    if (keypress.code <= 36) { return keypress.code + 54; }
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