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

        ActionListItem. Represent the action data shown in one item of action list. Action could be
        refueling, service or common event

        --*/
        public ref class ActionListItem sealed
        {
        public:
            ActionListItem(ActionInfo^ actionInfo);

            /*++
            Property Description:

            Row1. Text information to action list row 1. Content is formatted according action type.

            --*/
            property Platform::String^ Row1
            {
                Platform::String^ get();
                void set(Platform::String^ row1);
            };

            /*++
            Property Description:

            Row2_1. Return milage text into action list row 2 column 1.

            --*/
            property Platform::String^ Row2_1
            {
                Platform::String^ get();
                void set(Platform::String^ row2_1);
            };

            /*++
            Property Description:

            Row2_2. Return date text into action list row 2 column 1.

            --*/
            property Platform::String^ Row2_2
            {
                Platform::String^ get();
                void set(Platform::String^ row2_2);
            };

            /*++
            Property Description:

            IconSrc. Icon source path for the action list item.

            --*/
            property Platform::String^ IconSrc
            {
                Platform::String^ get();
                void set(Platform::String^ row1);
            };

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

            Set mileage unit of the current vehicle

            Arguments:

            int odometerUnitId: 0 = km, 1 = mil.
            --*/
            void SetOdometerUnit (int odometerUnitId)
            {
                mCurrentVechileOdometerUnitId = odometerUnitId;
            }

            /*++
            Routine Description:

            Set fuel unit of the current vehicle

            Arguments:

            int fuelUnitId: 0 = liter, 1 = gallon
            --*/
            void SetFuelUnit(int fuelUnitId)
            {
                mCurrentVechileFuelUnitId = fuelUnitId;
            }

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
            /*++
            Mileage unit of the current vehicle
            --*/
            int mCurrentVechileOdometerUnitId;

            /*++
            Fuel unit of the current vehicle
            --*/
            int mCurrentVechileFuelUnitId;

        };

    }
}

