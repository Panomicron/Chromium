# Chromium
## The Panomicron Chromium is an open compact camera project
the guiding principle and main point of a film compact camera is in the name, **compact**, everything is done to keep the camera as small as possible, (with the manufacturing methods I have had access to). 

Currently the camera needs some components to be machined for rigidity and smooth operations sake. the rest is 3d printable with a FDM 3d printer.

Still working on full documentation.



# Known Issues
- [x] The battery cover is held closed with magnets and is not strong enough to counteract the contact springs, It has been redesigned to use a locking mechanism instead, it has not yet been printed and tested.
- [ ] The geartrain tends to jam on rewinds, the motor has a hard time overcoming internal friction in the film cansiter or on the final swinging gear (?) and will push the swing arm out - will be prototyped in POM to see if friction can be reduced - otherwise another sytem might need to be devised
- [ ] Still a lot of noise in the system, and functions can trigger on their own, more small capacitors need to be added in general 
- [ ] Counter EEPROM value sometimes resets mid roll, once this has been fixed the camera could potentially automatically prompt rewind, currently you know the roll is finished because the motors will stall.

# To be done

- [ ] full documentation

- [ ] Lightmeter
  - [ ] Auto Exposure
  - [ ] Shutter Priority
  - [ ] Complete A mode
  - [ ] Metering Modes?
 
- [ ] Menu & Modes
  - [ ] Sport Mode
  - [ ] Silent Mode (no advance until shutter button is released , à la Hexar RF
  - [ ] Multiple Exposure Mode
  
- [ ] whole new 24x35mm variant of the camera, needs shutter to be flipped around as the motor would interfere in the frame, and would use a more aproriate lens, gear train would need to be modified, body widened. 
 
