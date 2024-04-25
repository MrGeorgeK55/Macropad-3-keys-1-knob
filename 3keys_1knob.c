// ===================================================================================
// Project:   MacroPad Mini for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Firmware example implementation for the MacroPad Mini.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: min. 12 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Press a macro key and see what happens.
// - To enter bootloader hold down key 1 while connecting the MacroPad to USB. All
//   NeoPixels will light up white as long as the device is in bootloader mode
//   (about 10 seconds).

// Example of the eeprom data structure

// Address:   0  1  2  3  4  5  6  7  8  9 10 11
// Address:  00 01 02 03 04 05 06 07 08 09 0A 0B  (hex)
//----------------------------------------------
// x0000000: T0 M0 C0 00 00 00 00 00 00 00 00 00  // key 1 // used only on keyboard keys
// x000000c: T1 C0 00 00 00 00 00 00 00 00 00 00  // key 2 // used only on consumer keys
// x0000018: T2 A0 C0 C1 C2 C3 C4 C5 C6 C7 C8 C9  // key 3 // used only on macro keys

// T0 represents the type field of the first key
// 00 = keyboard key // 01 = consumer key // 02 = macro key

// For keyboard and consumer keys
// M0 represents the mod field of the first key

// C0 represents the code field of the first key
// (rarelly you need a mod for consumer keys so it is not used)

// C1 to C9 represents the extra code fields only used for macro keys

// For macro keys
// A0 represents the ammount of keys in the macro

// for visibility reasons any 00 after any data its just an FF or empty field
// 00 is different than FF
/*

Key Modifiers

00 None
01 Ctrl
02 Shift
03 Ctrl + Shift
04 Alt
05 Ctrl + Alt
06 Shift + Alt
07 Ctrl + Shift + Alt

*/

// Functions not supported yet by only reading the eeprom
// Strings pf text
// Mouse movements
// Modifiers on macro secuences
// Modifiers on consumer keys
// Macros on rotary encoder rotation (enabled on the switch)

// Aditional notes
// i still havent found the way to make work the RIGTH_GUI and LEFT_GUI keys
// (aka windows keys or Apple equivalent)

// ===================================================================================

// Libraries
#include <config.h>     // user configurations
#include <system.h>     // system functions
#include <delay.h>      // delay functions
#include <neo.h>        // NeoPixel functions
#include <usb_conkbd.h> // USB HID consumer keyboard functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB)
{
  USB_interrupt();
}

enum KeyType
{
  KEYBOARD = 0,
  CONSUMER = 1,
  MACRO = 2
};

// theorically we can assign up to 19 keys to each macro
// 128 bytes divided by 6 keys = 21.333 bytes per key rounded to 21
// 21 minus 2 bytes for the type and ammount of keys = 19 bytes (macros up to 19 keys)

// 12 are the bytes used for each key for now (macros up to 10 keys)
// you only need to change this value if you want to use more keys
// by now it will only read the first 12 bytes of each key
// additionaly change  uint8_t code[X]; with the maximum ammount of keys you want to use

int index = 0;
int __xdata incremental = 12;

// structur with key details
// to make it easier to read, i changed the order of the fields
__xdata struct key
{
  enum KeyType type;     // type of key 00 = keyboard key // 01 = consumer key // 02 = macro key
  uint8_t mod;           // modifier of the key (only used in Keyboard type)
  uint8_t ammount;       // ammount of keys in the macro (only used in Macro type)
  uint8_t code[10];      // code of the key (only used in Keyboard and Macro type)
  uint16_t codeConsumer; // code of the key (only used in Consumer type) (was getting an error when using an array)
  uint8_t last;          // last state of the key
};

// ===================================================================================
// NeoPixel Functions
// ===================================================================================
// here you can chose the custom colors for the NeoPixels
// Update NeoPixels
void NEO_update(uint8_t *neo)
{
  EA = 0;                       // disable interrupts
  NEO_writeColor(neo[0], 0, 0); // NeoPixel 1 lights up red
  NEO_writeColor(0, neo[1], 0); // NeoPixel 2 lights up green
  NEO_writeColor(0, 0, neo[2]); // NeoPixel 3 lights up blue
  EA = 1;                       // enable interrupts
}

// Read EEPROM (stolen from https://github.com/DeqingSun/ch55xduino/blob/ch55xduino/ch55xduino/ch55x/cores/ch55xduino/eeprom.c)
// not changed, no idea how it works
uint8_t eeprom_read_byte(uint8_t addr)
{
  ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
  ROM_ADDR_L = addr << 1; // Addr must be even
  ROM_CTRL = ROM_CMD_READ;
  return ROM_DATA_L;
}

// handle key press
void handle_key(uint8_t current, struct key *key, uint8_t *neo)
{
  if (current != key->last)
  {                      // state changed?
    key->last = current; // update last state flag
    if (current)
    { // key was pressed?
      if (key->type == KEYBOARD)
      {
        KBD_code_press(key->mod, key->code[0]); // press keyboard/keypad key
      }

      else if (key->type == CONSUMER)
      {
        CON_press(key->codeConsumer); // press consumer key
      }

      else if (key->type == MACRO)
      {

        for (int i = 0; i < key->ammount; i++)
        {
          KBD_code_press(0, key->code[i]);
          DLY_ms(5);
          KBD_code_release(0, key->code[i]);
          DLY_ms(5);
        }
      }
      if (neo)
        *neo = NEO_MAX; // light up corresponding NeoPixel
    }
    else
    { // key was released?
      if (key->type == KEYBOARD)
      {
        KBD_code_release(key->mod, key->code[0]); // release
      }
      else if (key->type == CONSUMER)
      {
        CON_release(key->codeConsumer); // release
      }
      // no need to release macro keys but enter cooldown in case it is a macro key
      if (key->type == MACRO)
      {
        DLY_ms(50);
      }
    }
  }
  else if (key->last)
  { // key still being pressed?
    if (neo)
      *neo = NEO_MAX; // keep NeoPixel on
  }
}

// handle knob
void handle_knob(struct key *key)
{
  if (key->type == KEYBOARD)
  {
    KBD_code_type(key->mod, key->code[0]); // press keyboard/keypad key
  }

  else if (key->type == CONSUMER)
  {
    CON_type(key->codeConsumer); // press consumer key
  }

  else if (key->type == MACRO)
  {
    for (int i = 0; i < key->ammount; i++)
    {
      KBD_code_press(0, key->code[i]);
      DLY_ms(5);
      KBD_code_release(0, key->code[i]);
      DLY_ms(5);
    }
  }
  DLY_ms(10);
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void)
{
  // Variables
  __xdata struct key keys[6]; // array of struct for keys
  __idata uint8_t i;
  __idata uint8_t j; // temp variable
  uint8_t neo[6] =
      {0, NEO_MAX, 0}; // brightness of NeoPixels

  // Enter bootloader if key 1 is pressed
  NEO_init(); // init NeoPixels
  if (!PIN_read(PIN_KEY1))
  {              // key 1 pressed?
    NEO_latch(); // make sure pixels are ready
    for (i = 9; i; i--)
      NEO_sendByte(NEO_MAX); // light up all pixels
    BOOT_now();              // enter bootloader
  }

  // Setup
  CLK_config(); // configure system clock
  DLY_ms(5);    // wait for clock to settle
  KBD_init();   // init USB HID keyboard
  WDT_start();  // start watchdog timer

  // TODO: Read eeprom for key characters
  // it sohuld read each row of 12 bytes and assign them to the key struct
  for (i = 0; i < 6; i++)
  {
    if (eeprom_read_byte(index) == 0)
    {
      keys[i].type = KEYBOARD;
      keys[i].mod = eeprom_read_byte(index + 1);
      keys[i].code[0] = eeprom_read_byte(index + 2);
      keys[i].last = 0;
    }
    else if (eeprom_read_byte(index) == 1)
    {
      keys[i].type = CONSUMER;
      keys[i].codeConsumer = eeprom_read_byte(index + 1);
      keys[i].last = 0;
    }
    else if (eeprom_read_byte(index) == 2)
    {
      keys[i].type = MACRO;
      keys[i].ammount = eeprom_read_byte(index + 1);
      for (j = 0; j < keys[i].ammount; j++)
      {
        keys[i].code[j] = eeprom_read_byte(index + 2 + j);
      }
      keys[i].last = 0;
    }
    index += incremental;
  }

  // Loop
  while (1)
  {
    handle_key(!PIN_read(PIN_KEY1), &keys[0], &neo[0]);
    handle_key(!PIN_read(PIN_KEY2), &keys[1], &neo[1]);
    handle_key(!PIN_read(PIN_KEY3), &keys[2], &neo[2]);
    handle_key(!PIN_read(PIN_ENC_SW), &keys[3], (void *)0);

    if (!PIN_read(PIN_ENC_A))
    { // encoder turned ?
      if (PIN_read(PIN_ENC_B))
      {
        handle_knob(&keys[4]); // clockwise?
      }
      else
      {
        handle_knob(&keys[5]); // counter-clockwise?
      }
      while (!PIN_read(PIN_ENC_A))
        ; // wait until next detent
    }

    // Update NeoPixels
    NEO_update(neo);
    for (i = 0; i < 3; i++)
    {
      if (neo[i] > NEO_GLOW)
        neo[i]--; // fade down NeoPixel
    }
    DLY_ms(5);   // latch and debounce
    WDT_reset(); // reset watchdog
  }
}
