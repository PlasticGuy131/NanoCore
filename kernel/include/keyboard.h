#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED 1

#define KEY_FLAG_PRESSED 1
#define KEY_FLAG_SHIFT (1 << 1)
#define KEY_FLAG_CAPS_LOCK (1 << 2)
#define KEY_FLAG_CTRL (1 << 3)
#define KEY_FLAG_ALT (1 << 4)
#define KEY_FLAG_NUM_LOCK (1 << 5)
#define KEY_FLAG_SCROLL_LOCK (1 << 6)

#define KEYCODE_ESCAPE 57
#define KEYCODE_LEFT_CONTROL 58
#define KEYCODE_LEFT_SHIFT 59
#define KEYCODE_RIGHT_SHIFT 60
#define KEYCODE_LEFT_ALT 61
#define KEYCODE_CAPS_LOCK 62
#define KEYCODE_NUM_LOCK 63
#define KEYCODE_SCROLL_LOCK 64

#define KEYCODE_FUNCTION_PLUS_N 64

#define KEYCODE_KEYPAD_SLASH 77
#define KEYCODE_KEYPAD_ENTER 78

#define KEYCODE_RIGHT_CONTROL 80
#define KEYCODE_ALTGR 81
#define KEYCODE_HOME 82
#define KEYCODE_CURSOR_UP 83
#define KEYCODE_PAGE_UP 84
#define KEYCODE_CURSOR_LEFT 85
#define KEYCODE_CURSOR_RIGHT 86
#define KEYCODE_END 87
#define KEYCODE_CURSOR_DOWN 88
#define KEYCODE_PAGE_DOWN 89
#define KEYCODE_INSERT 90
#define KEYCODE_LEFT_GUI 91
#define KEYCODE_RIGHT_GUI 92
#define KEYCODE_APPS 93

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