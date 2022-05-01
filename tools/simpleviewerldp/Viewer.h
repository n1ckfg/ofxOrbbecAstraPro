

#ifndef _ONI_SAMPLE_VIEWER_H_
#define _ONI_SAMPLE_VIEWER_H_

#include <OpenNI.h>
#include "libusb_set.h"

#define MAX_DEVICES 20  // There is no limitations, we choose 20 as "reasonable" value in real use case.
#define MAX_DEPTH 10000

typedef char XnChar;


class SampleViewer
{
public:
    SampleViewer(const char* strSampleName, openni::VideoStream depthStreams[], int devNum);
	virtual ~SampleViewer();

    virtual openni::Status init(int argc, char **argv, const XnChar** deviceUri, char (*SerialNum)[12],openni::Device& Device, int sensortype);
	virtual openni::Status run();	//Does not return

protected:
	virtual void display();
	virtual void displayPostDraw(){};	// Overload to draw over the screen image

	virtual void onKey(unsigned char key, int x, int y);

	virtual openni::Status initOpenGL(int argc, char **argv);
	void initOpenGLHooks();

private:
	SampleViewer(const SampleViewer&);
	SampleViewer& operator=(SampleViewer&);

	void displayFrame(const openni::VideoFrameRef& frame);
	void switchLDPInterface(bool turnOn);

	static SampleViewer* ms_self;
	static void glutIdle();
	static void glutDisplay();
	static void glutKeyboard(unsigned char key, int x, int y);

	openni::VideoFrameRef   m_depthFrame[MAX_DEVICES];
	float			        m_pDepthHist[MAX_DEPTH];
	char			        m_strSampleName[ONI_MAX_STR];
	
	openni::RGB888Pixel*	m_pTexMap;
	unsigned int		    m_nTexMapX;
	unsigned int		    m_nTexMapY;
	int					    m_nResX;
	int					    m_nResY;

	int   		            m_depthIndex;
	int 			  m_sensortype;


    const int                   m_devNum;
    char                        (*m_serialNum)[12];
	const XnChar**              m_deviceUri;
    uint8_t                     buf1[10];
    uint8_t                     buf2[10];

    openni::Device*             m_devices;

    openni::VideoStream*        m_depthStreams;	
};


#endif // _ONI_SAMPLE_VIEWER_H_