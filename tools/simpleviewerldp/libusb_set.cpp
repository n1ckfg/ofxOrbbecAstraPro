#include "libusb_set.h"

int LaserSwitcher::send_usb(uint16_t c, void *buf0, uint16_t l1, void *buf1, uint16_t l2)
{
	//printf("%s enter\n",__func__);
	unsigned int al = 0;
	
	int res;
	uint8_t ob[0x400];
	uint8_t ib[0x200];
	cam_hdr *chdr = (cam_hdr*)ob;
	cam_hdr *rhdr = (cam_hdr*)ib;

	if (handle == 0) return -1;

	if (l1 & 1 || l1 > (0x400 - sizeof(*chdr))) {
		return -1;
	}

	chdr->m[0] = 0x47;
	chdr->m[1] = 0x4d;
	chdr->c = c;
	chdr->t = cam_tag;
	chdr->l = l1 / 2;

	//copy the cmdbuf
	memcpy(ob + sizeof(*chdr), buf0, l1);
	res = libusb_control_transfer(handle,
		LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
		0x00,
		0x0000,
		0x0000,
		(unsigned char *)ob,
		l1 + sizeof(*chdr),
		1000);

	if (res < 0)
	{
		printf("send failed (%d)\n", res);
		return res;
	}

	do
	{
		al = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR, 0x00, 0x0000, 0x0000, (unsigned char *)ib, 0x200, 1000);
	} while ((al == 0) || (al == 0x200));

	if (al < (int)sizeof(*rhdr)) {
		return res;
	}
	al -= sizeof(*rhdr);

	if (rhdr->m[0] != 0x52 || rhdr->m[1] != 0x42) {
		printf("-1\n");
		return -1;
	}
	if (rhdr->c != chdr->c) {
		printf("-2\n");
		return -2;
	}
	if (rhdr->t != chdr->t) {
		printf("-3\n");
		return -3;
	}
	if (rhdr->l != (al / 2)) {
		printf("-4\n");
		return -4;
	}

	if (al > l2) {
		memcpy(buf1, ib + sizeof(*rhdr), l2);
	}
	else {
		memcpy(buf1, ib + sizeof(*rhdr), al);
	}

	cam_tag++;

	return al;

}

uint32_t LaserSwitcher::p1(uint32_t a)
{
	uint32_t v;
	uint32_t buf1[10];
	uint8_t buf2[10];

	if (a % 4 != 0)
	{
		printf("a is illegal\n");
		return -1;
	}

	*(uint32_t*)buf1 = a;
	send_usb(83, buf1, 4, buf2, 6);
	v = *(uint32_t*)&buf2[2];
	return v;

}


int LaserSwitcher::p2(uint32_t a, uint32_t v)
{
	uint8_t buf1[10];
	uint8_t buf2[10];

	*(uint32_t*)buf1 = a;
	*(uint32_t*)&buf1[4] = v;
	send_usb(84, buf1, 8, buf2, 2);
	return 0;
}


int LaserSwitcher::EnableLDP(uint8_t enable)
{
	printf("%s enter\n",__func__);
	uint32_t *b = (uint32_t *)0x50005000;
	uint32_t r, _d;

	_d = enable & 0x01;

	r = p1((uint32_t)(b + 6));
	r = (r & (~(1 << 7))) | ((_d << 7));
	p2((uint32_t)(b + 6), r);
	p2((uint32_t)(b + 7), ~r);

	return 0;
}

void LaserSwitcher::Close()
{
	// TODO: 
	 
	if(handle!=NULL){
		libusb_close(handle);
	}
	//libusb_uninit
}


int LaserSwitcher::Open()
{
	int res = 0;
	int found = 0;
	int cnt = 0;
	int i = 0;

	printf("%s enter\n",__func__);

	res = libusb_init(NULL); // TODO: check res

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
	{
		libusb_free_device_list(devs, 1);
		return -1;
	}

	struct libusb_device_descriptor desc;

	while ((dev = devs[i++]) != NULL)
	{

		res = libusb_get_device_descriptor(dev, &desc);
		if (res < 0)
		{
			printf("failed to get device descriptor");
			libusb_free_device_list(devs, 1);
			return -1;
		}
		
		printf("VID %04x:PID %04x (bus %d, device %d)\n", desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		if (desc.idVendor == 0x2bc5)
		{
			cam_tag = 0;
			found = 1;
			printf("find device successfully\r\n");
			break;
		}
	}

	if (found == 0)
	{
		printf("find device failed\r\n");
		return -1;
	}

	res = libusb_open(dev, &handle);
	if (res != 0)
	{
		printf("open device failed\r\n");
		return -1;
	}

	return 0;
}

bool LaserSwitcher::SwitchLDP(int enable)
{
	printf("%s enter\n",__func__);
	int rlt = Open();
	if (rlt < 0)
		return false;

	EnableLDP(enable);

	Close();

	return true;
}

