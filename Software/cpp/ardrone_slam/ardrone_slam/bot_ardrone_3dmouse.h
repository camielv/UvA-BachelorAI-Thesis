#pragma once
#include <Windows.h>
#include <wtypes.h>
#include <setupapi.h>
extern "C" {
#include <hidsdi.h>
}

#define MOUSE_MAX_DEVICES 10
#define MOUSE_PARSE_INTERVAL 50.0 // 50 ms

typedef struct _HidDevice {
	PSP_DEVICE_INTERFACE_DETAIL_DATA  pDevInterfaceDetailData;
	HANDLE handle;
	PHIDP_PREPARSED_DATA pPreparsedData;
	HIDP_CAPS capabilities;
	PHIDP_BUTTON_CAPS pInButCaps,pOutButCaps,pFeatButCaps;
	PHIDP_VALUE_CAPS pInValCaps,pOutValCaps,pFeatValCaps;
	char *buf;
	BOOL bGotTranslationVector, bGotRotationVector;
	int allDOFs[6];
} HidDevice;


class bot_ardrone;

static DWORD WINAPI start_behavior_thread(void* Param);

class bot_ardrone_3dmouse
{
public:
	bot_ardrone_3dmouse(bot_ardrone **b, int nr_bots);
	~bot_ardrone_3dmouse();

	static bot_ardrone **bots;
	static int nr_bots;

	void init();

private:
	int nDevices;
	double last_event;
	HANDLE thread;
	HidDevice Devices[MOUSE_MAX_DEVICES];
};