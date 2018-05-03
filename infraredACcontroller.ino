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

#include "IRremote.h"
#include "PietteTech_DHT.h"
#include "Particle-OneWire.h"
#include "DS18B20.h"
#include "elapsedMillis.h"

#define APP_NAME "infraredACcontroller"
String VERSION = "Version 0.05";
/*******************************************************************************
 * changes in version 0.01:
       * Initial version
 * changes in version 0.02:
       * ready for testing
 * changes in version 0.03:
       * adding temperature sensor ds18b20 on D2
 * changes in version 0.04:
       * adding DHT22 sensor for sensing ambient temperature and humidity on D5
 * changes in version 0.05:
       * Particle share link: https://go.particle.io/shared_apps/5aea78bf5ae4dfa4a4000e74
       * fixing turn on/temp commands
       * adding heat and cool cloud functions
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
// unsigned int cool[13][59] = {{*cool_18}, {*cool_19}, {*cool_20}, {*cool_21}, {*cool_22}, {*cool_23}, {*cool_24}, {*cool_25}, {*cool_26}, {*cool_27}, {*cool_28}, {*cool_29}, {*cool_30}};
unsigned int cool[13][59];

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
// unsigned int heat[15][59] = {{*heat_18}, {*heat_19}, {*heat_20}, {*heat_21}, {*heat_22}, {*heat_23}, {*heat_24}, {*heat_25}, {*heat_26}, {*heat_27}, {*heat_28}, {*heat_29}, {*heat_30}};
unsigned int heat[15][59];

// OFF COMMAND
unsigned int off[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0};
// ON COMMAND
unsigned int on[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0};

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

/*******************************************************************************
 temperature sensor and variables
*******************************************************************************/
//Sets Pin D2 for Temp Sensor
DS18B20 ds18b20 = DS18B20(D2);

// Sample temperature sensor every 30 seconds
#define TEMPERATURE_SAMPLE_INTERVAL 30 * MILLISECONDS_TO_SECONDS
elapsedMillis temperatureSampleInterval;

//temperature related variables
#define INVALID -1
double temperatureCurrent = INVALID;
double temperatureTarget = 30.0;

//sensor difference with real temperature (if none set to zero)
//use this variable to align measurements with your existing displays
double temperatureCalibration = 0;

// Celsius is the default unit, set this boolean to true if you want to use Fahrenheit
const bool useFahrenheit = false;

#define MILLISECONDS_TO_SECONDS 1000

/*******************************************************************************
 DHT sensor for ambient sensing
*******************************************************************************/
#define DHTTYPE DHT22 // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN 5      // Digital pin for communications
void dht_wrapper();   // must be declared before the lib initialization
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);
bool bDHTstarted; // flag to indicate we started acquisition
double temperatureCurrent4 = INVALID;
double humidityCurrent4 = INVALID;
// This wrapper is in charge of calling the DHT sensor lib
void dht_wrapper() { DHT.isrCallback(); }

/*******************************************************************************
 * Function Name  : setup
 * Description    : this function runs once at system boot
 *******************************************************************************/
void setup()
{
  // publish startup message with firmware version
  Particle.publish(APP_NAME, VERSION, PRIVATE);

  Serial.begin(9600);
  Serial.println("hello");

  irrecv.enableIRIn(); // Start the receiver

  // declare cloud functions
  // https://docs.particle.io/reference/firmware/photon/#particle-function-
  // Up to 15 cloud functions may be registered and each function name is limited to a maximum of 12 characters.
  Particle.function("setTemp", setTemp);
  Particle.function("turnOff", turnOff);
  Particle.function("turnOn", turnOn);
  Particle.function("heat", setHeat);
  Particle.function("cool", setCool);

  // declare cloud variables
  // https://docs.particle.io/reference/firmware/photon/#particle-variable-
  // Up to 20 cloud variables may be registered and each variable name is limited to a maximum of 12 characters.
  Particle.variable("temperature", temperatureCurrent);

  Particle.variable("tempAmbieDHT", temperatureCurrent4);
  Particle.variable("humiAmbieDHT", humidityCurrent4);

  // send a samsung volume up/down code (good for testing your circuit)
  Particle.function("samsungVolUp", sendSamsungVolumeUp);
  Particle.function("samsungVolDn", sendSamsungVolumeDown);

  setupCoolArray();
  setupHeatArray();

}

/*******************************************************************************
 * Function Name  : loop
 * Description    : this function runs continuously while the project is running
 *******************************************************************************/
void loop()
{
  // enable this function to start decoding your remote
  // decodeIRcodes();

  // read the temperature sensor
  readTemperature();
}

/*******************************************************************************
********************************************************************************
********************************************************************************
 INFRARED FUNCTIONS
********************************************************************************
********************************************************************************
*******************************************************************************/

/*******************************************************************************
 * Function Name  : setTemp
 * Description    : this function sents the IR command to set the temperature of the device
                    for cooling, the command must be in the form of "coolXX" 
                    where 18 <= XX <= 30
                    Example: "cool23"
                    for heating, the command must be in the form of "heatXX" 
                    where 16 <= XX <= 30
                    Example: "heat18"
 *******************************************************************************/
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
  // irsend.sendRaw(irPulsesToSendWithOnCommand, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY); // ---->>> this should be the ON command plus temp desired
  // delay(500);
  irsend.sendRaw(irPulsesToSend, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY);
  Particle.publish(APP_NAME, "Setting device to: " + decodedCommand + "/" + targetTemp, PRIVATE);
  return 0;
}

/*******************************************************************************
 * Function Name  : setHeat
 * Description    : this function sents the IR command to set the temperature of the device
                    for heating, in the parameter send in the temperature desired
                    where temperature is 16 <= XX <= 30
                    Example: "18"
 *******************************************************************************/
int setHeat(String command)
{
  unsigned int irPulsesToSend[IR_COMMAND_LENGTH];
  int i, targetTemp;

  // verify the command length is valid
  if (command.length() != 2)
  {
    Particle.publish("ERROR", "Temperature has to be two digits long: " + command, PRIVATE);
    return -1;
  }

  // parse the temperature
  targetTemp = command.toInt();

  // verify the temperature is valid. Valid range is 16~30
  if ((targetTemp < 16) || (targetTemp > 30))
  {
    Particle.publish("ERROR", "Invalid temperature: " + String(targetTemp) + ". Valid range: 16~30", PRIVATE);
    return -1;
  }

  // unsigned int heat_16[59] = {3, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
  for (int i = 0; i < IR_COMMAND_LENGTH; i++)
  {
    irPulsesToSend[i] = heat[targetTemp - 16][i];
  }

  // convert the command from 3,2,1,0 to respective pulse duration
  for (int i = 0; i < IR_COMMAND_LENGTH; i++)
  {
    irPulsesToSend[i] = convertToPulseDuration(irPulsesToSend[i]);
  }

  // let's send the command twice so we make sure the device gets it
  // irsend.sendRaw(irPulsesToSendWithOnCommand, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY); // ---->>> this should be the ON command plus temp desired
  // delay(500);
  irsend.sendRaw(irPulsesToSend, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY);
  Particle.publish(APP_NAME, "Setting device to HEAT " + command, PRIVATE);
  return 0;
}

/*******************************************************************************
 * Function Name  : setCool
 * Description    : this function sents the IR command to set the temperature of the device
                    for cooling, the command must be in the form of "coolXX" 
                    where temperature must be 18 <= XX <= 30
                    Example: "23"
 *******************************************************************************/
int setCool(String command)
{
  unsigned int irPulsesToSend[IR_COMMAND_LENGTH];
  int i, targetTemp;

  // verify the command length is valid
  if (command.length() != 2)
  {
    Particle.publish("ERROR", "Temperature has to be two digits long: " + command, PRIVATE);
    return -1;
  }

  // parse the temperature
  targetTemp = command.toInt();

  // verify the temperature is valid. Valid range is 18~30
  if ((targetTemp < 18) || (targetTemp > 30))
  {
    Particle.publish("ERROR", "Invalid temperature: " + String(targetTemp) + ". Valid range: 18~30", PRIVATE);
    return -1;
  }

  // unsigned int cool[13][59] = { {*cool_18}, {*cool_19}, {*cool_20}, {*cool_21}, {*cool_22}, {*cool_23}, {*cool_24}, {*cool_25}, {*cool_26}, {*cool_27}, {*cool_28}, {*cool_29}, {*cool_30}};
  for (int i = 0; i < IR_COMMAND_LENGTH; i++)
  {
    irPulsesToSend[i] = cool[targetTemp - 18][i];
  }

  // convert the command from 3,2,1,0 to respective pulse duration
  for (int i = 0; i < IR_COMMAND_LENGTH; i++)
  {
    // irPulsesToSend[i] = convertToPulseDuration(irPulsesToSend[i]);
    irPulsesToSend[i] = convertToPulseDuration(irPulsesToSend[i]);
  }

  // let's send the command twice so we make sure the device gets it
  // irsend.sendRaw(irPulsesToSendWithOnCommand, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY); // ---->>> this should be the ON command plus temp desired
  // delay(500);
  irsend.sendRaw(irPulsesToSend, IR_COMMAND_LENGTH, IR_CARRIER_FREQUENCY);
  Particle.publish(APP_NAME, "Setting device to COOL " + command, PRIVATE);
  return 0;
}

/*******************************************************************************
 * Function Name  : turnOff
 * Description    : this function sents the IR command to turn off the device
 *******************************************************************************/
int turnOff(String dummy)
{
  unsigned int irPulsesToSend[IR_COMMAND_LENGTH];

  // convert the command from 3,2,1,0 to respective pulse duration
  for (int i = 0; i < IR_COMMAND_LENGTH; i++)
  {
    irPulsesToSend[i] = convertToPulseDuration(off[i]);
  }

  irsend.sendRaw(irPulsesToSend, 59, 38);
  Particle.publish(APP_NAME, "Setting device to OFF", PRIVATE);
  return 0;
}

/*******************************************************************************
 * Function Name  : turnOn
 * Description    : this function sents the IR command to turn on the device
 *******************************************************************************/
int turnOn(String dummy)
{
  unsigned int irPulsesToSend[IR_COMMAND_LENGTH];

  // convert the command from 3,2,1,0 to respective pulse duration
  for (int i = 0; i < IR_COMMAND_LENGTH; i++)
  {
    irPulsesToSend[i] = convertToPulseDuration(on[i]);
  }

    String decodif = "";
    for (int i = 0; i < IR_COMMAND_LENGTH; i++)
    {
      decodif = decodif + String(on[i]) + ",";
    }
    decodif = decodif + "END";
    Serial.println(decodif);

  irsend.sendRaw(irPulsesToSend, 59, 38);
  Particle.publish(APP_NAME, "Setting device to ON", PRIVATE);
  return 0;
}

/*******************************************************************************
 * Function Name  : convertToPulseDuration
 * Description    : this function sents the IR command to set the temperature of the device
                    Now remember that in this totally arbitrary encoding:
                     - 3 means an 8500 microseconds pulse
                     - 2 means a 4000 microseconds pulse
                     - 1 means a 1500 microseconds pulse
                     - 0 means a 500 microseconds pulse
 *******************************************************************************/
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

// these are codes for a Samsung TV. I leave them here as an easy way to test your IR sending circuit
// (handy if you have a samsung TV around)
unsigned int SamsungVolumeUp[68] = {4600, 4350, 650, 1550, 700, 1500, 700, 1550, 700, 400, 700, 400, 700, 450, 650, 450, 700, 400, 700, 1500, 700, 1550, 650, 1550, 700, 400, 700, 400, 700, 450, 650, 450, 700, 400, 700, 1500, 700, 1550, 650, 1550, 700, 400, 700, 450, 700, 400, 700, 400, 700, 400, 700, 450, 650, 450, 650, 450, 650, 1550, 700, 1500, 700, 1550, 700, 1500, 700, 1550, 650};
unsigned int SamsungVolumeDown[68] = {4450, 4450, 550, 1650, 550, 1650, 550, 1650, 550, 550, 550, 600, 500, 600, 500, 600, 500, 600, 500, 1650, 600, 1600, 600, 1600, 600, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 1650, 550, 1650, 550, 600, 500, 1650, 550, 600, 550, 550, 500, 600, 550, 550, 550, 550, 550, 550, 550, 1650, 550, 550, 550, 1650, 550, 1650, 550, 1650, 550, 1650, 550};

/*******************************************************************************
 * Function Name  : sendSamsungVolumeUp
 * Description    : send a samsung volume up code (good for testing your circuit)
 *******************************************************************************/
int sendSamsungVolumeUp(String dummy)
{
  irsend.sendRaw(SamsungVolumeUp, 68, 38);
  Particle.publish(APP_NAME, "Sent Samsung volume UP code", PRIVATE);
  return 0;
}

/*******************************************************************************
 * Function Name  : sendSamsungVolumeDown
 * Description    : send a samsung volume down code (good for testing your circuit)
 *******************************************************************************/
int sendSamsungVolumeDown(String dummy)
{
  irsend.sendRaw(SamsungVolumeDown, 68, 38);
  Particle.publish(APP_NAME, "Sent Samsung volume DOWN code", PRIVATE);
  return 0;
}

/*******************************************************************************
 * Function Name  : decodeIRcodes
 * Description    : this will print on the serial port the codes your remote is sending
                    enable this function in the loop function if you intend to use it
                    Now remember that in this totally arbitrary encoding:
                     - 3 means an 8500 microseconds pulse
                     - 2 means a 4000 microseconds pulse
                     - 1 means a 1500 microseconds pulse
                     - 0 means a 500 microseconds pulse
 *******************************************************************************/
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

/*******************************************************************************
********************************************************************************
********************************************************************************
 SENSOR FUNCTIONS
********************************************************************************
********************************************************************************
*******************************************************************************/

/*******************************************************************************
 * Function Name  : readTemperature
 * Description    : reads the temperature sensor in D2 at every TEMPERATURE_SAMPLE_INTERVAL
 * Return         : none
 *******************************************************************************/
void readTemperature()
{

  // is time up? no, then come back later
  if (temperatureSampleInterval < TEMPERATURE_SAMPLE_INTERVAL)
  {
    return;
  }

  //is time up, reset timer
  temperatureSampleInterval = 0;

  getTemp();
  getTempAmbDHT();

  Particle.publish(APP_NAME, "TAmb: " + double2string(temperatureCurrent) + ", TAmbDHT: " + double2string(temperatureCurrent4) + ", HAmbDHT: " + double2string(humidityCurrent4), PRIVATE);
}

/*******************************************************************************
 * Function Name  : getTemp
 * Description    : reads the DS18B20 sensor on D2
 * Return         : nothing
 *******************************************************************************/
void getTemp()
{

  int dsAttempts = 0;
  double temperatureLocal = INVALID;

  if (!ds18b20.search())
  {
    ds18b20.resetsearch();
    temperatureLocal = ds18b20.getTemperature();
    while (!ds18b20.crcCheck() && dsAttempts < 4)
    {
      dsAttempts++;
      if (dsAttempts == 3)
      {
        delay(1000);
      }
      ds18b20.resetsearch();
      temperatureLocal = ds18b20.getTemperature();
      continue;
    }
    dsAttempts = 0;

    if (useFahrenheit)
    {
      temperatureLocal = ds18b20.convertToFahrenheit(temperatureLocal);
    }

    // calibrate values
    temperatureLocal = temperatureLocal + temperatureCalibration;

    // if reading is valid, take it
    if ((temperatureLocal != INVALID) && (ds18b20.crcCheck()))
    {
      temperatureCurrent = temperatureLocal;
    }
  }
}

/*******************************************************************************
 * Function Name  : getTempAmbDHT
 * Description    : reads the temperature of the DHT22 sensor
 * Return         : none
 *******************************************************************************/
void getTempAmbDHT()
{

  // start the sample
  if (!bDHTstarted)
  {
    DHT.acquireAndWait(5);
    bDHTstarted = true;
  }

  //still acquiring sample? go away and come back later
  if (DHT.acquiring())
  {
    return;
  }

  //I observed my dht22 measuring below 0 from time to time, so let's discard that sample
  if (DHT.getCelsius() < 0)
  {
    //reset the sample flag so we can take another
    bDHTstarted = false;
    return;
  }

  if (useFahrenheit)
  {
    temperatureCurrent4 = DHT.getFahrenheit();
  }
  else
  {
    temperatureCurrent4 = DHT.getCelsius();
  }

  humidityCurrent4 = DHT.getHumidity();

  //reset the sample flag so we can take another
  bDHTstarted = false;
}

/*******************************************************************************
********************************************************************************
********************************************************************************
 HELPER FUNCTIONS
********************************************************************************
********************************************************************************
*******************************************************************************/

/*******************************************************************************
 * Function Name  : double2string
 * Description    : return the string representation of the double number
                     passed as parameter with 2 decimals
 * Return         : the string
 *******************************************************************************/
String double2string(double doubleNumber)
{
  String stringNumber = String(doubleNumber);

  //return only 2 decimals
  // Example: show 19.00 instead of 19.000000
  stringNumber = stringNumber.substring(0, stringNumber.length() - 4);

  return stringNumber;
}

/*******************************************************************************
 * Function Name  : setupCoolArray
 * Description    : fill up the cool array
 * Return         : void
 *******************************************************************************/
void setupCoolArray()
{

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[0][i] = cool_18[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[1][i] = cool_19[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[2][i] = cool_20[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[3][i] = cool_21[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[4][i] = cool_22[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[5][i] = cool_23[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[6][i] = cool_24[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[7][i] = cool_25[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[8][i] = cool_26[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[9][i] = cool_27[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[10][i] = cool_28[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[11][i] = cool_29[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    cool[12][i] = cool_30[i];
  }

}

/*******************************************************************************
 * Function Name  : setupHeatArray
 * Description    : fill up the heat array
 * Return         : void
 *******************************************************************************/
void setupHeatArray()
{

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[0][i] = heat_16[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[1][i] = heat_17[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[2][i] = heat_18[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[3][i] = heat_19[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[4][i] = heat_20[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[5][i] = heat_21[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[6][i] = heat_22[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[7][i] = heat_23[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[8][i] = heat_24[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[9][i] = heat_25[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[10][i] = heat_26[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[11][i] = heat_27[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[12][i] = heat_28[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[13][i] = heat_29[i];
  }

  for (int i = 0; i < IR_COMMAND_LENGTH; i++) 
  {
    heat[14][i] = heat_30[i];
  }

}
