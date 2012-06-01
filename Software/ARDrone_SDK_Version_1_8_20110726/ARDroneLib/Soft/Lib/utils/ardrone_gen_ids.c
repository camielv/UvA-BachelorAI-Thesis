#include <utils/ardrone_gen_ids.h>
#include <utils/ardrone_crc_32.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include <time.h>

void
ardrone_gen_appid (const char *appName, const char *sdkVersion, char appId [9], char *appDesc, int descLen)
{
  uint32_t binaryId;
#define _BUFFER_SIZE 512
  char appNamePlusSdk [_BUFFER_SIZE] = {0};
  sprintf_s (appNamePlusSdk, _BUFFER_SIZE, "%s:%s", appName, sdkVersion);
#undef _BUFFER_SIZE
  binaryId = ardrone_crc_32 ((uint8_t *)appNamePlusSdk, strlen (appNamePlusSdk));
  sprintf_s (appId, 9, "%08x", binaryId);
  appId [8] = '\0';
  strncpy (appDesc, appName, descLen);
}

void
ardrone_gen_usrid (const char *usrName, char usrId [9], char *usrDesc, int descLen)
{
  uint32_t binaryId = ardrone_crc_32 ((uint8_t *)usrName, strlen (usrName));
  sprintf_s (usrId, 9, "%08x", binaryId);
  usrId [8] = '\0';
  strncpy (usrDesc, usrName, descLen);
}

void
ardrone_gen_sessionid (char sessId [9], char *sessDesc, int descLen)
{
  uint32_t binaryId;

  static int runOnce = 1;
  if (1 == runOnce)
    {
      srand ((unsigned int) time (NULL));
      runOnce = 0;
    }


  binaryId = rand ();
  binaryId <<= 15;
  binaryId += rand ();
  binaryId <<= 15;
  binaryId += rand ();

  //binaryId = (0 != binaryId) ? binaryId : 1u;
  sprintf_s (sessId, 9, "%08x", binaryId);
  sessId [8] = '\0';
  sprintf_s (sessDesc, descLen, "Session %s", sessId);
}
