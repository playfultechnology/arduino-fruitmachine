# arduino-fruitmachine
This repository contains code listings, wiring diagrams, and other reference information relating to a project in which I disassembled a fruit machine into its various constituent components and then rebuilt them back together using an Arduino microprocessor as a controller.
Accompanying videos can be found at https://www.youtube.com/playlist?list=PLogiUurtMYtRenE8BooiE4FDIyxuIyZQj

<!-- ![Fruit Machine](FruitMachine.png?raw=true "Fruit Machine") -->
<img src="https://raw.githubusercontent.com/playfultechnology/arduino-fruitmachine/d5c05b58d625fce697c9b50bb38bf21852ebf172/FruitMachine.png" width="200" height="458">


## Feature Reel
The feature reel uses a stepper motor with 12 positions, three lamps, and an opto-sensor. 
![Fruit Machine Feature Reel](FruitMachineFeatureReel/FeatureReel_Assembly_bb.jpg?raw=true "Fruit Machine Reel Assembly")


## Reels
The reels work the same way as the feature reel, except that each has 16 positions, three lamps, and an opto-sensor. Rather than directly wire to a motor driver, I used a RAMPS shield, which provides a convenient way to interface with stepper motor drivers (A4988, DRV8825, TMC2209 or similar)
![Fruit Machine Feature Reel](FruitMachineReels/FruitMachineReels_bb.jpg?raw=true "Fruit Machine Reels")


## Hopper
There are two commonly-used interfaces for coin hoppers - serial and parallel. 
### Parallel interface
The parallel interface is easy to understand - the hopper simply dispenses coins as soon as, and far as long as, 24V power is connected. A coin counter output pin is connected to ground every time a coin exits the hopper, which can be used to cut power once sufficient coins have been dispensed.
### Serial interface
A serial interface called CCTalk was introduced to make universal communication standard with various monetary devices in machines - note validators, coin mechs, and hoppers. The CCTalk standard defines a set of messages that are sent over a serial connection (you can view the documentation here: http://www.coinoperatorshop.com/media/products/manual/cctalk/cctalk44-2.pdf).

![Fruit Machine Hopper](FruitMachineHopper/FruitMachineHopper_bb.jpg?raw=true "Fruit Machine Hopper")

## LED Display
The primary LED display is formed from five interlocking 1.8" common cathode 7-segment LED displays - datasheet can be found at  http://www.farnell.com/datasheets/99300.pdf
To use the displays, I'm sourcing current into the anodes of each segment using a UDN2981 high-side darlington transistor array (the channels of the UDN2981, corresponding to the segments to be lit, are set by a 74HC595 shift register). The segments of all the displays are wired together, and the particular digit to be lit is selected by grounding the common cathode of that digit through a ULN2803 low-side darlington transistor array. The ULN2803 too is set by a 74HC595 which is daisy-chained to the first, meaning all 5 digit displays can be set using just three pins on the Arduino.

Update: 
 - the 74HC595+UDN2981 on the high side can be replaced with a single MIC5891 8-channel source driver
 - the 74HC595+ULN2803 on the low side can be replaced with a single TPIC6595 8-channel sink driver

![Fruit Machine LED Displays](https://raw.githubusercontent.com/playfultechnology/arduino-fruitmachine/d5c05b58d625fce697c9b50bb38bf21852ebf172/FruitMachineLED/FruitMachineLED_bb.jpg?raw=true "Fruit Machine LED Display")


## Lamps
The incandescent lamps were attached to a long cable harness, each sharing a common ground and their own individual 12V supply. To switch 12V to each lamp individually, I used a MIC5891 chip, which combines the functionality of a shift register and a high-side driver, able to source current of <50V@500mA to 8 channels. In hindsight, I could have also used this chip in place of the 74HC595 and UDN2981 in the LED display too.
![Fruit Machine Lamps](https://raw.githubusercontent.com/playfultechnology/arduino-fruitmachine/master/FruitMachineLamps/FruitMachineLamps_bb.jpg?raw=true "Fruit Machine Lamps")


## Illuminated Buttons
The original buttons, like the lamps, were illuminated with individual 12V incandescent bulbs, sharing a common ground and individually switched 12V supply wires. To facilitate easier wiring and greater functionality, I decided to replace these with PL9823 LEDs. PL9823s are similar to common "Neopixel" or WS2812 LEDs, in that they are indiviudally-addressable, RGB LEDs, and any number of them can be chained together in a long series controlled by only a single data pin on an Arduino. However, unlike Neopixels which commonly come supplied on a strip, PL9823s are individual LEDs supplied in a 5mm form factor, just like a regular LED (except with 4 pins, not 2).
![Illuminated Fruit Machine Buttons](https://raw.githubusercontent.com/playfultechnology/arduino-fruitmachine/master/FruitMachineButtons/FruitMachineButtons_bb.jpg?raw=true "Illuminated Fruit Machine Buttons")


## Counters
The counters are the most simple electronic components. 
![Counters](FruitMachineCoinCounter/FruitMachineCoinCounter_bb?raw=true "Fruit Machine Coin Counter")

