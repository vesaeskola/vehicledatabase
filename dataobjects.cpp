/*++

Module Name:

dataobjects.cpp

Abstract:

This module implements general data objects used to store database data and common enumerations.


Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/

#include "pch.h"

using namespace MasterDetailApp;
using namespace Windows::Foundation;

/*++
Routine Description:

Constructor

--*/
PageNavigateArgs::PageNavigateArgs(int VehicleID, MasterDetailApp::PageArgs PageArguments, Platform::Object^ Object)
    : mVehicleID       (VehicleID),
      mPageArguments   (PageArguments),
      mObject          (Object)
{
}

/*++
Routine Description:

setActionObject. Store action object.

Arguments:

actionObject - Action object, this object could be: ServiceInfo, FuelingInfo or EventInfo object.

--*/
void ActionInfo::setActionObject(Platform::Object ^ actionObject)
{
    // Try to cast to known object type, if casting fails the object pointer remain null
    mServiceInfo = dynamic_cast<ServiceInfo^>(actionObject);
    mFuelingInfo = dynamic_cast<FuelingInfo^>(actionObject);
    mEventInfo = dynamic_cast<EventInfo^>(actionObject);
}

/*++
Routine Description:

getActionObject. Store action object.

Return Value: Platform::Object^. Either ServiceInfo^, FuelingInfo^ or EventInfo^ obejct,
--*/
Platform::Object ^ ActionInfo::getActionObject()
{
    if (mServiceInfo != nullptr) return mServiceInfo;
    if (mFuelingInfo != nullptr) return mFuelingInfo;
    if (mEventInfo != nullptr) return mEventInfo;
    return nullptr;
}

