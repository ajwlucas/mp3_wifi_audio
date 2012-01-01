WIPHONIC - Low Cost Wireless Audio Streaming
============================================
18.08.10


Changelog - Release 0.2
-----------------------
- Fixed inlining issue with 10.4 tools causing constraints fail

About
-----

'WiPhonic' was a group project as part of the third year Computer
Science & Electronics course at University of Bristol. See 
http://www.cs.bris.ac.uk/ for more information about the 
university and course.

The aim of the project was to design a product capable of streaming
lossy audio from a PC over Wi-Fi to an XMOS-based device that can
be connected to an existing powered speaker for maximum flexibility.
The product specification was driven by cost, hence the decision to
utilise a PWM audio DAC. MP3 decoding is done in real time on the 
chip.

A Java application running on the PC uses a unique method of parsing
the MP3 file into packets. This reduces the buffer overhead on the
XMOS and is more resilient to the audio effects of packet loss.

A final product could be implemented on an L2 with a small added BOM. 
An external DAC could be added for higher quality audio.

Warranty
--------

None. The code should compile but things will probably break.

Known Issues
------------

- The PWM routine contains an unknown bug causing noise on output if
 the MP3 is heavily clipped. More investigation needed.
 
- Decoding of mono MP3 files is disabled due to memory constraints. 
 Re-enabling it will require some more optimisation of the polyphase
 filter and/or optimisations else where.


