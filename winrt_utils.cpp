/*++

Module Name:

winrt_utils.cpp

Abstract:

This module implements Common utilities to helt UWP application implementation.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

--*/
#include "pch.h"

using namespace Platform;
using namespace Windows::UI::Popups;
using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;

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

/*++
Routine Description:

Let user to pick image file, copy it to app storage folder

Return Value: StorageFile^. StorageFile pointing to copied image file.
--*/
StorageFile^ MasterDetailApp::PickImage()
{
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::Thumbnail;
    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    openPicker->FileTypeFilter->Append(".jpg");
    openPicker->FileTypeFilter->Append(".png");
    //openPicker->FileTypeFilter->Append(".sqlite");

    create_task(openPicker->PickSingleFileAsync()).then([](StorageFile^ file)
    {
        if (file != nullptr)
        {
            create_task(file->CopyAsync(ApplicationData::Current->LocalFolder, file->Name, NameCollisionOption::ReplaceExisting)).then([](StorageFile^ newFile)
            {
                String^ dbFilePath = newFile->Path;
                return newFile;
            });
        }
    });
    return nullptr;
}

/*++
Routine Description:

Convert integer value to Plaform string using roundation rules (100 51 -> "100.51")
Return Value: String^. String containing the converted value
--*/
Platform::String^ MasterDetailApp::IntToPlatformString (int value)
{
    std::wstring wideString = std::to_wstring(value / (float)100.0);
    std::replace(wideString.begin(), wideString.end(), ',', '.');
    wideString.erase(wideString.find_last_of('.') + 3, std::string::npos);

    return ref new Platform::String(wideString.c_str());
}

/*++
Routine Description:

Convert Plaform string to integer using roundation rules ("100.5562" -> "100.56")
Return Value: int. Integer containing the converted value
--*/
int MasterDetailApp::PlatformStringToInt(Platform::String^ Value)
{
    // Conversion from text -> float -> 100 x int
    std::wstring wideString = Value->Data();
    std::replace(wideString.begin(), wideString.end(), ',', '.');
    float famount = wcstof(wideString.c_str(), NULL);
    return (int)((famount + 0.005) * 100.0);
}
