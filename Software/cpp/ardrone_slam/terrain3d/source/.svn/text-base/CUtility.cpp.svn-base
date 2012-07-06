#include "..\include\stdafx.h"
#include "..\include\CUtility.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Searches through folders to find a media file. Adapted from the DirectX Sample Framework.
Parameters:
[in] file - Name of the file we're looking for.
[in/out] path - If file is found, filled with full path to the desired file.
Returns: TRUE if the file was found, FALSE otherwise
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CUtility::GetMediaFile( char* file, char path[] )
{
    char exeName[MAX_PATH] = {0};
    char exeFolder[MAX_PATH] = {0};

    // Get full executable path
    GetModuleFileName( NULL, exeFolder, MAX_PATH );
    exeFolder[MAX_PATH - 1] = 0;

    // Get pointer to beginning of executable file name
    // which is after the last slash
    char* pCutPoint = NULL;
    for ( int i = 0; i < MAX_PATH; i++ )
    {
        if ( exeFolder[i] == '\\' )
        {
            pCutPoint = &exeFolder[i + 1];
        }
    }

    if ( pCutPoint )
    {
        // Copy over the exe file name
        strcpy_s( exeName, MAX_PATH, pCutPoint );

        // Chop off the exe file name from the path so we
        // just have the exe directory
        *pCutPoint = 0;

        // Get pointer to start of the .exe extension 
        pCutPoint = NULL;
        for ( int i = 0; i < MAX_PATH; i++ )
        {
            if ( exeName[i] == '.' )
            {
                pCutPoint = &exeName[i];
            }
        }
        // Chop the .exe extension from the exe name
        if ( pCutPoint )
        {
            *pCutPoint = 0;
        }

        // Add a slash
        strcat_s( exeName, MAX_PATH, "\\" );
    }

    // Search all the folders in searchFolders
    if ( SearchFolders( file, exeFolder, exeName, path ) )
    {
        return TRUE;
    }

    // Search all the folders in searchFolders with media\ appended to the end
    char mediaFile[MAX_PATH] = "media\\";
    strcat_s( mediaFile, MAX_PATH, file );
    if ( SearchFolders( mediaFile, exeFolder, exeName, path ) )
    {
        return TRUE;
    }

    return FALSE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Searches through folders to find a media file. Adapted from the DirectX Sample Framework.
Parameters:
[in] filename - File we are looking for
[in] exeFolder - Folder of the executable
[in] exeName - Name of the executable
[in/out] fullPath - Returned path if file is found.
Returns: TRUE if the file was found, FALSE otherwise.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CUtility::SearchFolders( char* filename, char* exeFolder, char* exeName, char fullPath[] )
{
    char* searchFolders[] = 
    { 
        ".\\", "..\\", "..\\..\\", "%s", "%s..\\", "%s..\\..\\", "%s..\\%s", "%s..\\..\\%s"
    };

    // Look through each folder to find the file
    char currentPath[MAX_PATH] = {0};
    for ( int i = 0; i < 8; i++ )
    {
        sprintf_s( currentPath, MAX_PATH, searchFolders[i], exeFolder, exeName );
        strcat_s( currentPath, MAX_PATH, filename );
        if ( GetFileAttributes( currentPath ) != INVALID_FILE_ATTRIBUTES )
        {
            strcpy_s( fullPath, MAX_PATH, currentPath );
            return TRUE;
        }
    }

    // Crap...didn't find it
    return FALSE;
}
