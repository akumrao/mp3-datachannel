# mp3-datachannel
Mp3 streaming over webrtc data channel


## Usage

The signaling server uses Node.js, `express` and `ws` and can be started as such:

```
$ npm install
$ npm start
```
open browser
http://localhost:8000/
presse connect


open c++ app or one more browser tab 
./testclient  // check beneath for compilation



With the server running, open two windows of a recent version of Firefox, Chrome, or Safari and visit `https://localhost:8443`.

* Note the HTTPS! There is no redirect from HTTP to HTTPS.
* Some browsers or OSs may not allow the webcam to be used by multiple pages at once. You may need to use two different browsers or machines.

## TLS
To enable secured https
Recent versions of Chrome require secure websockets for WebRTC. Thus, this example utilizes HTTPS. Included is a self-signed certificate that must be accepted in the browser for the example to work.



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



For compiling c++ part

apt-get install m4
apt-get install automake
apt-get install libtool

git clone  https://github.com/sctplab/usrsctp
 cd usrsctp/
./bootstrap
./configure --enable-debug
 make install

example
https://chromium.googlesource.com/external/github.com/sctplab/usrsctp/+/HEAD/Manual.md
$ ./discard_server 11111 22222
$ ./client 127.0.0.1 9 0 22222 11111



git clone https://github.com/gabime/spdlog
mkdir build && cd build
root:build# cmake .. && make -j

for compilling nice

correct way

download libnice-0.1.17.tar.gz    from  https://www.mirrorservice.org/sites/distfiles.macports.org/libnice/
do not use any other version and also do not try to install with apt-get install

./configure   --disable-static  --without-gstreamer-0.10
make 
make install

apt-get install glib2-dev

wrong way do not do it 

apt install python3-pip
pip3 install meson

sudo pip3 install meson
meson build --prefix=/usr
ninja -C build
sudo ninja -C build install



cd  mp3-datachannel/

cmake -DCMAKE_BUILD_TYPE=Debug .

make

cd mp3-datachannel/websocket_client

./testclient

you will see mp4 movie at browser



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
 



