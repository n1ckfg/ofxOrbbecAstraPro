#include "ofApp.h"

//------------------------------------------------------------------------
void ofApp::setup()
{
	astraCam.setup();
	astraCam.setRegistration(true);
	astraCam.initSelectedStreams(true, true, false);
	
	/*
	// https://www.dropbox.com/sh/bgua92ffs7qdr7r/AACpyRqgeP1U7mwvNqzJPB8ra?dl=0&preview=Orbbec+OpenNI+Extended+API+Instruction.pdf
	// http://docs.ros.org/en/kinetic/api/astra_camera/html/namespaceopenni.html
	int dataSize = 4;
	int ldp_en = 0;
	int propertyId = 100; //openni::OBEXTENSION_ID_LDP_EN 
	astraCam.setProperty(propertyId, (uint8_t *) &ldp_en, dataSize);
	*/
}
//------------------------------------------------------------------------
void ofApp::update()
{
	astraCam.update();
	if (astraCam.isFrameNew())
	{
		// Do something fun.
	}
}
//------------------------------------------------------------------------
void ofApp::draw()
{
	astraCam.drawDepth(0, 0);
	astraCam.draw(640, 0);
}
//------------------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	// Poll events here!
}
//------------------------------------------------------------------------
