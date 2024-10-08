#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED 1

#include <keycode.h>

#define KEY_FLAG_PRESSED 1
#define KEY_FLAG_SHIFT (1 << 1)
#define KEY_FLAG_CAPS_LOCK (1 << 2)
#define KEY_FLAG_CTRL (1 << 3)
#define KEY_FLAG_ALT (1 << 4)
#define KEY_FLAG_NUM_LOCK (1 << 5)
#define KEY_FLAG_SCROLL_LOCK (1 << 6)

typedef struct
{
    unsigned code;
    char flags;
} Keypress;

/// @brief Reads a keypress from the keyboard, and calls the registered callback with it
void keyboard_read_key();

/// @brief Registers a callback which is called on every keypress
/// @param callback Callback function
void keyboard_register_callback(void (*callback)(Keypress));

/// @brief Clears current keypress callback
void keyboard_clear_callback();

/// @brief Converts an ascii character to a keyboard code
/// @param ascii Ascii character
/// @return Keyboard code (0 if none exists)
int keyboard_char_code(char ascii);

/// @brief Converts a keypress to an ascii character
/// @param keypress Keypress
/// @return Ascii character (or \0)
char keyboard_keypress_char(Keypress keypress);

#endif