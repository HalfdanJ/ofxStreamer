## About
This addon is for streaming an image from one computer to another using H.264 protocol and ffmpeg as backbone.


## Installation
Add the addon to a new project with the project generator, or add it to an existing project using the OFPlugin for XCode. 

**Note:** Due to a bug in the [project generator](https://github.com/ofZach/projectGeneratorSimple/issues/60) and [OFPlugin](https://github.com/admsyn/OFPlugin/issues/26), you need to remove some files from your project. Remove the folder `ofxStreamer/libs/libx264/include` in the project navigator. 

## Usage
*ofxStreamerSender*: 
Use the `ofxStreamerSender` to send a video feed to a receiver.  It can both send an `ofImage` or a `unsigned char *`, it will then be encoded by ffmpeg into a h.264 packet, and transmitted over UDP to the destination IP.  The settings of the sender can be tuned with the `preset` and `tune` attributes.

The stream can also be opened in applications like VLC

*ofxStreamerReceiver*:
Use this class to receive video feeds from the sender. It is inheriting from `ofBaseVideo`, so it can be used like a `ofVideoPlayer` after it has been set up with a port number.
	

### Save the stream from commandline
If you would like to save the stream as a video file you can use ffmpeg like this:

	$ ./ffmpeg -i "udp://@:1234" -vcodec libx264 output.mp4 
