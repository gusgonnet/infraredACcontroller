// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
// This is a human-readable summary of (and not a substitute for) the license.
// Disclaimer
//
// You are free to:
// Share — copy and redistribute the material in any medium or format
// Adapt — remix, transform, and build upon the material
// The licensor cannot revoke these freedoms as long as you follow the license terms.
//
// Under the following terms:
// Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
// NonCommercial — You may not use the material for commercial purposes.
// ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
// No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
//
// Notices:
// You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.
// No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
//
// github: https://github.com/gusgonnet/infraredACcontroller
// hackster: https://www.hackster.io/gusgonnet
//
// Free for personal use.
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/

#include "application.h"
#include "IRremote.h"

#define APP_NAME "infraredACcontroller"
String VERSION = "Version 0.02";
/*******************************************************************************
 * changes in version 0.01:
       * Initial version
 * changes in version 0.02:
       * ready for testing
*******************************************************************************/

SYSTEM_MODE(AUTOMATIC);

#define IR_COMMAND_LENGTH 59
#define IR_CARRIER_FREQUENCY 38
#define COMMAND_OFF "off"
#define COMMAND_HEAT "heat"
#define COMMAND_COOL "cool"

/* AC commands
 - 3 means an 8500 microseconds pulse
 - 2 means a 4000 microseconds pulse
 - 1 means a 1500 microseconds pulse
 - 0 means a 500 microseconds pulse
See explanation below.
*/

// COOL COMMANDS
unsigned int cool_18[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int cool_19[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0};
unsigned int cool_20[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0};
unsigned int cool_21[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0};
unsigned int cool_22[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0};
unsigned int cool_23[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
unsigned int cool_24[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0};
unsigned int cool_25[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0};
unsigned int cool_26[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0};
unsigned int cool_27[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0};
unsigned int cool_28[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0};
unsigned int cool_29[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0};
unsigned int cool_30[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

// this 2 dimension array is for convenience of the code when setting the temperature
unsigned int cool[13][59] = {{*cool_18}, {*cool_19}, {*cool_20}, {*cool_21}, {*cool_22}, {*cool_23}, {*cool_24}, {*cool_25}, {*cool_26}, {*cool_27}, {*cool_28}, {*cool_29}, {*cool_30}};

// HEAT COMMANDS
unsigned int heat_16[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
unsigned int heat_17[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
unsigned int heat_18[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0};
unsigned int heat_19[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0};
unsigned int heat_20[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0};
unsigned int heat_21[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0};
unsigned int heat_22[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0};
unsigned int heat_23[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
unsigned int heat_24[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0};
unsigned int heat_25[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
unsigned int heat_26[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
unsigned int heat_27[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
unsigned int heat_28[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0};
unsigned int heat_29[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0};
unsigned int heat_30[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0};

// this 2 dimension array is for convenience of the code when setting the temperature
unsigned int heat[15][59] = {{*heat_18}, {*heat_19}, {*heat_20}, {*heat_21}, {*heat_22}, {*heat_23}, {*heat_24}, {*heat_25}, {*heat_26}, {*heat_27}, {*heat_28}, {*heat_29}, {*heat_30}};

// OFF COMMAND
unsigned int off[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0};

// ON COMMAND
// To obtain the ON command for a setting/mode, I plan on setting the pulse at position 27 to 0 and the pulse at position 51 to 1.
// See explanation below.

/*
how I decoded a command:

This is what the IR library prints to the serial interface:
29168950,8500,4050,500,1500,550,500,550,450,550,500,500,1550,500,500,550,500,500,500,550,500,500,500,550,450,550,500,550,1500,500,500,550,500,500,500,550,500,500,550,500,1500,550,1500,550,450,550,1500,550,500,500,1500,550,500,550,450,550,500,500,500,550,END
3,3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,END

we can discard the first value, 29168950 in this case, since the library states that 
"the receive buffer starts with the duration of the gap space before the first mark".

so from this command:
29168950,8500,4050,500,1500,550,500,550,450,550,500,500,1550,500,500,550,500,500,500,550,500,500,500,550,450,550,500,550,1500,500,500,550,500,500,500,550,500,500,550,500,1500,550,1500,550,450,550,1500,550,500,500,1500,550,500,550,450,550,500,500,500,550,END

what is really interesting are these values:
8500,4050,500,1500,550,500,550,450,550,500,500,1550,500,500,550,500,500,500,550,500,500,500,550,450,550,500,550,1500,500,500,550,500,500,500,550,500,500,550,500,1500,550,1500,550,450,550,1500,550,500,500,1500,550,500,550,450,550,500,500,500,550,END

We could now send those values with the following code:

unsigned int temp18[59]={8550,4000,550,1500,550,450,550,500,500,550,500,1550,500,500,500,500,550,500,500,500,550,500,500,500,550,500,500,1500,550,500,500,500,550,500,500,500,550,450,550,1500,550,1500,550,500,500,1500,550,500,550,1500,500,500,550,500,500,500,550,450,550};
irsend.sendRaw(temp18,59,38);


Digging deeper:
If we separate values around 500 with a 0 and 1500 with a 1, this being arbitrarily, 
and also assigning 3 to values over 5000, and 2 to values around 4000, we end up with:
3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,END

Now remember that in this totally arbitrary encoding:
 - 3 means an 8500 microseconds pulse
 - 2 means a 4000 microseconds pulse
 - 1 means a 1500 microseconds pulse
 - 0 means a 500 microseconds pulse

why encoding in numbers, you may ask? it helps visualising and comparing commands, I hope. This way, it becomes easier to 
compare pulses that change or remain the same among different commands.


The OFF command
The off command seems to be the same regardless of the set temperature.
This is the off command at 18 and 30 degrees in cool mode:
OFF at 18:3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,END
OFF at 30:3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,END


The ON command
For the ON command things are different. This is the ON command at 18 and 30 degrees:
ON at 18: 3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,END
ON at 30: 3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,END

Definitely not the same. Seems like the ON command conveys the set temperature, among the rest of the settings. 
If you think about it for a second, this is probably wanted by design: you turn the device on and set it to the mode you want.

If we compare the ON with the already ON commands (I mean, the AC is already on when this command is sent) we could try to identify the differences:
position     : 0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8  
ON at 18     : 3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,END
already ON 18: 3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,END
ON at 30:      3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,END
already ON 30: 3,2,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,END
position     : 0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8  
Seems like the pulse at position 27 changes (first position being 0). It is 1 on the already on and 0 on the ON command. 
Pulse at position 51 changes as well, just the other way around. It is 0 on the already on and 1 on the ON command.  
The other pulses remain the same.

*/

/*
from: http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html

The raw data for received IR measures the duration of successive spaces and marks in 50us ticks. 
The first measurement is the gap, the space before the transmission starts. The last measurement is the final mark.

The raw data for sending IR holds the duration of successive marks and spaces in microseconds. 
The first value is the first mark, and the last value is the last mark.

There are two differences between the raw buffers for sending and for receiving. The send buffer 
values are in microseconds, while the receive buffer values are in 50 microsecond ticks. 
The send buffer starts with the duration of the first mark, while the receive buffer starts with 
the duration of the gap space before the first mark.

from: http://www.maartendamen.com/2010/05/jeenode-infrared-project-part-1-getting-started/

source: http://www.sbprojects.com/knowledge/ir/
In serial communication we usually speak of 'marks' and 'spaces'. The 'space' is the default signal,
 which is the off state in the transmitter case. No light is emitted during the 'space' state. 
 During the 'mark' state of the signal the IR light is pulsed on and off at a particular frequency. 
 Carrier frequencies between 30kHz and 60kHz are commonly used in consumer electronics. 
 The most common one is 38kHz though. 
At the receiver side a 'space' is usually represented by a high level of the receiver's output. 
A 'mark' is then automatically represented by a low level.

Please note that the 'marks' and 'spaces' are not the 1-s and 0-s we want to transmit. 
The real relationship between the 'marks' and 'spaces' and the 1-s and 0-s depends on 
the protocol that's being used. More information about that can be found on the pages 
that describe the protocols.

transmit example from the Particle community:
https://community.particle.io/t/cash-bounty-for-irremote-irrecv-port-solved/23349/51?u=gusgonnet

*/

// source : https://github.com/babean/BeanSQ-IRremote/blob/master/examples/IRrecord/IRrecord.ino
// Storage for the recorded code
int codeType = -1;             // The type of code
unsigned long codeValue;       // The code value if not raw
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen;                   // The length of the code
int toggle = 0;                // The RC5/6 toggle state

int RECV_PIN = D6;
IRrecv irrecv(RECV_PIN);
decode_results results;

int TX_PIN = A5; //this is hardcoded in IRremoteInt.h line 488: #define TIMER_PWM_PIN        A5
IRsend irsend;

void setup()
{
  // publish startup message with firmware version
  Particle.publish(APP_NAME, VERSION, PRIVATE);

  Serial.begin(9600);
  Serial.println("hello");

  irrecv.enableIRIn(); // Start the receiver

  Particle.function("setTemp", setTemp);
  Particle.function("turnOff", turnOff);
}

void loop()
{
  // enable this function to start decoding your remote
  // decodeIRcodes();
}

// this function supports:
// heat19
// cool23
int setTemp(String command)
{
  unsigned int irPulsesToSendWithOnCommand[IR_COMMAND_LENGTH];
  unsigned int irPulsesToSend[IR_COMMAND_LENGTH];
  int i, targetTemp;
  bool commandIsOK = false;

  String tempCommand = command.toLowerCase();
  String decodedCommand = "";

  // verify the command length is valid
  if (tempCommand.length() != 6)
  {
    Particle.publish("ERROR", "Invalid command: " + command, PRIVATE);
    return -1;
  }

  // verify the command is valid
  if (!((tempCommand.startsWith(COMMAND_HEAT)) || (tempCommand.startsWith(COMMAND_COOL))))
  {
    Particle.publish("ERROR", "Invalid command: " + command, PRIVATE);
    return -1;
  }

  // parse the temperature, starting at 4th char
  targetTemp = tempCommand.substring(4).toInt();

  // is it a HEAT command?
  if (tempCommand.startsWith(COMMAND_HEAT)) //"heat"
  {
    decodedCommand = COMMAND_HEAT;

    // verify the temperature is valid. Valid range is 16~30
    if ((targetTemp < 16) || (targetTemp > 30))
    {
      Particle.publish("ERROR", "Invalid temperature: " + String(targetTemp) + ". Valid range: 16~30", PRIVATE);
      return -1;
    }
  }

  // is it a COOL command?
  if (tempCommand.startsWith(COMMAND_COOL)) //"cool"
  {
    decodedCommand = COMMAND_COOL;

    // verify the temperature is valid. Valid range is 18~30
    if ((targetTemp < 18) || (targetTemp > 30))
    {
      Particle.publish("ERROR", "Invalid temperature: " + String(targetTemp) + ". Valid range: 18~30", PRIVATE);
      return -1;
    }
  }

  // now I have to get the command to send from the respective array
  if (decodedCommand == COMMAND_COOL)
  {
    // unsigned int cool[13][59] = { {*cool_18}, {*cool_19}, {*cool_20}, {*cool_21}, {*cool_22}, {*cool_23}, {*cool_24}, {*cool_25}, {*cool_26}, {*cool_27}, {*cool_28}, {*cool_29}, {*cool_30}};
    for (int i = 0; i < IR_COMMAND_LENGTH; i++)
    {
      irPulsesToSendWithOnCommand[i] = cool[targetTemp - 18][i];
      irPulsesToSend[i] = cool[targetTemp - 18][i];
    }
  }
  if (decodedCommand == COMMAND_HEAT)
  {
    // unsigned int heat_16[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
    for (int i = 0; i < IR_COMMAND_LENGTH; i++)
    {
      irPulsesToSendWithOnCommand[i] = heat[targetTemp - 16][i];
      irPulsesToSend[i] = heat[targetTemp - 16][i];
    }
  }

  // modify the on command with the ON bits
  // To obtain the ON command for a setting/mode, the plan is to set the pulse at position 27 to 0 and the pulse at position 51 to 1.
  irPulsesToSendWithOnCommand[27] = 0;
  irPulsesToSendWithOnCommand[51] = 1;

  // convert the command from 3,2,1,0 to respective pulse duration
  for (int i = 0; i < IR_COMMAND_LENGTH; i++)
  {
    irPulsesToSendWithOnCommand[i] = convertToPulseDuration(irPulsesToSendWithOnCommand[i]);
    irPulsesToSend[i] = convertToPulseDuration(irPulsesToSend[i]);
  }

  // let's send the command twice so we make sure the device gets it
  irsend.sendRaw(irPulsesToSendWithOnCommand, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY); // ---->>> this should be the ON command plus temp desired
  delay(500);
  irsend.sendRaw(irPulsesToSend, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY);
  Particle.publish(APP_NAME, "Setting device to: " + decodedCommand + "/" + targetTemp, PRIVATE);
  return 0;
}

// Now remember that in this totally arbitrary encoding:
//  - 3 means an 8500 microseconds pulse
//  - 2 means a 4000 microseconds pulse
//  - 1 means a 1500 microseconds pulse
//  - 0 means a 500 microseconds pulse
int convertToPulseDuration(unsigned int code)
{
  switch (code)
  {
  case 0:
    return 500;
    break;
  case 1:
    return 1500;
    break;
  case 2:
    return 4000;
    break;
  case 3:
    return 8500;
    break;
  }
  return -1;
}

int turnOff(String dummy)
{

  irsend.sendRaw(off, 59, 38);
  delay(500);
  irsend.sendRaw(off, 59, 38);
  Particle.publish(APP_NAME, "Setting device to OFF", PRIVATE);
  return 0;
}

// these are codes for a Samsung TV. I leave them here as an easy way to test your IR sending circuit
// (handy if you have a samsung TV around)
unsigned int SamsungVolumeUp[68] = {4600, 4350, 650, 1550, 700, 1500, 700, 1550, 700, 400, 700, 400, 700, 450, 650, 450, 700, 400, 700, 1500, 700, 1550, 650, 1550, 700, 400, 700, 400, 700, 450, 650, 450, 700, 400, 700, 1500, 700, 1550, 650, 1550, 700, 400, 700, 450, 700, 400, 700, 400, 700, 400, 700, 450, 650, 450, 650, 450, 650, 1550, 700, 1500, 700, 1550, 700, 1500, 700, 1550, 650};
unsigned int SamsungVolumeDown[68] = {4450, 4450, 550, 1650, 550, 1650, 550, 1650, 550, 550, 550, 600, 500, 600, 500, 600, 500, 600, 500, 1650, 600, 1600, 600, 1600, 600, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 1650, 550, 1650, 550, 600, 500, 1650, 550, 600, 550, 550, 500, 600, 550, 550, 550, 550, 550, 550, 550, 1650, 550, 550, 550, 1650, 550, 1650, 550, 1650, 550, 1650, 550};

int sendSamsungVolumeUp(String dummy)
{
  irsend.sendRaw(SamsungVolumeUp, 68, 38);
  Particle.publish(APP_NAME, "Sent Samsung volume UP code", PRIVATE);
  return 0;
}

int sendSamsungVolumeDown(String dummy)
{
  irsend.sendRaw(SamsungVolumeDown, 68, 38);
  Particle.publish(APP_NAME, "Sent Samsung volume DOWN code", PRIVATE);
  return 0;
}

// enable this function to start decoding your remote
void decodeIRcodes()
{

  if (irrecv.decode(&results))
  {
    unsigned long hash = decodeHash(&results);

    Serial.println(results.value, HEX);

    String decodif = "";
    for (int i = 0; i < results.rawlen; i++)
    {
      decodif = decodif + String(results.rawbuf[i] * 50) + ",";
    }
    decodif = decodif + "END";
    Serial.println(decodif);

    String decodif0123 = "";
    String tempChar = "";

    // Now remember that in this totally arbitrary encoding:
    //  - 3 means an 8500 microseconds pulse
    //  - 2 means a 4000 microseconds pulse
    //  - 1 means a 1500 microseconds pulse
    //  - 0 means a 500 microseconds pulse
    for (int i = 0; i < results.rawlen; i++)
    {
      if (results.rawbuf[i] * 50 > 5000)
      {
        tempChar = "3";
      }
      else if (results.rawbuf[i] * 50 > 2000)
      {
        tempChar = "2";
      }
      else if (results.rawbuf[i] * 50 > 1000)
      {
        tempChar = "1";
      }
      else
      {
        tempChar = "0";
      }
      decodif0123 = decodif0123 + tempChar + ",";
    }
    decodif0123 = decodif0123 + "END";
    Serial.println(decodif0123);

    Particle.publish(String(results.value), decodif + "|" + String(results.rawlen), PRIVATE);
    // storeCode(&results);
    irrecv.resume(); // Receive the next value
  }
}

#define FNV_PRIME_32 16777619
#define FNV_BASIS_32 2166136261

int compare(unsigned int oldval, unsigned int newval)
{
  if (newval < oldval * .8)
  {
    return 0;
  }
  else if (oldval < newval * .8)
  {
    return 2;
  }
  else
  {
    return 1;
  }
}
unsigned long decodeHash(decode_results *results)
{
  unsigned long hash = FNV_BASIS_32;
  for (int i = 1; i + 2 < results->rawlen; i++)
  {
    int value = compare(results->rawbuf[i], results->rawbuf[i + 2]);
    // Add value into the hash
    hash = (hash * FNV_PRIME_32) ^ value;
  }
  return hash;
}
