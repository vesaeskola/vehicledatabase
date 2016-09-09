/*++

Module Name:

winrt_utils.h

Abstract:

This module implements Common utilities to help UWP application implementation.

Environment:

Universal Windows Platform application

Copyright � 2016 Vesa Eskola. All rights reserved.

--*/


#pragma once

#include "pch.h"

namespace MasterDetailApp
{
    /*++
    Routine Description:

    Determines whether a path to a file system object such as a file or folder is valid.

    Arguments:

    fullPath - Platform string that contains the full path of the object to verify.

    Type: bool

    true if the file exists; otherwise, false
    --*/
    bool PathFileExists(_In_ Platform::String^ fullPath);

    /*++
    Routine Description:

    OBSOLATE: NOT IN USE, REMOVE
    Read the binary file to allocated memory. Note: Client must free the memory allocated for the data. 

    Arguments:

    fullPath - Platform string that contains the full path of the file to be read.

    Type: Pointer to binary data

    NULL if the file not read succesfully; otherwise pointer to read data.
    --*/
    char* ReadHexFile(_In_ Platform::String^ fullPath);

    /*++
    Routine Description:

    Delete a file

    Arguments:

    fullPath - Platform string that contains the full path of the file to delete.

    Type: bool

    true if the file deleted succesfully; otherwise, false
    --*/
    bool DeleteFile(_In_ Platform::String^ fullPath);

    /*++
    Routine Description:

    Message dialog with a default close command and content.

    Arguments:

    message - Platform string that contains the message content

    Type: bool

    true if the file exists; otherwise, false
    --*/
    void ShowMessageDialogAsync(_In_ Platform::String^ message);

}

