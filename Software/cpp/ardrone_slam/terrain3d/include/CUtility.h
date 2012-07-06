/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CUtility.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Utility functions

Created :  11/29/2005
Modified : 12/01/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef CUTILITY_H
#define CUTILITY_H

#include "stdafx.h"

class CUtility
{
public:
    static BOOL GetMediaFile( char* file, char path[] );
private:
    static BOOL SearchFolders( char* filename, char* exeFolder, char* exeName, char fullPath[] );
};

#endif