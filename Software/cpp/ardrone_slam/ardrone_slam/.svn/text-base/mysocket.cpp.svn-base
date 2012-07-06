#include "global.h"
#include "mysocket.h"
#include "windows.h"
#include "botinterface.h"

bool mysocket::WSARunning = false;
int mysocket::nr_open_sockets = false;

mysocket::mysocket(int id, int PortNo, char *IPAddress, char *argbuffer, int buffersize, botinterface *i)
{
	bool result;
	this->id = id;
	this->i = i;
	connected = false;
	this->buffersize = buffersize;
	if (!argbuffer)
	{
		use_local_buffer = true;
		buffer = NULL;
	}
	else
	{
		use_local_buffer = false;
		buffer = argbuffer;
	}

	result = open(PortNo, IPAddress);

	if (result)
	{
		connected = true;
		Sleep(10);
		start_listener();
	}
}


mysocket::~mysocket(void)
{
	close();
}


int mysocket::send(char *message)
{
	//printf("send: %s\n", message);
	return ::send(s, message, strlen(message), 0);
}


static DWORD WINAPI receive_thread(void* Param)
{
	mysocket* This = (mysocket*) Param; 

	int bytes;
	bytes = SOCKET_ERROR;

	if (This->use_local_buffer)
		This->buffer = new char[This->buffersize];

	while(bytes = recv(This->s, This->buffer, This->buffersize, 0))
	{
		if(bytes == -1)
			return 0;

		This->i->socket_callback(This->id, This->buffer, bytes);
	}

	return 0;
}

bool mysocket::bytes_waiting()
{
	u_long iMode = 1;
	int bytes = SOCKET_ERROR;
	char buffer;

	ioctlsocket(s, FIONBIO, &iMode);
	bytes = recv(s, &buffer, 1, MSG_PEEK);
	iMode = 0;
	ioctlsocket(s, FIONBIO, &iMode);

	return (bytes == 1);
}


int mysocket::start_listener(void)
{
	h = CreateThread( NULL, 0, receive_thread, (void*) this, 0, NULL);

	return 1;
}


bool mysocket::open(int PortNo, char* IPAddress)
{
	if (!WSARunning)
		winsock_start();

     //Fill out the information needed to initialize a socket�

    SOCKADDR_IN target; //Socket address information

    target.sin_family = AF_INET; // address family Internet
    target.sin_port = htons (PortNo); //Port to connect on
    target.sin_addr.s_addr = inet_addr (IPAddress); //Target IP

    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket

    if (s == INVALID_SOCKET)
    {
        return false; //Couldn't create the socket

    }  

    //Try connecting...
    if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
    {
        return false; //Couldn't connect
    }
    else
	{
		nr_open_sockets++;
        return true; //Success
	}
}


void mysocket::close()
{
    //Close the socket if it exists
    if (s) {
        closesocket(s);
		connected = false;

		if (--nr_open_sockets <= 0)
			winsock_stop();
	}
}

bool mysocket::winsock_start()
{
	if (WSARunning)
		return false;

   //Start up Winsock
    WSADATA wsadata;

    int error = WSAStartup(0x0202, &wsadata);
    //Did something happen?

    if (error)
        return false;

    //Did we get the right Winsock version?
    if (wsadata.wVersion != 0x0202)
    {
        WSACleanup(); //Clean up Winsock
        return false;
    }

	return true;
}


void mysocket::winsock_stop()
{
	if (WSARunning && nr_open_sockets <= 0) {
		WSACleanup();
		nr_open_sockets = 0;
	}
}