# mp3-datachannel
Mp3 streaming over webrtc data channel

**Limitation of webrtc Audio channel with RTP packets are as follows**

Please check broadcast at mediaserer for mp3 streaming over audio channel


**Audio drops to maintain real time streaming**

16 bit sampling audio quality with Opus Codec 

Only supports Stereo 

Does not support mp3 and AAC( mp4a) 5.1 

Jitter buffer is very small

Even after Full Band music setup quality is not good

Full Band music takes lots of bandwidth

No surround sound at pixe streaming

No Multi Channel support for Opus, because of Browser limitations

**Refer
https://en.wikipedia.org/wiki/Opus_(audio_format)
**
Abbreviation
Audio
bandwidth
Effective
sample rate
NB (narrowband)
4 kHz
8 kHz
MB (medium-band)[note 1]
6 kHz
12 kHz
WB (wideband)
8 kHz
16 kHz
SWB (super-wideband)
12 kHz
24 kHz
FB (fullband)[note 2]
20 kHz
48 kHz







**Why Unreal  game engine is using OPus?**

Real time audio response. For example bullet firing game

What will happen if we add one more channel for background sound at Unreal Engine?

We can add data channel for background sound. We can stream mp3( stereo) or AAC/mp4a( 5.1) with data channel

We can have 5.1 surround sound with streaming AAC over data channel.


**Bottleneck in adding audio over data channel at Unreal Engine**

a)Unreal Engine does not support mp3 and AAC file format

b) Fade in and out  of audo  feature of Unreal engine are complex.

c)Unreal filter and sound enhancement  feature will break.




**Mono	**0: M: mono

Stereo	0: L: left
1: R: right

**Quad**	0: L: left
1: R: right
2: SL: surround left
3: SR: surround right

**5.1**	0: L: left
1: R: right
2: C: center
3: LFE: subwoofer
4: SL: surround left
5: SR: surround right
 

