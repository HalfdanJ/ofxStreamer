
## Installation

### Dependencies

Install ffmpeg wth x264 eg. using brew  

	$ brew install ffmpeg

### Adding to another project
Copy the folder to addons, remove example

WRONG:

	in other linker flags add "$(SRCROOT)/../libs/osx/libx264.a"  (or just drag the file into other linker flags)
	in Header Search Path add "/usr/local/include"	


### Save the stream

	$ ./ffmpeg -i "udp://@:1234" -vcodec libx264 output.mp4 


### Compilaton notes

#### libiconv

    $ brew install libiconv --universal
	
#### libbz2

Download source from http://www.bzip.org/downloads.html
Open makefile and add -m32 to CFLAGS and LDFLAGS
Run 

	$ make
	$ make install
	
Then copy the just generated libbz2.a

ffmpeg

	$ git clone git://source.ffmpeg.org/ffmpeg.git ffmpeg
	$ cd ffmpeg
	$ ./configure --cc="clang -m32" --enable-gpl --prefix="/Users/jonas/Desktop/ffmpeg"  --enable-libx264 
	$ make

x264

	$ ./configure  --host=i686-apple-darwin11.1.0 --extra-cflags="-arch i386" --extra-ldflags="-arch i386" --disable-asm
	$ make

### Testing
List of RTSP streams for testing of the decoder: http://www.vlc.eu.pn/

