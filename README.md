# 3keys_1knob
Custom firmware for a 3-key + rotary encoder macropad (Based on IC CH552g) (https://hackaday.io/project/189914)

# Example data structure for the EEPROM


| Hex Address | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B |
|-----------|----|----|----|----|----|----|----|----|----|----|----|----|
| x0000000: | T0 | M0 | C0 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 
| x000000c: | T1 | C0 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 
| x0000018: | T2 | A0 | C0 | C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8 | C9 |

T0 represents the type field of the first key  
00 = keyboard key // 01 = consumer key // 02 = macro key  

### For keyboard and consumer keys
M0 represents the mod field of the first key  
  
C0 represents the code field of the first key  
(rarelly you need a mod for consumer keys so it is not used)  
  
C1 to C9 represents the extra code fields only used for macro keys  
  
  
### Modifiers  
  
00 None  
01 Ctrl  
02 Shift  
03 Ctrl + Shift  
04 Alt  
05 Ctrl + Alt  
06 Shift + Alt  
07 Ctrl + Shift + Alt  


### For macro keys
A0 represents the ammount of keys in the macro  
  
for visibility reasons any 00 after any data its just an FF or empty field   
00 is different than FF   



# Compilatioon

### compile:
`$ make bin`

### compile & flash to pad:
- if on original firmware: depending on hardware you need to connect P3.6 to
  5V (VCC) using a 1k resistor or P1.5 to GND, while connecting USB
- if on this firmware: press key1 while connecting USB
- `$ make flash`

### configure keys:
1. `$ isp55e0 --data-dump flashdata.bin`
2. edit first 6 bytes of this binary (3 keys, plus 3 for the knob), and write it back:
3. `$ isp55e0 --data-flash flashdata.bin`
