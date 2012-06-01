#pragma once

#include <winsock.h>
#include <windows.h>

class botinterface;

#define SCK_VERSION1            0x0101
#define SCK_VERSION2            0x0202

static DWORD WINAPI receive_thread(void* Param);

class mysocket
{
public:
	mysocket::mysocket(int id, int PortNo, char *IPAddress, char *argbuffer, int buffersize, botinterface *i);
	~mysocket(void);
	int send(char *message);
	bool mysocket::bytes_waiting();

	int id;
	bool connected;
	botinterface *i;
	SOCKET s;
	bool use_local_buffer;
	char *buffer;
	int buffersize;
	static bool WSARunning;
	static int nr_open_sockets;

private:
	int start_listener(void);
	bool open(int PortNo, char *IPAddress);
	void close();
	bool mysocket::winsock_start();
	void mysocket::winsock_stop();

	HANDLE h;
};

