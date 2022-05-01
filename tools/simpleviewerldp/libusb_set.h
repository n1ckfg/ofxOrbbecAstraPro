#ifndef LIBUSUB_SET_H
#define LIBUSUB_SET_H
//#include <libusb-1.0/libusb.h>
#include "libusb.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>


class LaserSwitcher
{
public:
	bool SwitchLDP(int enable);
	bool SwitchLaser(int enable);

		
//private:
		
	typedef struct
	{
		uint8_t m[2];
		uint16_t l;
		uint16_t c;
		uint16_t t;
	} cam_hdr;


	int Open();
	void Close();

	int EnableLDP(uint8_t enable);
	uint16_t EnableLaser(uint32_t enable);
	uint16_t GetLaserStatus();
	uint32_t p1(uint32_t a);
	int p2(uint32_t a, uint32_t v);
	int send_usb(uint16_t c, void *buf0, uint16_t l1, void *buf1, uint16_t l2);

	uint16_t cam_tag = 0;

	libusb_device_handle *handle;
	libusb_device **devs;
	libusb_device *dev;

};

#endif

