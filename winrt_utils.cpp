/*++

Module Name:

winrt_utils.cpp

Abstract:

This module implements Common utilities to helt UWP application implementation.

Environment:

Universal Windows Platform application

Copyright � 2016 Vesa Eskola. All rights reserved.

--*/
#include "pch.h"

using namespace Platform;
using namespace Windows::UI::Popups;
using namespace concurrency;

#include "MasterDetailPage.xaml.h"

/*++
Routine Description:

Determines whether a path to a file system object such as a file or folder is valid.

Arguments:

fullPath - Platform string that contains the full path of the object to verify.

Return Value: bool. true if the file exists; otherwise, false
--*/
bool MasterDetailApp::PathFileExists(_In_ String^ fullPath)
{
    std::ifstream file(fullPath->Data());

    if (!file)
    {
        // Can't open file
        return false;
    }
    else
    {
        if (file.is_open())
        {
            file.close();
        }
        return true;
    }
}

/*++
Routine Description:

OBSOLATE: NOT IN USE, REMOVE
Read the binary file to allocated memory. Note: Client must free the memory allocated for the data.

Arguments:

fullPath - Platform string that contains the full path of the file to be read.

Return Value: char*. NULL if the file not read succesfully; otherwise pointer to data read from file.
--*/
char* MasterDetailApp::ReadHexFile(_In_ String^ fullPath)
{
    std::streamoff DataLen = 0;
    char * pBinData = NULL;

    std::ifstream file;
    file.open(fullPath->Data(), std::ios::binary);

    if (!file.bad())
    {
        return NULL;
    }

    // get length of file:
    file.seekg(0, std::ios::end);

    DataLen = file.tellg();
    if (DataLen > 127)
    {
        file.seekg(0, std::ios::beg);

        // allocate memory:
        pBinData = new char[(unsigned int)DataLen];

        // read data as a block:
        file.read(pBinData, DataLen);
    }
    file.close();
    return pBinData;
}

/*++
Routine Description:

Delete a file

Arguments:

fullPath - Platform string that contains the full path of the file to delete.

Return Value: bool. true if the file deleted succesfully; otherwise, false
--*/
bool MasterDetailApp::DeleteFile(_In_ String^ fullPath)
{
    return (_wremove(fullPath->Data()) == 0) ? true : false;
}

/*++
Routine Description:

Message dialog with a default close command and content.

Arguments:

message - Platform string that contains the message content

--*/
void MasterDetailApp::ShowMessageDialogAsync (_In_ String^ message)
{
    // Create the message dialog and set its content; it will get a default "Close" command since there aren't any other commands being added
    auto messageDialog = ref new MessageDialog(message);


    // Show the message dialog
    // TBD: Eliminate usage of task
    create_task(messageDialog->ShowAsync()).then([](IUICommand^ command)
    {
        ;
    });
}


