# arduino-fruitmachine
This repository contains code listings, wiring diagrams, and other reference information relating to a project in which I disassembled a fruit machine into its various constituent components and then rebuilt them back together using an Arduino microprocessor as a controller.
Accompanying videos can be found at https://www.youtube.com/playlist?list=PLogiUurtMYtRenE8BooiE4FDIyxuIyZQj

![Fruit Machine](FruitMachine.png?raw=true "Fruit Machine")


## Feature Reel
The feature reel uses a stepper motor with 12 positions, three lamps, and an opto-sensor. 
![Fruit Machine Feature Reel](FruitMachineFeatureReel/FeatureReel_Assembly_bb.jpg?raw=true "Fruit Machine Reel Assembly")


## LED Display
The primary LED display is formed from five interlocking 1.8" common cathode 7-segment LED displays - datasheet can be found at  http://www.farnell.com/datasheets/99300.pdf
To use the displays, I'm sourcing current into the anodes of each segment using a UDN2981 high-side darlington transistor array (the channels of the UDN2981, corresponding to the segments to be lit, are set by a 74HC595 shift register). The segments of all the displays are wired together, and the particular digit to be lit is selected by grounding the common cathode of that digit through a ULN2803 low-side darlington transistor array.

![Fruit Machine LED Displays](FruitMachineLED/FruitMachineLED_bb.jpg?raw=true "Fruit Machine LED Display")
