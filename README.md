
![20240426_140314](https://github.com/MrGeorgeK55/Macropad-3-keys-1-knob/assets/103085400/3a6453fc-e2b9-457d-84e9-b193e2463602)

# 3keys_1knob
Custom firmware for a 3-key + rotary encoder macropad (Based on IC CH552g) (https://hackaday.io/project/189914)

# Example data structure for the EEPROM


| Hex Address | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B |
|-----------|----|----|----|----|----|----|----|----|----|----|----|----|
| x0000000: | T0 | M0 | C0 | M1 | C1 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 
| x000000c: | T1 | C0 | C1 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 
| x0000018: | T2 | A0 | C0 | C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8 | C9 |

T0 represents the type field of the first key  
00 = keyboard key // 01 = consumer key // 02 = macro key  

### For keyboard and consumer keys
T0 represents the type field of the first key  
00 = keyboard key // 01 = consumer key // 02 = macro key  
  
For keyboard  
M0 represents the mod field of the first keyboad key  
M1 represents the mod field of the second keyboard key   
  
C0 represents the code field of the first keyboad key  
C1 represents the code field of the second keyboard key   
  
For Consumer   
C0 represents the code field of the first consumer key  
C1 represents the code field of the second consumer key  
(rarelly you need a mod for consumer keys so it is not used)  
  
> [!NOTE] 
> The keyboard and consumer keys have 2 states, this because in consumer keys Play/Pause are two different keys and for Keyboard becomes useful to have Ctrl+c and Ctrl+v in the same key  


### For macro keys
A0 represents the ammount of keys in the macro  
**This number must be in Hex value, not decimal**

  
*for visibility reasons any 00 after any data its just an FF or empty field   
00 is different than FF   

## Example data

### Keyboard type

| Raw bytes         | Type     | Mod | Key | Mod2 | Key2 | Result | Result2 |
| ----------- | --- | ----     | ---- | --- | --- | --- | --- |  
| 00 00 04 00 04 .. | Keyboard | None | a | None | a | a | a |
| 00 02 04 02 04 .. | Keyboard | Shift | a | Shift | a | A | A |
| 00 01 06 01 19 .. | Keyboard | Ctrl | c | Ctrl | v | Ctrl+C | Ctrl+V |
| 00 05 4C 03 29 .. | Keyboard | Ctrl+Alt | DEL | Ctrl+Shift | ESC | Ctrl+Alt+DEL | Ctrl+Shift+ESC |


### Consumer Type  

| Raw bytes | Type | Key | Key2 |
| -------- | ------ |  --- |  --- | 
| 01 B6 B6 .. | Consumer | Previous | Previous | 
| 01 B0 B1 .. | Consumer | Play | Pause |
| 01 E9 E9 .. | Consumer | Vol + | Vol + |
| 01 EA EA .. | Consumer | Vol - | Vol - |

### Macro Type

| Raw bytes | Type | # of Keys | Code | Code1 | Code2 | Result |   
| -------- | ------ |  --- |  --- |  --- | --- | --- |
| 02 03 06 04 17 .. | Macro | 3 | c | a | t | cat |
| 02 03 07 12 0A .. | Macro | 3 | d | o | g | dog |

# Compilation

### compile:
`$ make bin`

### compile & flash to pad:
- if on original firmware: depending on hardware you need to connect P3.6 to
  5V (VCC) using a 1k resistor or P1.5 to GND, while connecting USB
- if on this firmware: press key1 while connecting USB
- `$ make flash`

### configure keys:
1. `$ isp55e0 --data-dump flashdata.bin`
2. edit this binary (3 keys, plus 3 for the knob), and write it back:
3. `$ isp55e0 --data-flash flashdata.bin`


### Documentation

  [CH552x Datasheet ](https://www.wch-ic.com/downloads/CH552DS1_PDF.html)  
  [USB HID Codes](https://usb.org/sites/default/files/hut1_21_0.pdf#page=83)   
  [isp55e0](https://github.com/frank-zago/isp55e0)  

### Modifiers

 | ID | Function |  
 | --- | --- |
 | 00 | None  |  
 | 01 | Ctrl  |  
 | 02 | Shift | 
 | 03 | Ctrl + Shift |   
 | 04 | Alt |   
 | 05 | Ctrl + Alt | 
 | 06 | Shift + Alt | 
 | 07 | Ctrl + Shift + Alt | 

### Consumer Keyboard Keycodes
  
| Function | ID |  
| --- | --- |
| SYS_POWER | 30 |
| SYS_RESET | 31 |
| SYS_SLEEP | 32 |
| VOL_MUTE | E2 |
| VOL_UP | E9 |
| VOL_DOWN | EA |
| CON_MEDIA_PLAY | B0 |
| CON_MEDIA_PAUSE | B1 |
| CON_MEDIA_RECORD | B2 |
| MEDIA_FORWARD | B3 |
| MEDIA_REWIND | B4 |
| MEDIA_NEXT | B5 |
| MEDIA_PREV | B6 |
| MEDIA_STOP | B7 |
| MEDIA_EJECT | B8 |
| MEDIA_RANDOM | B9 |
| MENU | 40 |
| MENU_PICK | 41 |
| MENU_UP | 42 |
| MENU_DOWN | 43 |
| MENU_LEFT | 44 |
| MENU_RIGHT | 45 |
| MENU_ESCAPE | 46 |
| MENU_INCR | 47 |
| MENU_DECR | 48 |



# Notes:
### Functions not supported yet by only reading the EEPROM  
- Strings of text (you maybe need a bigger EEPROM) (maybe a huffman algorithm)
- Mouse movements/clicks (not sure, not tested)
- Modifiers on macro secuences
- Modifiers on consumer keys

i still havent found the way to make work the RIGTH_GUI and LEFT_GUI keys  
(aka windows keys or Apple equivalent)


### Yet to implement

- After a key is pressed and changes to Second key, add a delay to change it automatically to First Key 
- 
-
(no idea of what else could it need)
