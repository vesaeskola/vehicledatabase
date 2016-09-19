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

/*++
Routine Description:

Constructor

--*/
ActionListItem::ActionListItem(ActionInfo^ actionInfo)
    : mActionInfo (actionInfo)
{
    DebugOut("ActionListItem::ActionListItem");
}

/*++
Property Description:

Row1. Text information to action list row 1. Content is formatted according action type.

--*/
Platform::String^ ActionListItem::Row1::get()
{
    Platform::String^ retValue = ref new Platform::String;
    if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        retValue = dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Description;
        return retValue;
    }
    else if (dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        retValue = MasterDetailApp::IntToPlatformString(dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->Amount);
        retValue += mActionInfo->FuelUnitID == 0 ? L" lit. " : L" gal. ";
        retValue += dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->Description;
        return retValue;
    }
    else if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        switch (dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->EventID)
        {
        case 1: retValue = L"Mileage: "; break;
        case 2: retValue = L"Accessory: "; break;
        case 3: retValue = L"Trip: "; break;
        case 4: retValue = L"Memo: "; break;
        case 5: retValue = L"Other: "; break;
        }

        retValue += dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->Description;
        return retValue;
    }
    else
    {
        retValue = "";
    }
    return retValue;
}

/*++
Property Description:

Row1. Text information to action list row 1. This is read only property.

--*/
void ActionListItem::Row1::set(Platform::String^ row1)
{

}
/*++
Property Description:

Row2_1. Return milage text into action list row 2_1.

--*/
Platform::String^ ActionListItem::Row2_1::get ()
{
    Platform::String^ retValue = ref new Platform::String;
    if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        retValue += dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Mileage;
        retValue += mActionInfo->OdoMeterUnitID == 0 ? L" km " : L" mil. ";
    }
    else if (dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        retValue += dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->Mileage;
        retValue += mActionInfo->OdoMeterUnitID == 0 ? L" km " : L" mil. ";
    }
    else if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        retValue += dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->Mileage;
        retValue += mActionInfo->OdoMeterUnitID == 0 ? L" km " : L" mil. ";
    }
    else
    {
        retValue = "";
    }
    return retValue;
}

/*++
Property Description:

Row2_1. Text information to action list row 2_1. This is read only property.

--*/
void ActionListItem::Row2_1::set(Platform::String^ row2_1)
{
}

/*++
Property Description:

Row2_2. Return date text into action list row 2_2.

--*/
Platform::String^ ActionListItem::Row2_2::get()
{
    Platform::String^ retValue = ref new Platform::String;
    if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        Windows::Foundation::DateTime dateTime;
        dateTime.UniversalTime = dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Date;
        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("month.abbreviated day year");
        retValue = dateTime.UniversalTime != 0 ? dtf->Format(dateTime) : "";
        return retValue;
    }
    else if (dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        Windows::Foundation::DateTime dateTime;
        dateTime.UniversalTime = dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->Date;
        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("month.abbreviated day year");
        retValue = dateTime.UniversalTime != 0 ? dtf->Format(dateTime) : "";
        return retValue;
    }
    else if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        Windows::Foundation::DateTime dateTime;
        dateTime.UniversalTime = dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->Date;
        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("month.abbreviated day year");
        retValue = dateTime.UniversalTime != 0 ? dtf->Format(dateTime) : "";
        return retValue;
    }
    else
    {
        retValue = "";
    }
    return retValue;
}

/*++
Property Description:

Row2_2. Text information to action list row 2_2. This is read only property.

--*/
void ActionListItem::Row2_2::set(Platform::String^ row2_2)
{
}

/*++
Property Description:

IconSrc. Icon source path for the action list Column 1

--*/
Platform::String^ ActionListItem::IconSrc::get()
{
    Platform::String^ retValue = ref new Platform::String(L"Assets/default_icon_480.png");
    if (dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        retValue = dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->IsFull ? "Assets/fueling_1_480.png" : "Assets/fueling_2_480.png";
    }
    else if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        switch (dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->EventID)
        {
        case 1: retValue = "Assets/mil_icon_480.png"; break;      // Mileage
        case 2: retValue = "Assets/acc_icon_480.png"; break;      // Accessory
        case 3: retValue = "Assets/trip_480.png"; break;          // Trip
        case 4: retValue = "Assets/memo_icon_480.png"; break;     // Memo
        case 5: retValue = "Assets/other_icon_480.png"; break;    // Other
        }
    }
    else if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject()) != nullptr)
    {
        switch (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Type)
        {
        case 0x01: retValue = "Assets/service_01_480_black.png"; break;      // Oil change
        case 0x02: retValue = "Assets/service_02_480.png"; break;            // Oil filter change
        case 0x03: retValue = "Assets/service_03_480_black.png"; break;      // Oil + filter change
        case 0x04: retValue = "Assets/service_04_480_black.png"; break;      // Oil + filter Air filter change
        case 0x07: retValue = "Assets/service_07_480.png"; break;            // Fuel filter change
        case 0x08: retValue = "Assets/service_08_480.png"; break;            // Fuel filter change
        case 0x10: retValue = "Assets/service_16_480.png"; break;            // General service

                                                                                // Combined 2x service
        case 0x05:
        case 0x06:
        case 0x09:
        case 0x0A:
        case 0x0C:
        case 0x11:
        case 0x12:
        case 0x14:
        case 0x18:
            retValue = "Assets/service_2x_480.png";
            break;

            // Combined 3x service
        case 0x0B:
        case 0x0D:
        case 0x0E:
        case 0x13:
        case 0x15:
        case 0x16:
        case 0x19:
        case 0x1A:
        case 0x1C:
            retValue = "Assets/service_3x_480.png";
            break;

            // Combined 4x service
        case 0x0F:
        case 0x17:
        case 0x1B:
        case 0x1D:
        case 0x1E:
            retValue = "Assets/service_4x_480.png";
            break;

            // Combined 5x service
        case 0x1F:
            retValue = "Assets/service_5x_480.png";
            break;

            // Combined unknown service
        default: retValue = "Assets/service_x_480.png";     break;
        }
    }
    return retValue;
}

/*++
Property Description:

IconSrc. Text information to action list icon. This is read only property.

--*/
void ActionListItem::IconSrc::set(Platform::String^ iconSrc)
{
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
        actionInfo->OdoMeterUnitID = mCurrentVechileOdometerUnitId;
        actionInfo->FuelUnitID = mCurrentVechileFuelUnitId;
        actionListItems->Append(ref new ActionListItem(actionInfo));
    }
}



