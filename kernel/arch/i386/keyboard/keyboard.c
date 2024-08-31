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

static const int SCANCODE_EXTRA_CODES[4] = { 0x0E, 0x0F, 0x1C, 0x39 };

static const char EXTRA_CODE_CHARS[4] = { '\b', '\t', '\n', ' ' };

static const int SCANCODE_CONTROL_CODES[8] = { 0x01, LEFT_CTRL, LEFT_SHIFT, RIGHT_SHIFT, LEFT_ALT, CAPS_LOCK, NUM_LOCK, SCROLL_LOCK };

static const unsigned char NUMBER_CODE_SYMBOLS[10] = { ')', '!', '\"', 163 /* £ */, '$', '%', '^', '&', '*', '(' };

static const unsigned char SYMBOL_CODE_SSYMBOLS[16] = { '_', '+', '{', '}', ':', '@',
                                                        172 /* ¬ */, '~', '<', '>', 
                                                        '?', '*', '-', '+', '.', '|' };

static int shifts = 0;
static int controls = 0;
static bool alt = false;
static bool caps_lock = false;
static bool num_lock = false;
static bool scroll_lock = false;

static void (*keypress_callback)(Keypress);

static unsigned scancode_to_code(int scancode)
{
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
        if (scancode == SCANCODE_CONTROL_CODES[i]) { return i + 57; }
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
    for (int i = 0; i < 16; i++) { if (ascii == SYMBOL_CODE_CHARS[i]) { return i + 37; } }
    for (int i = 0; i < 4; i++) { if (ascii == SCANCODE_EXTRA_CODES[i]) { return i + 53; } }
    
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
        if (keypress.flags & KEY_FLAG_SHIFT) { return SYMBOL_CODE_SSYMBOLS[keypress.code - 37]; }
        return SYMBOL_CODE_CHARS[keypress.code - 37];
    }
    if (keypress.code <= 56) { return EXTRA_CODE_CHARS[keypress.code - 53]; }
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
        else if (scancode == CAPS_LOCK) { caps_lock = !caps_lock; }
        else if (scancode == NUM_LOCK) { num_lock = !num_lock; }
        else if (scancode == SCROLL_LOCK) { scroll_lock = !scroll_lock; }
    }
    else
    {
        scancode &= 0x7F;
        if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) { shifts--; }
        else if (scancode == LEFT_CTRL) { controls--; }
        else if (scancode == LEFT_ALT) { alt = false; }
    }

    if (shifts) { keypress.flags |= KEY_FLAG_SHIFT; }
    if (controls) { keypress.flags |= KEY_FLAG_CTRL; }
    if (alt) { keypress.flags |= KEY_FLAG_ALT; }
    if (caps_lock) { keypress.flags |= KEY_FLAG_CAPS_LOCK; }
    if (num_lock) { keypress.flags |= KEY_FLAG_NUM_LOCK; }
    if (scroll_lock) { keypress.flags |= KEY_FLAG_SCROLL_LOCK; }

    keypress.code = scancode_to_code(scancode);

    if (keypress_callback) { keypress_callback(keypress); }
}