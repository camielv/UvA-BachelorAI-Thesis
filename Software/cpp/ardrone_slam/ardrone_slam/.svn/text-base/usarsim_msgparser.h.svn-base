#pragma once

#include <string>

//#define PI 3.14159265f
#define RAD_TO_MILDEG  57295.7795f
#define MS2_TO_MG 101.971621f

using namespace std;

bool usarsim_msgparser_value(string *input, string *key, string *value);
int usarsim_msgparser_type(string *input);

int usarsim_msgparser_int(string *input, char *tag);
float usarsim_msgparser_float(string *input, char *tag);
void usarsim_msgparser_float3(string *input, char *tag, float *f);
float usarsim_msgparser_rad_to_mildeg(float f);
float usarsim_msgparser_ms2_to_mg(float f);
