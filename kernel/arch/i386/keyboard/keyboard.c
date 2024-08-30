#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include <keyboard.h>
#include <port.h>

#define LEFT_SHIFT 0x1D
#define LEFT_CTRL 0x2A
#define RIGHT_SHIFT 0x36
#define LEFT_ALT 0x38
#define CAPS_LOCK 0x3A

static const int SCANCODE_ALPHA_CODES[26] = { 0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
                                             0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C};

static const int SCANCODE_SYMBOL_CODES[15] = {0x0C, 0x0D, 0x1A, 0x1B, 0x27,
                                              0x28, 0x29, 0x33, 0x34, 0x35,
                                              0x37, 0x4A, 0x4E, 0x53, 0x56};

static const char SYMBOL_CODE_CHARS[15] = {'-', '=', '[', ']', ';',
                                           '\'', '`', ',', '.', '/',
                                           '*', '-', '+', '.', '\\'};

static const int SCANCODE_EXTRA_CODES[4] = {0x0E, 0x0F, 0x1C, 0x39};

static const char EXTRA_CODE_CHARS[4] = {'\b', '\t', '\n', ' '};

static const int SCANCODE_CONTROL_CODES[5] = { LEFT_SHIFT, LEFT_CTRL, RIGHT_SHIFT, LEFT_ALT, CAPS_LOCK };

static int shifts = 0;
static int controls = 0;
static bool caps_lock = false;
static bool alt = false;

static void (*keypress_callback)(Keypress);

static unsigned scancode_to_code(int scancode)
{
    if (scancode <= 0xA) { return scancode; }
    if (scancode == 0xB) { return 1; }
    for (int i = 0; i < 26; i++) { if (scancode == SCANCODE_ALPHA_CODES[i]) { return i + 11; } }
    for (int i = 0; i < 15; i++)
    {
        if (scancode == SCANCODE_SYMBOL_CODES[i]) { return i + 37; }
        else if (scancode < SCANCODE_SYMBOL_CODES[i]) { break; }
    }
    for (int i = 0; i < 4; i++)
    {
        if (scancode == SCANCODE_EXTRA_CODES[i]) { return i + 52; }
        else if (scancode < SCANCODE_EXTRA_CODES[i]) { break; }
    }
    for (int i = 0; i < 5; i++)
    {
        if (scancode == SCANCODE_CONTROL_CODES[i]) { return i + 56; }
        else if (scancode < SCANCODE_CONTROL_CODES[i]) { break; }
    }

    return 0;
}

void keyboard_register_callback(void (*callback)(Keypress)) { keypress_callback = callback; }
void keyboard_clear_callback() { keypress_callback = 0; }

int keyboard_char_code(char ascii)
{
    if (isdigit(ascii)) { return ascii - '0' + 1; }
    if (isupper(ascii)) { return ascii - 54; }
    for (int i = 0; i < 15; i++) { if (ascii == SYMBOL_CODE_CHARS[i]) { return i + 37; } }
    for (int i = 0; i < 4; i++) { if (ascii == SCANCODE_EXTRA_CODES[i]) { return i + 52; } }
    
    return 0;
}

char keyboard_keypress_char(Keypress keypress)
{
    if (keypress.code == 0) { return '\0'; }
    if (keypress.code <= 10) { return keypress.code - 1 + '0'; }
    if (keypress.code <= 36) { return keypress.code + 54; }
    if (keypress.code <= 51) { return SYMBOL_CODE_CHARS[keypress.code - 37]; }
    if (keypress.code <= 55) { return EXTRA_CODE_CHARS[keypress.code - 52]; }
    return '\0';
}

void keyboard_read_key()
{
    int scancode = inb(PS2_DATA);
    Keypress keypress;
    keypress.flags = 0;
    if (!(scancode & 0x80))
    {
        keypress.flags |= KEY_FLAG_PRESSED;
        if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) { shifts++; }
        else if (scancode == LEFT_CTRL) { controls++; }
        else if (scancode == LEFT_ALT) { alt = true; }
        else if (scancode == CAPS_LOCK) { caps_lock = true; }
    }
    else
    {
        if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) { shifts--; }
        else if (scancode == LEFT_CTRL) { controls--; }
        else if (scancode == LEFT_ALT) { alt = false; }
        else if (scancode == CAPS_LOCK) { caps_lock = false; }
    }

    if (shifts) { keypress.flags |= KEY_FLAG_SHIFT; }
    if (controls) { keypress.flags |= KEY_FLAG_CTRL; }
    if (alt) { keypress.flags |= KEY_FLAG_ALT; }
    if (alt) { keypress.flags |= KEY_FLAG_CAPS_LOCK; }

    scancode &= 0x7F;
    
    keypress.code = scancode_to_code(scancode);

    if (keypress_callback) { keypress_callback(keypress); }
}