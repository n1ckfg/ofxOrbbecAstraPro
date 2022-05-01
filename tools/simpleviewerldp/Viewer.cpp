
// Undeprecate CRT functions
#ifndef _CRT_SECURE_NO_DEPRECATE 
	#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include "Viewer.h"
#include <iostream>
#include <string>
#include <sstream>

#include <GL/glut.h>
#include "OniSampleUtilities.h"



using namespace std;

#define GL_WIN_SIZE_X	800
#define GL_WIN_SIZE_Y	600
#define TEXTURE_SIZE	512

#define DEFAULT_DISPLAY_MODE	DISPLAY_MODE_DEPTH1


#define MIN_NUM_CHUNKS(data_size, chunk_size)	((((data_size)-1) / (chunk_size) + 1))
#define MIN_CHUNKS_SIZE(data_size, chunk_size)	(MIN_NUM_CHUNKS(data_size, chunk_size) * (chunk_size))


SampleViewer* SampleViewer::ms_self = NULL;

void SampleViewer::glutIdle()
{
	glutPostRedisplay();
}

void SampleViewer::glutDisplay()
{
	SampleViewer::ms_self->display();
}

void SampleViewer::glutKeyboard(unsigned char key, int x, int y)
{
	SampleViewer::ms_self->onKey(key, x, y);
}

SampleViewer::SampleViewer(const char* strSampleName, openni::VideoStream depthStreams[], int devNum) :
m_pTexMap(NULL), m_depthIndex(0), m_devNum(devNum)

{
    m_depthStreams = depthStreams;
	ms_self = this;
	strncpy(m_strSampleName, strSampleName, ONI_MAX_STR);
}

SampleViewer::~SampleViewer()
{
	delete[] m_pTexMap;

	ms_self = NULL;
}


openni::Status SampleViewer::init(int argc, char **argv, const XnChar** deviceUri, 
	                                                   char(*serialNum)[12],openni::Device& Device, int sensortype)
{
   	 m_serialNum = serialNum;
	m_deviceUri = deviceUri;
	m_devices = &Device;
	m_sensortype = sensortype; 


	m_nResX = m_depthStreams[0].getVideoMode().getResolutionX();
	m_nResY = m_depthStreams[0].getVideoMode().getResolutionY();

	// Texture map init
	m_nTexMapX = MIN_CHUNKS_SIZE(m_nResX, TEXTURE_SIZE);
	m_nTexMapY = MIN_CHUNKS_SIZE(m_nResY, TEXTURE_SIZE);
	m_pTexMap = new openni::RGB888Pixel[m_nTexMapX * m_nTexMapY];

	return initOpenGL(argc, argv); 

}


void SampleViewer::switchLDPInterface(bool turnOn)
{
	printf("%s enter ! \n",__func__);
	LaserSwitcher laserSwitcher;
	laserSwitcher.SwitchLDP(turnOn); //enable ldp
}


openni::Status SampleViewer::run()	//Does not return
{
	glutMainLoop();

	return openni::STATUS_OK;
}

void SampleViewer::displayFrame(const openni::VideoFrameRef& frame)
{
	if (!frame.isValid())
		return;

	const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)frame.getData();
	openni::RGB888Pixel* pTexRow = m_pTexMap + frame.getCropOriginY() * m_nTexMapX;
	int rowSize = frame.getStrideInBytes() / sizeof(openni::DepthPixel);

	for (int y = 0; y < frame.getHeight(); ++y)
	{
		const openni::DepthPixel* pDepth = pDepthRow;
		openni::RGB888Pixel* pTex = pTexRow + frame.getCropOriginX();

		for (int x = 0; x < frame.getWidth(); ++x, ++pDepth, ++pTex)
		{
			if (*pDepth != 0)
			{
				int nHistValue = m_pDepthHist[*pDepth];
				pTex->r = nHistValue;
				pTex->g = nHistValue;
				pTex->b = nHistValue;
			}
		}

		pDepthRow += rowSize;
		pTexRow += m_nTexMapX;
	}
}

void SampleViewer::display()
{
	// Read all frames

    for (int i = 0; i < m_devNum; ++i)
    {		
        m_depthStreams[i].readFrame(&(m_depthFrame[i]));
    }

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, GL_WIN_SIZE_X, GL_WIN_SIZE_Y, 0, -1.0, 1.0);

	memset(m_pTexMap, 0, m_nTexMapX * m_nTexMapY * sizeof(openni::RGB888Pixel));

	// Display selected frame
	int depthIndex = m_depthIndex;
	calculateHistogram(m_pDepthHist, MAX_DEPTH, m_depthFrame[depthIndex]);
	displayFrame(m_depthFrame[depthIndex]);


	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTexMapX, m_nTexMapY, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pTexMap);

	// Display the OpenGL texture map
	glColor4f(1,1,1,1);

	glBegin(GL_QUADS);

	// upper left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	// upper right
	glTexCoord2f((float)m_nResX / (float)m_nTexMapX, 0);
	glVertex2f(GL_WIN_SIZE_X, 0);
	// bottom right
	glTexCoord2f((float)m_nResX / (float)m_nTexMapX, (float)m_nResY / (float)m_nTexMapY);
	glVertex2f(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	// bottom left
	glTexCoord2f(0, (float)m_nResY / (float)m_nTexMapY);
	glVertex2f(0, GL_WIN_SIZE_Y);

	glEnd();

	// Swap the OpenGL display buffers
	glutSwapBuffers();
}

void SampleViewer::onKey(unsigned char key, int /*x*/, int /*y*/)
{		
	int ret;
	switch (key)
	{
	case 27:
        for (int i = 0; i < m_devNum; i++)
        {
            m_depthStreams[i].stop();
            m_depthStreams[i].destroy();
        }
        
	openni::OpenNI::shutdown();
	exit (1);	
        break;
	case 'a':
	printf("press the key a ---> enable ldp \n");
	switchLDPInterface(1); //enable ldp
	break;
	case 's':
	printf("press the key s ---> disable ldp \n");
	switchLDPInterface(0);//disable ldp
	break;
	default:
	break;
	}
	
}

openni::Status SampleViewer::initOpenGL(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	glutCreateWindow (m_strSampleName);
	// 	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	initOpenGLHooks();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	return openni::STATUS_OK;

}

void SampleViewer::initOpenGLHooks()
{
	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
}
