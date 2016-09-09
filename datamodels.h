/*++

Module Name:

datamodels.h

Abstract:

This module implements view models to maintain list view data.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/


#pragma once

#include "pch.h"


namespace MasterDetailApp { namespace ViewModels {

        /*++
        Class Description:

        VehicleListItem. Represent the vehicle data shown in one row of vehicle list.

        --*/
        public ref class VehicleListItem sealed
        {
        public:
            VehicleListItem(VehicleInfo^ vehicle);

            /*++
            Property Description:

            VehicleId. From SQL VECHILES table: Vehicle identifier.

            --*/
            property int VehicleId
            {
                int get() { return mVehicleInfo->VehicleId; }
                void set(int VehicleId) { mVehicleInfo->VehicleId = VehicleId; }
            }

            /*++
            Property Description:

            Make. From SQL VECHILES table: Manufacturer of vehicle.

            --*/
            property Platform::String^ Make
            {
                Platform::String^ get() { return mVehicleInfo->Make; }
                void set(Platform::String^ Make) { mVehicleInfo->Make; }
            }

            /*++
            Property Description:

            RegPlate. From SQL VECHILES table: Register plate of vehicle.

            --*/
            property Platform::String^ RegPlate
            {
                Platform::String^ get() { return mVehicleInfo->RegPlate; }
                void set(Platform::String^ Make) { mVehicleInfo->RegPlate; }
            }

            /*++
            Property Description:

            Model. From SQL VECHILES table: Model of vehicle.

            --*/
            property Platform::String^ Model
            {
                Platform::String^ get() { return mVehicleInfo->Model; }
                void set(Platform::String^ Model) { mVehicleInfo->Model = Model; }
            }

            /*++
            Property Description:

            VinCode. From SQL VECHILES table: Vehicle Identification Number of vehicle.

            --*/
            property Platform::String^ VinCode
            {
                Platform::String^ get() { return mVehicleInfo->VinCode; }
                void set(Platform::String^ VinCode) { mVehicleInfo->VinCode = VinCode; }
            }

        private:
            VehicleInfo^ mVehicleInfo;
        };

        /*++
        Class Description:

        VehicleListItemCollector. Maintain the the vehicle data list.

        --*/
        public ref class VehicleListItemCollector sealed
        {
        public:
            VehicleListItemCollector();

            /*++
            Routine Description:

            Event handler for the DatabaseEngine object. Each row found from VECHILES table trigger this event handler.
            Store the vehicle info into vehicle list.

            Arguments:

            vehicle - Vehicle info, one row of data from VECHILES table.

            Return Value: None.
            --*/
            void VehicleInfoEventHandler(VehicleInfo^ vehicle);

        private:

        };

        /*++
        Class Description:

        ActionListItem. Represent the action data shown in one row of action list. Action could be
        fuelling, service or common event

        --*/
        public ref class ActionListItem sealed
        {
        public:
            ActionListItem(ActionInfo^ actionInfo);

            /*++
            Property Description:

            Column1. Text information to action list column 1. Content is formatted according action type.

            --*/
            property Platform::String^ Column1
            {
                Platform::String^ get()
                {
                    Platform::String^ retValue = ref new Platform::String;
                    if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        Windows::Foundation::DateTime dateTime;
                        dateTime.UniversalTime = dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Date;
                        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate::get();
                        retValue = dateTime.UniversalTime != 0 ? dtf->Format(dateTime) : "";
                        return retValue;
                    }
                    else if (dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        Windows::Foundation::DateTime dateTime;
                        dateTime.UniversalTime = dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->Date;
                        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate::get();
                        retValue = dateTime.UniversalTime != 0 ? dtf->Format(dateTime) : "";
                        return retValue;
                    }
                    else if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        Windows::Foundation::DateTime dateTime;
                        dateTime.UniversalTime = dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->Date;
                        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate::get();
                        retValue = dateTime.UniversalTime != 0 ? dtf->Format(dateTime) : "";
                        return retValue;
                    }
                    else
                    {
                        retValue = "";
                    }
                    return retValue;
                }
            }

            /*++
            Property Description:

            Column2. Text information to action list column 2. Content is formatted according action type.

            --*/
            property Platform::String^ Column2
            {
                Platform::String^ get()
                {
                    Platform::String^ retValue = ref new Platform::String;
                    if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        retValue = L"Mileage: " + dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Mileage;
                        return retValue;
                    }
                    else if (dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        retValue = L"Mileage: " + dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->Mileage;
                    }
                    else if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        retValue = L"Mileage: " + dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->Mileage;
                    }
                    else
                    {
                        retValue = "";
                    }
                    return retValue;
                }
            }

            /*++
            Property Description:

            Column3. Text information to action list column 1. Not used.

            --*/
            property Platform::String^ Column3
            {
                Platform::String^ get()
                {
                    Platform::String^ retValue = ref new Platform::String;
                    // TBD: Format col3 here if needed
                    retValue = "";
                    return retValue;
                }
            }

            /*++
            Property Description:

            Row2. Text information to action list column 1

            --*/
            property Platform::String^ Row2
            {
                Platform::String^ get()
                {
                    Platform::String^ retValue = ref new Platform::String();
                    if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        retValue = L"Type:";
                        if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Type == 0x00) retValue += L"Service not specified";
                        if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Type & 0x01) retValue  += L"O";
                        if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Type & 0x02) retValue  += L":OF";
                        if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Type & 0x04) retValue  += L":AF";
                        if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Type & 0x08) retValue  += L":FF";
                        if (dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject())->Type & 0x10) retValue  += L":Other";

                        return retValue;
                    }
                    else if (dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        retValue = "Amount: " + dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject())->Amount;
                        return retValue;
                    }
                    else if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject()) != nullptr)
                    {
                        if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->EventID == 0x01) retValue += L"Mileage memo";
                        if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->EventID == 0x02) retValue += L"Accessory purchase";
                        if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->EventID == 0x03) retValue += L"Cruise";
                        if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->EventID == 0x04) retValue += L"Memory";
                        if (dynamic_cast<EventInfo^>(mActionInfo->getActionObject())->EventID == 0x05) retValue += L"Other action";
                        return retValue;
                    }
                    else
                    {
                        retValue = "";
                    }
                    return retValue;
                }
            }


        internal:
            ActionInfo^ GetActionInfo () { return mActionInfo; }

        private:
            ActionInfo^ mActionInfo;
        };

        /*++
        Class Description:

        ActionListItemCollector. Maintain the the action data list.

        --*/
        public ref class ActionListItemCollector sealed
        {
        public:
            ActionListItemCollector();

            /*++
            Routine Description:

            Event handler for the DatabaseEngine object. Each row found from VECHILES table trigger this event handler.
            Store the vehicle info into vehicle list.

            Arguments:

            vehicle - Vehicle info, one row of data from VECHILES table.

            Return Value: None.
            --*/

            /*++
            Routine Description:

            Event handler for the DatabaseEngine object. reading of EVENTS, FUELING or SERVICE trigger this event handler.
            Store the action info into actions list.

            Arguments:

            actionInfo - Action info, one row of data from EVENTS, FUELING or SERVICE table.

            Return Value: None.
            --*/
            void ActionInfoEventHandler(ActionInfo^ actionInfo);

        private:

        };

    }
}

