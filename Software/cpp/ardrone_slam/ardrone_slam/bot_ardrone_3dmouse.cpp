#include "bot_ardrone_3dmouse.h"
#include <string>
#include "bot_ardrone.h"
#include "global.h"
#include "opencv_helpers.h"

#include <stdio.h>
#include <malloc.h>


using namespace std;
using namespace cv;


bot_ardrone** bot_ardrone_3dmouse::bots	= NULL;
int bot_ardrone_3dmouse::nr_bots			= 0;

static DWORD WINAPI start_3dmouse_thread(void* Param)
{
	bot_ardrone_3dmouse *instance = (bot_ardrone_3dmouse*) Param; 
	
	instance->init();

	return 1;
}


bot_ardrone_3dmouse::bot_ardrone_3dmouse(bot_ardrone **b, int nr_bots)
{
	bot_ardrone_3dmouse::bots		= b;
	bot_ardrone_3dmouse::nr_bots	= nr_bots;

	nDevices = 0;
	last_event = 0.0;

	thread	= CreateThread(NULL, 0, start_3dmouse_thread, (void*) this, 0, NULL);
}


bot_ardrone_3dmouse::~bot_ardrone_3dmouse()
{
}


void bot_ardrone_3dmouse::init()
{
	Mat Mvel(3, 1, CV_32F);
	Mat Mor(3, 1, CV_32F);
	Mat Mrot(3, 3, CV_32F);
	Mor = 0.0f;
	float *state;

	GUID hidGuid;

	HidD_GetHidGuid( &hidGuid );
	HDEVINFO hDevInfo = SetupDiGetClassDevs( &hidGuid, NULL, NULL, 
		                                     DIGCF_DEVICEINTERFACE | DIGCF_PRESENT | DIGCF_PROFILE );

	int memberIndex=0;
	SP_DEVICE_INTERFACE_DATA  DevInterfaceData;
	DevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	while(SetupDiEnumDeviceInterfaces( hDevInfo, NULL, &hidGuid, memberIndex, &DevInterfaceData ))
	{
		DWORD requiredSize;

		// First call, just gets the size so we can malloc enough memory for the detail struct
		SetupDiGetDeviceInterfaceDetail( hDevInfo, &DevInterfaceData, NULL, 0, &requiredSize, NULL );

		// Second call gets the data
		Devices[nDevices].pDevInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc( requiredSize );
		Devices[nDevices].pDevInterfaceDetailData->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );
		SetupDiGetDeviceInterfaceDetail( hDevInfo, 
										 &DevInterfaceData,
										 Devices[nDevices].pDevInterfaceDetailData,
										 requiredSize,
										 NULL,
										 NULL );

		// Open this device
		Devices[nDevices].handle = CreateFile(Devices[nDevices].pDevInterfaceDetailData->DevicePath,
												GENERIC_READ | GENERIC_WRITE,
												FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL,
												OPEN_EXISTING,
												FILE_ATTRIBUTE_NORMAL,
												NULL);
		if ( Devices[nDevices].handle == NULL)
		{
			//printf("Error opening file \"%s\"\n",Devices[nDevices].pDevInterfaceDetailData->DevicePath);
			goto next;
		}

		// Get the preparsedData struct
		if (HidD_GetPreparsedData( Devices[nDevices].handle, &Devices[nDevices].pPreparsedData ) != TRUE)
		{
			//printf("Error getting PreparsedData\n");
			goto next;
		}

		if ( HidP_GetCaps( Devices[nDevices].pPreparsedData, &Devices[nDevices].capabilities ) != HIDP_STATUS_SUCCESS )
		{
			//printf("Error getting capabilities\n");
			goto next;
		}

next:
		memberIndex++;
		if (++nDevices == MOUSE_MAX_DEVICES) {--nDevices;}
	} // end of while(SetupDiEnumDeviceInterfaces) loop

	// Done with devInfo list.  Release it.
	SetupDiDestroyDeviceInfoList( hDevInfo );


	// Read data from each usagePage==1, usage==8 device
	int listIndex, waitListIndex = 0;
	HANDLE *waitListHandles = (HANDLE *)malloc(nDevices * sizeof(HANDLE ));
	HidDevice **waitListItems  = (HidDevice **)malloc(nDevices * sizeof(HidDevice*));
	for (listIndex=0; listIndex < nDevices; listIndex++)
	{
		if (Devices[listIndex].capabilities.UsagePage == 1 && Devices[listIndex].capabilities.Usage == 8)
		{
			// Add its handle and a pointer to the waitList
			waitListHandles[waitListIndex] = Devices[listIndex].handle;
			HidDevice *pDev = Devices + listIndex;
			waitListItems  [waitListIndex] = pDev;
			pDev->buf = (char *)malloc(pDev->capabilities.InputReportByteLength );
			ZeroMemory( pDev->buf, pDev->capabilities.InputReportByteLength );
			pDev->bGotTranslationVector = 
			pDev->bGotRotationVector    = FALSE;
			waitListIndex++;
		}
	}

	printf("3DMouse: found %i 3D mouses\n", waitListIndex);

	if (waitListIndex > 0)
	{
		DWORD nread, waitResult;

		BOOL bRun = TRUE;
		while(bRun)
		{
			// This only wakes for one of the HID devices.  Not sure why.
			waitResult = WaitForMultipleObjects( waitListIndex, waitListHandles, FALSE, INFINITE);

			// a HID device event
			if (waitResult < WAIT_OBJECT_0 + waitListIndex )
			{
				int index = waitResult - WAIT_OBJECT_0;
				HidDevice *pDev = waitListItems[index];
				if (ReadFile(pDev->handle, 
							 pDev->buf, 
						     pDev->capabilities.InputReportByteLength, 
						     &nread, 
						     FALSE))
				{


				switch ( pDev->buf[0] )
				{
					case 0x01:  
						if (nread != 7) break; // something is wrong
						//printf("Device %d Got Translation vector\n", index);
						pDev->bGotTranslationVector = TRUE; 
						pDev->allDOFs[0] = (pDev->buf[1] & 0x000000ff) | ((int)pDev->buf[2]<<8 & 0xffffff00); 
						pDev->allDOFs[1] = (pDev->buf[3] & 0x000000ff) | ((int)pDev->buf[4]<<8 & 0xffffff00); 
						pDev->allDOFs[2] = (pDev->buf[5] & 0x000000ff) | ((int)pDev->buf[6]<<8 & 0xffffff00);
						break;

					case 0x02:  
						if (nread != 7) break; // something is wrong
						//printf("Device %d Got Rotation vector\n", index);
						pDev->bGotRotationVector    = TRUE; 
						pDev->allDOFs[3] = (pDev->buf[1] & 0x000000ff) | ((int)pDev->buf[2]<<8 & 0xffffff00); 
						pDev->allDOFs[4] = (pDev->buf[3] & 0x000000ff) | ((int)pDev->buf[4]<<8 & 0xffffff00); 
						pDev->allDOFs[5] = (pDev->buf[5] & 0x000000ff) | ((int)pDev->buf[6]<<8 & 0xffffff00);
						break;

					case 0x03:  // Buttons (display most significant byte to least)
						//printf("Device %d Button mask: %.2x %.2x %.2x\n",index,(unsigned char)pDev->buf[3],(unsigned char)pDev->buf[2],(unsigned char)pDev->buf[1]);
						switch ( pDev->buf[1] )
						{
						case 0x01:  // Stop reading on button 1
							for(int i=0; i<bot_ardrone_3dmouse::nr_bots; i++)
							{
								if (bot_ardrone_3dmouse::bots[i]->control.state == BOT_STATE_LANDED)
									bot_ardrone_3dmouse::bots[i]->take_off();
								else
									bot_ardrone_3dmouse::bots[i]->land();
							}
							break;
						default:
							break;
						}
						break;

					default:
						break;
				}

				// Translation and Rotation values come in two different packets.  Wait until we have both of them.
				if ( pDev->bGotTranslationVector && pDev->bGotRotationVector )
				{
					if ((clock() - last_event) > MOUSE_PARSE_INTERVAL)
					{
						last_event = clock();

						pDev->bGotTranslationVector = pDev->bGotRotationVector = FALSE;
						/*
						printf( "Device %d, all 6 DOF: T: (%d, %d, %d) R: (%d, %d, %d)\n", index, 
							pDev->allDOFs[0], pDev->allDOFs[1], pDev->allDOFs[2], pDev->allDOFs[3], pDev->allDOFs[4], pDev->allDOFs[5]);
						*/

						for(int i=0; i<bot_ardrone_3dmouse::nr_bots; i++)
						{
							state = bot_ardrone_3dmouse::bots[i]->slamcontroller->get_state();

							Mvel.at<float>(0) = (float) pDev->allDOFs[1] / -350.0f;
							Mvel.at<float>(1) = (float) pDev->allDOFs[0] / 350.0f;
							Mvel.at<float>(2) = (float) pDev->allDOFs[2] / -350.0f;

							Mor.at<float>(2) = -state[11];
							cv::RotationMatrix3D(Mor, Mrot);

							Mvel = Mrot * Mvel;

							bot_ardrone_3dmouse::bots[i]->control_set(BOT_ARDRONE_Velocity, BOT_ARDRONE_LinearVelocity, Mvel.at<float>(0));
							bot_ardrone_3dmouse::bots[i]->control_set(BOT_ARDRONE_Velocity, BOT_ARDRONE_LateralVelocity, Mvel.at<float>(1));
							bot_ardrone_3dmouse::bots[i]->control_set(BOT_ARDRONE_Velocity, BOT_ARDRONE_AltitudeVelocity, Mvel.at<float>(2));
							bot_ardrone_3dmouse::bots[i]->control_set(BOT_ARDRONE_Velocity, BOT_ARDRONE_RotationalVelocity, (float) pDev->allDOFs[5] / 350.0f);
							bot_ardrone_3dmouse::bots[i]->control_update();
						}
					}
				}

				}
			}
		}
	}
}