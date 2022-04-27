//
//  ofxOrbbecAstraPro.h
//
//  Created by StudioRF on 17/04/2020
//  Based on ofxOrbbecAstra by Matt Felsen (10/24/15).
//
//

#include "ofxOrbbecAstraPro.h"

//------------------------------------------------------------------------
ofxOrbbecAstraPro::ofxOrbbecAstraPro()
	:width(640), height(480), nearClip(150), farClip(1600)
{
	bInitialized = false;
	bDepthSourceEnabled = true;
}
//------------------------------------------------------------------------
ofxOrbbecAstraPro::~ofxOrbbecAstraPro() 
{
	astra::terminate();
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::setup() 
{
	setup("device/default");
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::setup(int width, int height) 
{
	this->width = width;
	this->height = height;

	setup("device/default");
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::setup(const string& uri) 
{
	colorImg.allocate(width, height, OF_IMAGE_COLOR);
	depthImg.allocate(width, height, OF_IMAGE_GRAYSCALE);
	depthPixels.allocate(width, height, OF_IMAGE_GRAYSCALE);
	cachedCoords.resize(width * height);
	updateDepthLookupTable();
	
	try
	{
		astra::initialize();
	}
	catch (const std::exception& exc)
	{
		ofLogError("ofxOrbbecAstraPro::setup") << exc.what();
	}

	streamSet = astra::StreamSet(uri.c_str());
	reader = astra::StreamReader(streamSet.create_reader());

	bInitialized = true;
	reader.add_listener(*this);
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::setRegistration(bool enable) 
{
	if (!bInitialized) {
		ofLogWarning("ofxOrbbecAstraPro::setRegistration") 
			<< "Must call setup() before setRegistration()";
		return;
	}

	reader.stream<astra::DepthStream>().enable_registration(enable);
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::setDepthClipping(unsigned short nearPlane, 
										 unsigned short farPlane) 
{
	nearClip = nearPlane;
	farClip = farPlane;
	updateDepthLookupTable();
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::initSelectedStreams(bool color, bool depth, bool point)
{
	if (!bInitialized)
	{
		ofLogWarning("ofxOrbbecAstraPro::initSelectedStreams")
			<< "Initialize first by calling setup()";
		return;
	}

	int device_fps = 30;

	if (color) {
		initStream<astra::ColorStream>(
			astra_pixel_formats::ASTRA_PIXEL_FORMAT_RGB888, 
			device_fps);
	}
	if (depth) {
		initStream<astra::DepthStream>(
			astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM, 
			device_fps);
	}
	if (point) {
		//initStream<astra::PointStream>(
		//	astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM, 
		//	device_fps, 
		//	false);
	}
}
//------------------------------------------------------------------------
template<typename T>
void ofxOrbbecAstraPro::initStream(astra_pixel_formats format,
	int fps, bool setMode)
{
	if (!bInitialized)
	{
		ofLogWarning("ofxOrbbecAstraPro::initStream")
			<< "Initialize first by calling setup()";
		return;
	}

	auto stream = reader.stream<T>();

	if (setMode) {
		astra::ImageStreamMode mode;
		mode.set_width(width);
		mode.set_height(height);
		mode.set_pixel_format(format);
		mode.set_fps(fps);

		stream.set_mode(mode);
	}
	stream.start();
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::initVideoGrabber(int deviceID) 
{
	bUseVideoGrabber = true;

	grabber = make_shared<ofVideoGrabber>();
	grabber->setDeviceID(deviceID);
	grabber->setDesiredFrameRate(30);
	grabber->setup(width, height);
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::update()
{
	bFrameNew = false;
	status = astra_update();

	if (bUseVideoGrabber && grabber) {
		grabber->update();
		if (grabber->isFrameNew()) {
			colorImg.setFromPixels(grabber->getPixels());
			colorImg.mirror(false, true);
			colorImg.update();
		}
	}
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::draw(float x, float y, float w, float h)
{
	if (!w) w = width;
	if (!h) h = height;

	colorImg.draw(x, y, w, h);
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::drawDepth(float x, float y, float w, float h)
{
	if (!w) w = width;
	if (!h) h = height;
	depthImg.draw(x, y, w, h);
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::on_frame_ready(astra::StreamReader& reader,
									   astra::Frame& frame)
{
	bFrameNew = true;

	auto colorFrame = frame.get<astra::ColorFrame>();
	auto depthFrame = frame.get<astra::DepthFrame>();
	auto pointFrame = frame.get<astra::PointFrame>();

	if (colorFrame.is_valid()) 
	{
		colorFrame.copy_to((astra::RgbPixel*) colorImg.getPixels().getData());
		colorImg.update();
	}

	if (depthFrame.is_valid()) 
	{
		depthFrame.copy_to((short*)depthPixels.getData());

		if (bDepthSourceEnabled) {
			// TODO do this with a shader so it's fast?
			for (int i = 0; i < (int)depthPixels.size(); i++) {
				short depth = depthPixels.getColor(i).r;
				float val = depthLookupTable[depth];
				depthImg.setColor(i, ofColor(val));
			}
			depthImg.update();
		}
	}

	if (pointFrame.is_valid()) 
	{
		pointFrame.copy_to((astra::Vector3f*) cachedCoords.data());
	}
}
//------------------------------------------------------------------------
void ofxOrbbecAstraPro::updateDepthLookupTable() 
{
	// From product specs, range is 8m
	int maxDepth = 8000;
	depthLookupTable.resize(maxDepth);

	// Depth values of 0 should be discarded, so set the LUT value to 0 as well
	depthLookupTable[0] = 0;

	// Set the rest
	for (int i = 1; i < maxDepth; i++) {
		depthLookupTable[i] = ofMap(i, nearClip, farClip, 255, 0, true);
	}
}
//------------------------------------------------------------------------