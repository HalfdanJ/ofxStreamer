
## Installation

### Dependencies

Install ffmpeg wth x264 eg. using brew  

	$ brew install ffmpeg


### Adding to another project
Copy the folder to addons, remove example

	in other linker flags add "$(SRCROOT)/../libs/osx/libx264.a"  (or just drag the file into other linker flags)
	in Header Search Path add "/usr/local/include"	




### Compilaton notes

libiconv

    $ brew install libiconv --universal