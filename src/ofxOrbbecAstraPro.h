//
//  ofxOrbbecAsPro.h
//
//  Created by StudioRF on 17/04/2020
//  Based on ofxOrbbecAstra by Matt Felsen (10/24/15).
//
//

#pragma once

#include "ofMain.h"
#include <astra/astra.hpp>

class ofxOrbbecAstraPro : public astra::FrameListener
{
public:
	ofxOrbbecAstraPro();
	~ofxOrbbecAstraPro();

	// For multiple cameras, use "device/sensor0",
	// "device/sensor1", etc. Otherwise, leave blank.
	void setup();
	void setup(int width, int height);
	void setup(const string& uri);

	void setRegistration(bool enable);
	void setDepthClipping(unsigned short nearPlane, unsigned short farPlane);

	void initSelectedStreams(bool color, bool depth, bool point);

	template<typename T>
	void initStream(astra_pixel_formats format, int fps, bool setMode = true);

	void initVideoGrabber(int deviceID = 0);

	void update();

	void draw(float x = 0, float y = 0, float w = 0, float h = 0);
	void drawDepth(float x = 0, float y = 0, float w = 0, float h = 0);
	
	// GET METHODS //

	inline ofVec3f getWorldCoordinateAt(int x, int y)
	{
		return cachedCoords[int(y) * width + int(x)];
	}
	inline unsigned short getNearClip() { return nearClip; }
	inline unsigned short getFarClip() { return farClip; }
	inline ofShortPixels& getRawDepth() { return depthPixels; }
	inline ofImage& getDepthImage() { return depthImg; }
	inline ofImage& getColorImage() { return colorImg; }
	inline astra_status_t getAstraStatus() { return status; }
	inline bool isFrameNew() { return bFrameNew; }

protected:

	virtual void on_frame_ready(astra::StreamReader& reader,
								astra::Frame& frame) override;

	void updateDepthLookupTable();

	astra::StreamSet streamSet;
	astra::StreamReader reader;

	int width;
	int height;
	bool bInitialized;
	bool bDepthSourceEnabled;
	bool bFrameNew;

	unsigned short nearClip;
	unsigned short farClip;

	ofShortPixels depthPixels;
	ofBuffer depthBuffer;
	ofImage depthImg;
	ofImage colorImg;

	astra_status_t status;

	// Many thanks to mattfelsen for this:
	// Hack for Astra Pro cameras which only expose color via a webcam/UVC
	// stream, not through the SDK
	bool bUseVideoGrabber;
	shared_ptr<ofVideoGrabber> grabber;

	vector<char> depthLookupTable;
	vector<ofVec3f> cachedCoords;
};