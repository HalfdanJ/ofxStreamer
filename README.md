## About
This addon is for streaming an image from one computer to another using H.264 protocol and ffmpeg as backbone.


## Installation
Add the addon to a new project with the project generator, or add it to an existing project using the OFPlugin for XCode. 

**Note:** Due to a bug in the project generator and [OFPlugin](https://github.com/admsyn/OFPlugin/issues/26), you need to remove some files from your project. Remove the folder `ofxStreamer/libs/libx264/include` in the project navigator. 


### Save the stream from commandline
If you would like to save the stream as a video file you can use ffmpeg like this:

	$ ./ffmpeg -i "udp://@:1234" -vcodec libx264 output.mp4 


### Compilaton notes (not needed to use the addon)

#### libiconv

    $ brew install libiconv --universal
	
#### libbz2

Download source from http://www.bzip.org/downloads.html
Open makefile and add -m32 to CFLAGS and LDFLAGS
Run 

	$ make
	$ make install
	
Then copy the just generated libbz2.a



x264
	$ git clone git://git.videolan.org/x264.git
	$ ./configure --host=i686-apple-darwin11.1.0 --extra-cflags="-arch i386" --extra-ldflags="-arch i386" --enable-shared
	$ make
	$ sudo make install

ffmpeg

	$ git clone git://source.ffmpeg.org/ffmpeg.git ffmpeg
	$ cd ffmpeg
	$ ./configure  --enable-gpl   --enable-libx264 --extra-ldflags="-L/usr/local/lib" --extra-cflags="-I/usr/local/include" --cc="clang -m32"
	$ make
	$ sudo make install

### Testing
List of RTSP streams for testing of the decoder: http://www.vlc.eu.pn/

