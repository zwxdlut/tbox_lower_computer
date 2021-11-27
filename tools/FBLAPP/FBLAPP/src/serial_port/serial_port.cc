#include <stdio.h>
#include <assert.h>

#include "serial_port.h"

serial_port::serial_port()
{
	handle_ = INVALID_HANDLE_VALUE;
}

serial_port::~serial_port()
{
	close();
}

int32_t serial_port::open(const std::string& _chl, const uint32_t _baud_rate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity)
{
	// open the serial port
	std::wstring wstr;
	wstr.assign(_chl.begin(), _chl.end());
	handle_ = CreateFile(wstr.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (handle_ == INVALID_HANDLE_VALUE)
	{
		printf("serial_port::open: open the serial port(%s) failed!\n", _chl.c_str());
		return -1;
	}

	// get and set the communication parameters
	DCB dcb;
	GetCommState(handle_, &dcb);
	dcb.BaudRate = _baud_rate;
	dcb.Parity = _parity;
	dcb.ByteSize = _data_bits;
	dcb.StopBits = _stop_bits;
	if (!SetCommState(handle_, &dcb))
	{
		printf("serial_port::open: set the serial port(%s) params failed!\n", _chl.c_str());
		return -1;
	}

	// set the R/W buffer
	SetupComm(handle_, SERIAL_PORT_BUFFER_SIZE, SERIAL_PORT_BUFFER_SIZE);

	// clear
	PurgeComm(handle_, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	// set the R/W time
	COMMTIMEOUTS time_out;
	GetCommTimeouts(handle_, &time_out);
	time_out.ReadIntervalTimeout = MAXDWORD;
	time_out.ReadTotalTimeoutMultiplier = 0;
	time_out.ReadTotalTimeoutConstant = 0;
	time_out.WriteTotalTimeoutMultiplier = 10;
	time_out.WriteTotalTimeoutConstant = 1000;

	if (!SetCommTimeouts(handle_, &time_out))
	{
		printf("serial_port::open: set the serial port(%s) timeouts failed!\n", _chl.c_str());
		return -1;
	}

	return 0;
}

int32_t serial_port::close()
{
	if (INVALID_HANDLE_VALUE != handle_)
	{
		CloseHandle(handle_);
	}

	return 0;
}

uint16_t serial_port::receive(uint8_t _buf[], const uint16_t _size)
{
	assert(NULL != _buf);

	DWORD size = 0;

	if (!ReadFile(handle_, _buf, _size, &size, NULL))
	{
		COMSTAT com_state;
		unsigned long error_flag;

		ClearCommError(handle_, &error_flag, &com_state);
		printf("serial_port::receive: read data failed!(_size = %d, out size = %d, cbInQue = %d)\n", _size, size, com_state.cbInQue);
	}

	return (uint16_t)size;
}

uint16_t serial_port::send(const uint8_t _buf[], const uint16_t _size)
{
	assert(NULL != _buf);

	DWORD size = 0;

	std::lock_guard<std::recursive_mutex> guard(tx_mutex_);

	if (!WriteFile(handle_, _buf, _size, &size, NULL) || size != _size)
	{
		printf("serial_port::send: write data failed!(_size = %d, sent size = %d)\n", _size, size);
	}

	return (uint16_t)size;
}

uint16_t serial_port::receive_with_header_poll(uint8_t _buf[], const uint16_t _size)
{
	assert(NULL != _buf);

	uint16_t size = 0;
	uint16_t out_size = 0;

	// receive 0xAA
	size = 1;

	if (size != receive(_buf, size) || (0xFF & (HEADER_FLAG >> 8)) != _buf[0])
	{
		return 0;
	}

	// receive 0x55
	size = 1;
	out_size = 0;

	while (out_size < size)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		out_size += receive(_buf + out_size, size - out_size);
	}

	if ((HEADER_FLAG & 0xFF) != _buf[0])
	{
		return 0;
	}

	// receive data size
	size = HEADER_SIZE - 2;
	out_size = 0;

	while (out_size < size)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		out_size += receive(_buf + out_size, size - out_size);
	}
		
	// receive data
	memcpy(&size, _buf, 2);
	size = _size > size ? size : _size;
	out_size = 0;

	while (out_size < size)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		out_size += receive(_buf + out_size, size - out_size);
	}
	
	return size;
}

uint16_t serial_port::send_with_header(const uint8_t _buf[], const uint16_t _size)
{
	uint16_t size = 0;

	std::lock_guard<std::recursive_mutex> guard(tx_mutex_);
#if defined (HEADER_FLAG) && defined (HEADER_SIZE)
	uint8_t	header[HEADER_SIZE] = { 0u };
	header[0] = HEADER_FLAG >> 8;
	header[1] = HEADER_FLAG & 0xFFu;
	header[2] = 0xFFu & _size;
	header[3] = _size >> 8;
	size = send(header, HEADER_SIZE);
#endif
	size += send(_buf, _size);

	return size;
}