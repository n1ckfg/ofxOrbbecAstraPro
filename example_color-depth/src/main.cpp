#include "ofMain.h"
#include "ofApp.h"

int main()
{
	// Since we are going to see 2 streams, 
	// let's create a double sized 640 width window
	ofSetupOpenGL(640 * 2, 480, OF_WINDOW);
	ofRunApp(new ofApp());
}