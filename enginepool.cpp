/*++

Module Name:

enginepool.cpp

Abstract:

This module implements EnginePool. Purpose is to instantiate and maintain all the engine
objects. Engine pool provide access of engine objects for the page object when ever requested.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/


#include "pch.h"

using namespace Platform;
using namespace MasterDetailApp;
using namespace MasterDetailApp::ViewModels;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Storage;


// TBD: How I can add these as member variables of MasterDetailPage, or how I can assing this pointer
// for the other page objects ?
DatabaseEngine^ gDatabase;
VehicleListItemCollector^ gVehicleListItemCollector;   // Receiver for database events, store vehicle info into collector array
ActionListItemCollector^ gActionListItemCollector;     // Receiver for database events, store action info into collector array

Vector<VehicleListItem^>^ gVehicleListItems;            // List model for the vehicles to be shown on MasterDetaiPage ListView control.
Vector<ActionListItem^>^ gActionListItems;              // List model for the actions to be shown on EventListpage ListView control.


/*++
Routine Description:

Constructor

--*/
EnginePool::EnginePool()
{
}

/*++
Routine Description:

Destructor

--*/
EnginePool::~EnginePool()
{
}

/*++
Routine Description:

Create the requested engine, if not already created.

Arguments:

enginename - name of the engine object

Return Value: Object^. Hat pointer to requested engine object.
--*/
Platform::Object^ EnginePool::GetEngineObject(Platform::String^ engineName)
{
    Platform::Object^ engineObject = nullptr;

    if (engineName == L"VehicleList")
    {
        if (gVehicleListItems == nullptr)
        {
            gVehicleListItems = ref new Vector<VehicleListItem^>;
        }
        engineObject = dynamic_cast<Platform::Object^>(gVehicleListItems);
    }
    if (engineName == L"ActionList")
    {
        if (gActionListItems == nullptr)
        {
            gActionListItems = ref new Vector<ActionListItem^>;
        }
        engineObject = dynamic_cast<Platform::Object^>(gActionListItems);
    }
    else if (engineName == L"VehicleListItemCollector")
    {
        if (gVehicleListItemCollector == nullptr)
        {
            gVehicleListItemCollector = ref new VehicleListItemCollector();
        }
        engineObject = dynamic_cast<Platform::Object^>(gVehicleListItemCollector);
    }
    else if (engineName == L"ActionListItemCollector")
    {
        if (gActionListItemCollector == nullptr)
        {
            gActionListItemCollector = ref new ActionListItemCollector();
        }
        engineObject = dynamic_cast<Platform::Object^>(gActionListItemCollector);
    }
    else if (engineName == L"DatabaseEngine")
    {
        if (gDatabase == nullptr)
        {
            gDatabase = ref new MasterDetailApp::DatabaseEngine;

            // Default database location
            String^ dbFullFilePath = ApplicationData::Current->LocalFolder->Path + "\\vehicle_data.db";

            if (PathFileExists(dbFullFilePath) == true)
            {
                if (gDatabase->Open(dbFullFilePath) != true || gDatabase->Ready == false)
                {
                    DebugOut("EnginePool::GetEngineObject: Database file could not be opened");
                    return nullptr;
                }
            }
            else
            {
                if (gDatabase->Create(dbFullFilePath) != true || gDatabase->Ready == false)
                {
                    DebugOut("EnginePool::GetEngineObject: Database file could not be created");
                    return nullptr;
                }
            }
        }
        engineObject = dynamic_cast<Platform::Object^>(gDatabase);
    }
    return engineObject;
}


