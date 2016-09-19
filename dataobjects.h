/*++

Module Name:

dataobjects.h

Abstract:

This module implements general data objects used to store database data and common enumerations.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/


#pragma once

#include "pch.h"

namespace MasterDetailApp
{
    /*++
    Class Description:

    VehicleInfo. Data storage class for one row of VECHILES table.

    --*/
    public ref class VehicleInfo sealed
    {
    public:

        /*++
        Property Description:

        VehicleID. Unique vehicle identifier, primary key of VECHILES table

        --*/
        property int VehicleId
        {
            int get() { return mVehicleID; }
            void set(int VehicleID) { mVehicleID = VehicleID; }
        }

        /*++
        Property Description:

        VinCode. Vehicle vin code

        --*/
        property Platform::String^ VinCode
        {
            Platform::String^ get() { return mVinCode; }
            void set(Platform::String^ VinCode) { mVinCode = VinCode; }
        }

        /*++
        Property Description:

        Make. Vehicle make.

        --*/
        property Platform::String^ Make
        {
            Platform::String^ get() { return mMake; }
            void set(Platform::String^ Make) { mMake = Make; }
        }

        /*++
        Property Description:

        Model. Vehicle model.

        --*/
        property Platform::String^ Model
        {
            Platform::String^ get() { return mModel; }
            void set(Platform::String^ Model) { mModel = Model; }
        }

        /*++
        Property Description:

        Year. Manufacturing year of the vehicle.

        --*/
        property int Year
        {
            int get() { return mYear; }
            void set(int Year) { mYear = Year; }
        }

        /*++
        Property Description:

        RegPlate. Vehicle registration plate

        --*/
        property Platform::String^ RegPlate
        {
            Platform::String^ get() { return mRegPlate; }
            void set(Platform::String^ RegPlate) { mRegPlate = RegPlate; }
        }

        /*++
        Property Description:

        Description. Vehicle description entered by end user-

        --*/
        property Platform::String^ Description
        {
            Platform::String^ get() { return mDescription; }
            void set(Platform::String^ Description) { mDescription = Description; }
        }
        /*++
        Property Description:

        Description. Vehicle description entered by end user-

        --*/
        property Platform::String^ ImagePath
        {
            Platform::String^ get() { return mImagePath; }
            void set(Platform::String^ newImagePath) { mImagePath = newImagePath; }
        }

        /*++
        Property Description:

        FuelUnitId. Identifier of vehicle fuel.

        --*/
        property int FuelUnitId
        {
            int get() { return mFuelUnitId; }
            void set(int FuelUnitId) { mFuelUnitId = FuelUnitId; }
        }

        /*++
        Property Description:

        OdometerUnitId. Identifier of vehicle odometer unit (km, mile, etc.)

        --*/
        property int OdometerUnitId
        {
            int get() { return mOdometerUnitId; }
            void set(int OdometerUnitId) { mOdometerUnitId = OdometerUnitId; }
        }

    private:
        int mVehicleID;
        Platform::String^ mVinCode;
        Platform::String^ mMake;
        Platform::String^ mModel;
        int mYear;
        Platform::String^ mRegPlate;
        Platform::String^ mDescription;
        Platform::String^ mImagePath;
        int mFuelUnitId;
        int mOdometerUnitId;
    };

    /*++
    Enumeration Description:

    PageArgs. Used to define page navigation reason.

    --*/
    public enum class PageArgs : int
    {
        PageArgsNone,                 // No specific page navigate arguments
        PageArgsFuelingList,          // Last fueling clicked, action list should show all fuelings
        PageArgsServiceList,          // Last service clicked, action list should show all services
        PageArgsEventsList,           // Last event clicked, action list should show all events
        PageArgsAppLaunched,          // Page is opened immediately after application launch
        PageArgsVehicleEntered,       // New vehicle was entered by user
        PageArgsVehicleModified,      // Existing vehicle was edited by user
        PageArgsOperationCancelled,   // User click back, and cancelled operation
        PageArgsEditVehicle,          // User want to edit already entered vehicle
        PageArgsEditAction,           // User want to edit already entered action
    };

    /*++
    Class Description:

    PageNavigateArgs. Capsulate arguments used to navigate between pages.

    --*/
    public ref class PageNavigateArgs sealed
    {
    public:
        /*++
        Routine Description:

        Constructor

        --*/
        PageNavigateArgs(int VehicleID, MasterDetailApp::PageArgs PageArguments, Platform::Object^ object);


        /*++
        Property Description:

        VehicleID. Unique vehicle identifier, primary key of VECHILES table

        --*/
        property int VehicleID
        {
            int get() { return mVehicleID; }
            void set(int VehicleID) { mVehicleID = VehicleID; }
        }

        /*++
        Property Description:

        PageArguments. Store page navigate argument.

        --*/
        property PageArgs PageArguments
        {
            PageArgs get() { return mPageArguments; }
            void set(PageArgs PageArguments) { mPageArguments = PageArguments; }
        }

        /*++
        Property Description:

        Object. Contain object related to page navication, could be e.g. VehicleInfo^ or ActionInfo^

        --*/
        property Platform::Object^ Object
        {
            Platform::Object^ get() { return mObject; }
            void set(Platform::Object^ info) { mObject = info; }
        }

    private:
        int mVehicleID;                 // TBD: Capsulate vehicleID into mObject
        PageArgs mPageArguments;        // Indicate why user navigate into page
        Platform::Object^ mObject;      // Object related to opened page, could be e.g. VehicleInfo^ or ActionInfo^

    };


    /*++
    Class Description:

    EventInfo. Data storage class for one row of EVENTS table.

    --*/
    public ref class EventInfo sealed
    {
    public:

        /*++
        Property Description:

        PrimaryKey. Unique primary key of FUELING, SERVICE or EVENTS table

        --*/
        property int PrimaryKey
        {
            int get() { return mPrimaryKey; }
            void set(int primaryKey) { mPrimaryKey = primaryKey; }
        }

        /*++
        Property Description:

        VehicleID. Unique vehicle identifier, primary key of VECHILES table

        --*/
        property int VehicleID
        {
            int get() { return mVehicleID; }
            void set(int VehicleID) { mVehicleID = VehicleID; }
        }

        /*++
        Property Description:

        EventID. Identifier of event type.

        --*/
        property int EventID
        {
            int get() { return mEventID; }
            void set(int EventID) { mEventID = EventID; }
        }

        /*++
        Property Description:

        Price. Price related to event .

        --*/
        property int Price
        {
            int get() { return mPrice; }
            void set(int Price) { mPrice = Price; }
        }

        /*++
        Property Description:

        Mileage. Odometer value.

        --*/
        property int Mileage
        {
            int get() { return mMileage; }
            void set(int Mileage) { mMileage = Mileage; }
        }

        /*++
        Property Description:

        Date. Date of the event.

        --*/
        property int64 Date
        {
            int64 get() { return mGlobalDate; }
            void set(int64 Date) { mGlobalDate = Date; }
        }

        /*++
        Property Description:

        Description. Vehicle description entered by end user-

        --*/
        property Platform::String^ Description
        {
            Platform::String^ get() { return mDescription; }
            void set(Platform::String^ Description) { mDescription = Description; }
        }


    private:
        int mPrimaryKey;
        int mVehicleID;
        int mEventID;
        int mPrice;
        int mMileage;
        int64 mGlobalDate;
        Platform::String^ mDescription;

    };

    /*++
    Class Description:

    FuelingInfo. Data storage class for one row of FUELING table.

    --*/
    public ref class FuelingInfo sealed
    {
    public:

        /*++
        Property Description:

        PrimaryKey. Unique primary key of FUELING, SERVICE or EVENTS table

        --*/
        property int PrimaryKey
        {
            int get() { return mPrimaryKey; }
            void set(int primaryKey) { mPrimaryKey = primaryKey; }
        }

        /*++
        Property Description:

        VehicleID. Unique vehicle identifier, primary key of VECHILES table

        --*/
        property int VehicleID
        {
            int get() { return mVehicleID; }
            void set(int VehicleID) { mVehicleID = VehicleID; }
        }

        /*++
        Property Description:

        Amount. Amount of refueling.

        --*/
        property int Amount
        {
            int get() { return mAmount; }
            void set(int Amount) { mAmount = Amount; }
        }

        /*++
        Property Description:

        Mileage. Odometer value.

        --*/
        property int Mileage
        {
            int get() { return mMileage; }
            void set(int Mileage) { mMileage = Mileage; }
        }

        /*++
        Property Description:

        IsFull. Boolean value describe if full tank refuelling happen.

        --*/
        property bool IsFull
        {
            bool get() { return mIsFull; }
            void set(bool IsFull) { mIsFull = IsFull; }
        }

        /*++
        Property Description:

        Price. Total price of fueling.

        --*/
        property int Price
        {
            int get() { return mPrice; }
            void set(int Price) { mPrice = Price; }
        }

        /*++
        Property Description:

        Date. Date of the refueling

        --*/
        property int64 Date
        {
            int64 get() { return mGlobalDate; }
            void set(int64 Date) { mGlobalDate = Date; }
        }

        /*++
        Property Description:

        Description. Vehicle description entered by end user-

        --*/
        property Platform::String^ Description
        {
            Platform::String^ get() { return mDescription; }
            void set(Platform::String^ Description) { mDescription = Description; }
        }

    private:
        int mPrimaryKey;
        int mVehicleID;
        int mAmount;
        int mMileage;
        bool mIsFull;
        int mPrice;
        int64 mGlobalDate;
        Platform::String^ mDescription;
    };

    /*++
    Class Description:

    ServiceInfo. Data storage class for one row of FUELING table.

    --*/
    public ref class ServiceInfo sealed
    {
    public:
        /*++
        Property Description:

        PrimaryKey. Unique primary key of FUELING, SERVICE or EVENTS table

        --*/
        property int PrimaryKey
        {
            int get() { return mPrimaryKey; }
            void set(int primaryKey) { mPrimaryKey = primaryKey; }
        }

        /*++
        Property Description:

        VehicleID. Unique vehicle identifier, primary key of VECHILES table

        --*/
        property int VehicleID
        {
            int get() { return mVehicleID; }
            void set(int VehicleID) { mVehicleID = VehicleID; }
        }

        /*++
        Property Description:

        Type. Type identifier of service.

        --*/
        property int Type
        {
            int get() { return mType; }
            void set(int Type) { mType = Type; }
        }

        /*++
        Property Description:

        Mileage. Odometer value.

        --*/
        property int Mileage
        {
            int get() { return mMileage; }
            void set(int Mileage) { mMileage = Mileage; }
        }

        /*++
        Property Description:

        Price. Total price of service.

        --*/
        property int Price
        {
            int get() { return mPrice; }
            void set(int Price) { mPrice = Price; }
        }

        /*++
        Property Description:

        Date. Date of service.

        --*/
        property int64 Date
        {
            int64 get() { return mGlobalDate; }
            void set(int64 Date) { mGlobalDate = Date; }
        }

        /*++
        Property Description:

        Description. Vehicle description entered by end user-

        --*/
        property Platform::String^ Description
        {
            Platform::String^ get() { return mDescription; }
            void set(Platform::String^ Description) { mDescription = Description; }
        }

    private:
        int mPrimaryKey;
        int mVehicleID;
        int mType;
        int mMileage;
        int mPrice;
        int64 mGlobalDate;
        Platform::String^ mDescription;

    };


    /*++
    Class Description:

    ActionInfo. Data storage class for common action. Action could contain data from one row of EVENTS, FUELING, 
    or SERVICE table.

    --*/
    public ref class ActionInfo sealed
    {
    public:
        /*++
        Routine Description:

        setActionObject. Store action object.

        Arguments:

        actionObject - Action object, this object could be: ServiceInfo, FuelingInfo or EventInfo object.

        --*/
        void setActionObject(Platform::Object^ actionObject);

        /*++
        Property Description:

        OdoMeterUnitID. Odometer unit. 

        --*/
        property int OdoMeterUnitID
        {
            int get() { return mOdoMeterUnitID; }
            void set(int OdoMeterUnitID) { mOdoMeterUnitID = OdoMeterUnitID; }
        }

        /*++
        Property Description:

        FuelUnitID. Fuel unit.

        --*/
        property int FuelUnitID
        {
            int get() { return mFuelUnitID; }
            void set(int FuelUnitID) { mFuelUnitID = FuelUnitID; }
        }

        /*++
        Routine Description:

        getActionObject. Store action object.

        Return Value: Platform::Object^. Either ServiceInfo^, FuelingInfo^ or EventInfo^ obejct,
        --*/
        Platform::Object^ getActionObject();

    private:
        ServiceInfo^ mServiceInfo;
        FuelingInfo^ mFuelingInfo;
        EventInfo^ mEventInfo;

        /*++
        Mileageunit of the current vehicle
        --*/
        int mOdoMeterUnitID;

        /*++
        Fuelunit of the current vehicle
        --*/
        int mFuelUnitID;



    };

}
