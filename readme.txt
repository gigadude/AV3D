To compile:

Be sure you have stdint.h and inttypes.h which you can get here:

http://code.google.com/p/msinttypes/source/browse/trunk/inttypes.h
http://code.google.com/p/msinttypes/source/browse/trunk/stdint.h

Compilation has been tested using VS2008.

To run:

Download http://www.limitz.eu/av3d.zip

This will contain the avi file and the latest release build.
Be sure to run the vcredist_x86.exe to get msvcrt90.dll which is 
the runtime needed to run the application.

KNOWN ISSUES:
Some GPU's tested don't seem to be able to do a CLAMP instruction (MIN(MAX(x, 0), 1) or CLAMP(x, 0, 1);
If this is the case, replace this line in the fragment shader

float intensity = min(max(dot(_vNormal, vec3(0, 0, -1)), 0), 1);

with

float intensity = max(dot(normalize(_vNormal), vec3(0, 0, -1)), 0);