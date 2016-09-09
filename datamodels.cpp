/*++

Module Name:

datamodels.cpp

Abstract:

This module implements data objects used to store vehicle data.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/

#include "pch.h"

using namespace MasterDetailApp;
using namespace MasterDetailApp::ViewModels;
using namespace Platform::Collections;


/*++
Routine Description:

Constructor

--*/
VehicleListItem::VehicleListItem(VehicleInfo^ vehicle)
    : mVehicleInfo (vehicle)
{
    DebugOut("VehicleListItem::VehicleListItem");
}

/*++
Routine Description:

Constructor

--*/
VehicleListItemCollector::VehicleListItemCollector()
{
    DebugOut("VehicleListItemCollector::VehicleListItemCollector");
}

/*++
Routine Description:

Database event handler. Store the vehicle object into list model.

--*/
void VehicleListItemCollector::VehicleInfoEventHandler(VehicleInfo^ Vehicle)
{
    DebugOut("VehicleListItemCollector::VehicleInfoEventHandler");
    Vector<VehicleListItem^>^ vehicleListItems = dynamic_cast<Vector<VehicleListItem^>^>(gEnginePool->GetEngineObject(L"VehicleList"));
    if (vehicleListItems != nullptr)
    {
        vehicleListItems->Append(ref new VehicleListItem(Vehicle));
    }
}

ActionListItem::ActionListItem(ActionInfo^ actionInfo)
    : mActionInfo (actionInfo)
{
    DebugOut("ActionListItem::ActionListItem");
}

/*++
Routine Description:

Constructor

--*/
ActionListItemCollector::ActionListItemCollector()
{
    DebugOut("ActionListItemCollector::ActionListItemCollector");
}

/*++
Routine Description:

Database event handler. Store the vehicle object into list model.

--*/
void ActionListItemCollector::ActionInfoEventHandler(ActionInfo^ actionInfo)
{
    DebugOut("ActionListItemCollector::ActionInfoEventHandler");
    Vector<ActionListItem^>^ actionListItems = dynamic_cast<Vector<ActionListItem^>^>(gEnginePool->GetEngineObject(L"ActionList"));
    if (actionListItems != nullptr)
    {
        actionListItems->Append(ref new ActionListItem(actionInfo));
    }
}



