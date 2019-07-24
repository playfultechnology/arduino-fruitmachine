# arduino-fruitmachine
This repository contains code listings, wiring diagrams, and other reference information relating to a project in which I disassembled a fruit machine into its various constituent components and then rebuilt them back together using an Arduino microprocessor as a controller.
Accompanying videos can be found at https://www.youtube.com/playlist?list=PLogiUurtMYtRenE8BooiE4FDIyxuIyZQj

<!-- ![Fruit Machine](FruitMachine.png?raw=true "Fruit Machine") -->
<img src="https://raw.githubusercontent.com/playfultechnology/arduino-fruitmachine/d5c05b58d625fce697c9b50bb38bf21852ebf172/FruitMachine.png" width="200" height="458">


## Feature Reel
The feature reel uses a stepper motor with 12 positions, three lamps, and an opto-sensor. 
![Fruit Machine Feature Reel](FruitMachineFeatureReel/FeatureReel_Assembly_bb.jpg?raw=true "Fruit Machine Reel Assembly")


## LED Display
The primary LED display is formed from five interlocking 1.8" common cathode 7-segment LED displays - datasheet can be found at  http://www.farnell.com/datasheets/99300.pdf
To use the displays, I'm sourcing current into the anodes of each segment using a UDN2981 high-side darlington transistor array (the channels of the UDN2981, corresponding to the segments to be lit, are set by a 74HC595 shift register). The segments of all the displays are wired together, and the particular digit to be lit is selected by grounding the common cathode of that digit through a ULN2803 low-side darlington transistor array. The ULN2803 too is set by a 74HC595 which is daisy-chained to the first, meaning all 5 digit displays can be set using just three pins on the Arduino.

Update: 
 - the 74HC595+UDN2981 on the high side can be replaced with a single MIC5981 8-channel source driver
 - the 74HC595+ULN2803 on the low side can be replaced with a single TPIC6595 8-channel sink driver

![Fruit Machine LED Displays](https://raw.githubusercontent.com/playfultechnology/arduino-fruitmachine/d5c05b58d625fce697c9b50bb38bf21852ebf172/FruitMachineLED/FruitMachineLED_bb.jpg?raw=true "Fruit Machine LED Display")
