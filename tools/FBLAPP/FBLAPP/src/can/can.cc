#include <stdio.h>
#include <assert.h>

#include "can.h"

#pragma comment(lib, "src/can/ECanVci.lib")

can::can(const uint8_t _dev_type, const uint8_t _dev_idx)
	: dev_type_(_dev_type)
	, dev_idx_(_dev_idx)
{
}

can::~can(void)
{
	close();
}

int32_t can::open(void)
{
	if (STATUS_OK != OpenDevice(dev_type_, dev_idx_, 0))
	{
		printf("can::open: Open the CAN device(%d, %d) failed!\n", dev_type_, dev_idx_);
		return -1;
	}
		
	return 0;
}

int32_t can::close(void)
{
	if (STATUS_OK != CloseDevice(dev_type_, dev_idx_))
	{
		printf("can::close: Close the CAN device(%d, %d) failed!\n", dev_type_, dev_idx_);
		return -1;
	}

	return 0;
}

int32_t can::init(const uint8_t _chl)
{
	INIT_CONFIG cfg;
	cfg.AccCode = 0;
	cfg.AccMask = 0xffffff;
	cfg.Filter = 0;
	cfg.Timing0 = 0;
	cfg.Timing1 = 0x1c;
	cfg.Mode = 0;

	if (STATUS_OK != InitCAN(dev_type_, dev_idx_, _chl, &cfg))
	{
		printf("can::init: Initialize the CAN(%d) failed!\n", _chl);
		return -1;
	}
		
	if (STATUS_OK != StartCAN(dev_type_, dev_idx_, _chl))
	{
		printf("can::init: Start the CAN(%d) failed!\n", _chl);
		return -1;
	}
		
	return 0;
}

uint8_t can::receive(const uint8_t _chl, uint32_t* const _id, uint8_t _buf[], const uint8_t _size)
{
	assert(NULL != _buf);

	CAN_OBJ frame;
	uint8_t size = 0;

	if (1 != Receive(dev_type_, dev_idx_, _chl, &frame, 1, 0))
		return 0;

	*_id = frame.ID;
	size = _size > frame.DataLen ? frame.DataLen : _size;
	memcpy(_buf, frame.Data, size);

	return size;
}

uint8_t can::send(const uint8_t _chl, const uint32_t _id, const uint8_t _buf[], const uint8_t _size)
{
	assert(NULL != _buf);

	CAN_OBJ frame;

	memset(&frame, 0, sizeof(CAN_OBJ));
	frame.ID = _id;
	frame.SendType = 0;
	frame.RemoteFlag = 0;
	frame.ExternFlag = 0;
	frame.DataLen = _size > 8 ? 8 : _size;
	memcpy(frame.Data, _buf, frame.DataLen);
	std::lock_guard<std::mutex> guard(tx_mutex_);
	if (1 != Transmit(dev_type_, dev_idx_, _chl, &frame, 1))
	{
		printf("can::send: Transmit failed!\n");
		return 0;
	}

	return _size;
}
