#include <stdbool.h>
#include <stdio.h>

#include <port.h>
#include <ps2.h>

static const int PS2_READ_CONFIG = 0x20;
static const int PS2_WRITE_CONFIG = 0x20;
static const int PS2_SELF_TEST = 0xAA;
static const int PS2_DISABLE_1 = 0xAD;
static const int PS2_DISABLE_2 = 0xA7;

static const int PS2_TEST_SUCCESS = 0x55;

static const int PS2_CONFIG_INT_1 = 1;
static const int PS2_CONFIG_CLOCK_1_OFF = 1 << 4;
static const int PS2_CONFIG_TRANSLATE_1 = 1 << 6;

static int ps2_comamand(int command, bool response)
{
    outb(PS2_STAT_COM, command);
    while (!(inb(PS2_STAT_COM) & 1))
    {
        printf("IN: %#x", inb(PS2_STAT_COM));
    }
    if (response) { return inb(PS2_DATA); }
    return 0;
}

static void ps2_comamand_second(int value)
{
    while ((inb(PS2_STAT_COM) & (1 << 1)));
    outb(PS2_DATA, value);
}

int ps2_initialize()
{
    ps2_comamand(PS2_DISABLE_1, false);
    ps2_comamand(PS2_DISABLE_2, false);
    inb(PS2_DATA);

    int config = ps2_comamand(PS2_READ_CONFIG, true);
    int mask = PS2_CONFIG_INT_1 | PS2_CONFIG_CLOCK_1_OFF | PS2_CONFIG_TRANSLATE_1;
    config &= !mask;
    ps2_comamand(PS2_WRITE_CONFIG, false);
    ps2_comamand_second(config);

    if (ps2_comamand(PS2_SELF_TEST, true) == PS2_TEST_SUCCESS) { return 0; }
    else
    {
        ps2_comamand(PS2_WRITE_CONFIG, false);
        ps2_comamand_second(config);
        return 1;
    }
}