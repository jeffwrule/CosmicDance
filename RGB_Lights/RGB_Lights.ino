// this file intentionally left blank see code.cpp

// This code allows you to control a number of light strings
// and switch their colors. 
// this was first created for the mater/anti-mater piece 
// and may have some very specific things about those pieces in it.


//
// Dec-18-2022;  Adding a new toggle switch to the sculpture but only for the Antimater version. 
//    
//    The sculpture had the momentary switch changed for a toggle switch.    
//    Updated to use new ToggleSwitch class; removed old momentary fob switch
//    Removed ifdef that allows for continious running...
//    Added immediate update of lights in inactive mode to allow you to see that the switch activated. 
//    removed devices (LED Strings) 
//        quark2 and quark3 these were external devices and one of them is using pin 12 (the toggle switch)
//        and is the source of the original bug where it looked like buttons where pushed
//    Added a new delay in inactive mode for 2 extra seconds at the top of the dimm cycle
//    Changed the Dimm cycle to increment each of the n(3) emitters of the pixel individually.   This take us 
//      from a 30 steps to 90 steps.   The hope is to have less noticible changes with each level change as it will
//      only be 1/3 of the light change.  Before we would increment all emmiters up one level with each step.
//    Removed FOB related #defines
//
