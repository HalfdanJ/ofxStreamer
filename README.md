
## Installation

### Dependencies

Install ffmpeg wth x264 eg. using brew  

	$ brew install ffmpeg


### Adding to another project
Copy the folder to addons, remove example

	in other linker flags add "$(SRCROOT)/../libs/osx/libx264.a"  (or just drag the file into other linker flags)
	in Header Search Path add "/usr/local/include"	




### Compilaton notes

ffmpeg

	$ git clone git://source.ffmpeg.org/ffmpeg.git ffmpeg
	$ cd ffmpeg
	$ ./configure --cc="clang -m32"
	$ make

x264

	$ ./configure  --host=i686-apple-darwin11.1.0 --extra-cflags="-arch i386" --extra-ldflags="-arch i386" --disable-asm
	$ make

libiconv

    $ brew install libiconv --universal