#pragma once

#include "ofMain.h"
#include "ofxOrbbecAstraPro.h"

class ofApp : public ofBaseApp
{
public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);

private:
	ofxOrbbecAstraPro astraCam;
};
