================================================================================
      AmiBerry - Commodore Amiga Emulator - Playstation Classic Edition
================================================================================
This is still experimental package. Its fully safe to use, but the experience
running this App may not be perfect

NOTE: This installation does not include Amiga Kickstart ROMs that are needed
to properly run the emulation, as the files are copyrighted. You can only use
AmigaOS Kickstart ROM files if you own a legal Amiga computer, or if you buy
roms from Cloanto or Hyperion Entertainment CVBS. Browse Cloanto's page to buy
legal Amiga kickstarts. you may need 1.3 and 3.1(or 3.0) if you want to
emulate A1200 and A500. I haven't tested Amiberry PSC with 3.1.4 but it should
work as well.  kick13.rom and kick31.rom are correct files to put
into kickstarts folder.

Preface:
This is a patched port of AmiBerry for PSC running AutoBleem launcher.
I created it because:

1. RetroArch with Amiga core is sometimes pain in the ....
2. I am daily original classic Amiga user and I love this computer
3. There a are lot of good games you want to play
4. PSC can emulate Commodore Amiga with great speed

Kernel / Keyboard / Input:
Installation of OTG kernel and running the AB from the rear port is
recommended. This enables front ports to use USB keyboard or mouse. Due to
specific bug in PSC do not connect both to the same port. Try to use USB
hub to connect more devices if needed etc (this may not be so stable). In
general this installation should not need keyboard for basic game launching.
AmiBerry source code is patched to properly handle PSC gamepad. Also it
should be able to use Retroarch gamepad files for other gamepads (not tested)
USB keyboards/mice do not work on STOCK PSC kernel at all. In case of vanilla
PSC you will be limited to Gamepad.

Key usage:
PSC Reset - open GUI menu
PSC Power - quit emulation

Default bindings in ready made configs adds also:
Cross - joy fire
Triangle - left mouse click
Circle - Enter on keyboard
Square - Space on keyboard
Select - open GUI
Start - Swap JoyPorts

Installing OTG Kernel:
To get OTG kernel - install BleemSync payload (easiest) or use the kernel
by Honeylab and flash it with fasboot (difficult way, but works ... also enable
OTG support in start_pman as described by Honeylab)