#include "usarsim_msgparser.h"
#include "bot_ardrone.h"
#include "global.h"


bool usarsim_msgparser_value(string *input, string *key, string *value)
{
	int startpos, endpos;

	startpos = (*input).find(*key);
	if (startpos == string::npos)
		return false;

	startpos += (*key).length() + 1;

	endpos = (*input).find('}', startpos+2);
	if (endpos == string::npos)
		return false;

	*value = (*input).substr(startpos, endpos-startpos);
	return true;
}


int usarsim_msgparser_type(string *input)
{
	string value;
	int sensor = BOT_ARDRONE_SENSOR_UNKNOW;

	if (usarsim_msgparser_value(input, &string("{Type"), &value))
	{
		if (value == "GroundTruth")
			sensor = BOT_ARDRONE_SENSOR_GT;
		else if (value == "IMU")
			sensor = BOT_ARDRONE_SENSOR_IMU;
		else if (value == "Sonar")
			sensor = BOT_ARDRONE_SENSOR_SONAR;
		else if (value == "Accel")
			sensor = BOT_ARDRONE_SENSOR_ACCEL;
	}

	return sensor;
}


int usarsim_msgparser_int(string *input, char *tag)
{
	string value;

	if (usarsim_msgparser_value(input, &string(tag), &value))
		return atoi(value.c_str());
	else
		return 0;
}


float usarsim_msgparser_float(string *input, char *tag)
{
	string value;

	if (usarsim_msgparser_value(input, &string(tag), &value))
		return (float)atof(value.c_str());
	else
		return 0.0;
}


void usarsim_msgparser_float3(string *input, char *tag, float *f)
{
	string value;
	int i;
	int pos;
	int offset = 0;

	if (usarsim_msgparser_value(input, &string(tag), &value))
	{
		for (i=0; i < 2; i++)
		{
			pos = value.find(',', offset);

			f[i] = (float)atof(value.substr(offset, pos-offset).c_str());
			offset = pos + 1;
		}

		f[i] = (float)atof(value.substr(offset).c_str());
	}
}


float usarsim_msgparser_rad_to_mildeg(float f)
{
	if (f > M_PI)
		f -= 2*M_PI;

	f *= RAD_TO_MILDEG;

	return f;
}


float usarsim_msgparser_ms2_to_mg(float f)
{
	return f * MS2_TO_MG;
}