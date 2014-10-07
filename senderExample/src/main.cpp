#include "testApp.h"

//--------------------------------------------------------------
int main(){
	// set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
	ofSetupOpenGL(900, 480, OF_WINDOW);
	ofRunApp(new testApp()); // start the app
}
