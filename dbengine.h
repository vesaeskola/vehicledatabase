/*++

Module Name:

dbengine.h

Abstract:

This module implements DatabaseEngine for the SQLite database.
Purpose is to store relational database tables into one file.
This project store vehicle data into SQLite database using following tables:
- EVENTS
- EVENT_TYPE
- FUELING
- SERVICE
- SERVCE_TYPE
- VECHILES

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/


#pragma once

#include "pch.h"

namespace MasterDetailApp
{
    ref class DatabaseEngine;

    // Read VECHILES table cause this event to be triggered
    public delegate void VehicleInfoHandler(VehicleInfo^ vehicleInfo);

    // Read EVENTS table cause this event to be triggered
    public delegate void EventInfoHandler(EventInfo^ eventInfo);

    // Read FUELING table cause this event to be triggered
    public delegate void FuelingInfoHandler(FuelingInfo^ fuelingInfo);

    // Read SERVICE table cause this event to be triggered
    public delegate void ServiceInfoHandler(ServiceInfo^ serviceInfo);

    // Read EVENTS, FUELING or SERVICE table cause this event to be triggered
    public delegate void ActionInfoHandler(ActionInfo^ actionInfo);

    public ref class DatabaseEngine sealed
    {
        bool _databaseReady;

    public:
        /*++
        Routine Description:

        Constructor

        --*/
        DatabaseEngine();

        /*++
        Routine Description:

        Destructor

        --*/
        virtual ~DatabaseEngine();

        /*++
        Event definitions:

        This object will trigger following events as a result of database reading operations.

        --*/
        event VehicleInfoHandler^ VehicleInfoRead;
        event EventInfoHandler^ EventInfoRead;
        event FuelingInfoHandler^ FuelingInfoRead;
        event ServiceInfoHandler^ ServiceInfoRead;
        event ActionInfoHandler^ ActionInfoRead;

        /*++
        Routine Description:

        Wrapper for VehicleInfoRead event

        Arguments:

        vehicleInfo - VehicleInfo object to be sent as parameter with event.

        --*/
        void SendVehicleInfoReadEvent (VehicleInfo^ vehicleInfo)
        {
            VehicleInfoRead(vehicleInfo);
        }

        /*++
        Routine Description:

        Wrapper for EventInfoRead event

        Arguments:

        eventInfo - EventInfo object to be sent as parameter with event.

        --*/
        void SendEventInfoReadEvent(EventInfo^ eventInfo)
        {
            EventInfoRead(eventInfo);
        }

        /*++
        Routine Description:

        Wrapper for FuelingInfoRead event

        Arguments:

        fuelingInfo - FuelingInfo object to be sent as parameter with event.

        --*/
        void SendFuelingInfoReadEvent(FuelingInfo^ fuelingInfo)
        {
            FuelingInfoRead(fuelingInfo);
        }

        /*++
        Routine Description:

        Wrapper for ServiceInfoRead event

        Arguments:

        serviceInfo - ServiceInfo object to be sent as parameter with event.

        --*/
        void SendServiceInfoReadEvent(ServiceInfo^ serviceInfo)
        {
            ServiceInfoRead(serviceInfo);
        }

        /*++
        Property Description:

        Ready. This property describe if database is opened correctly and contain valid tables.

        --*/
        property bool Ready
        {
            bool get()
            {
                return _databaseReady;
            }
        }

        /*++
        Routine Description:

        Open and validate database. Note: This method does not open other database if one is already open. Call
        CloseDatabase to switch into new database.
        Note: Currently the application is able to open files only from ApplicationData::Current->LocalFolder->Path

        Arguments:

        fullPath - Full path to SQLite file name

        Return Value: bool. true if database has succesfully opened and it contain valid tables.
        --*/
        bool Open (Platform::String^ fullPath);

        /*++
        Routine Description:

        Close database

        --*/
        void CloseDatabase ();

        /*++
        Routine Description:

        Create. Create an empty Vehicle data database which contain basic structure for the tables.

        Arguments:

        fullPath - Full path to SQLite file name

        Return Value: bool. true if database has succesfully created and opened.

        --*/
        bool Create(Platform::String^ fullPath);

        /*++
        Routine Description:

        Read last row from EVENTS table for given vehicle

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn

        Return Value: EventInfo^. Event info object, nullptr if not succeeded

        --*/
        EventInfo^ ReadLastEventInfoRow(int vehicleId);

        /*++
        Routine Description:

        Read last row from FUELLING table for given vehicle

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn

        Return Value: FuelingInfo^. Fueling info object, nullptr if not succeeded

        --*/
        FuelingInfo^ ReadLastFuelingInfoRow(int vehicleId);

        /*++
        Routine Description:

        Read last row from FUELLING table for given vehicle

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn

        Return Value: FuelingInfo^. Fueling info object, nullptr if not succeeded

        --*/
        ServiceInfo^ ReadLastServiceInfoRow(int vehicleId);

        /*++
        Routine Description:

        Read rows from FUELLING table for given vehicle. Trigger as many ActionInfoRead events
        as there is found fueling rows for selected the vehicle.

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn

        Return Value: bool. true if succeeded.

        --*/
        bool ReadAllFuelings(int vehicleId);

        /*++
        Routine Description:

        Read rows from SERVICE table for given vehicle. Trigger as many ActionInfoRead events
        as there is found service rows for selected the vehicle.

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn

        Return Value: bool. true if succeeded.

        --*/
        bool ReadAllServices(int vehicleId);

        /*++
        Routine Description:

        Read rows from EVENTS table for given vehicle. Trigger as many ActionInfoRead events
        as there is found events rows for selected the vehicle.

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn

        Return Value: bool. true if succeeded.

        --*/
        bool ReadAllEvents(int vehicleId);


        /*++
        Routine Description:

        Read Make, RegPlate, Model and VinCode columns from VECHILES table of database. Trigger as many VehicleInfoRead
        events as there is found vehicle rows from VECHILES table.

        Return Value: integer. Count of vehicles in the database

        --*/
        int ReadShortVehicleInfo();

        /*++
        Routine Description:

        Read full vehicle data of given vehicle ID from VECHILES table of database. Trigger VehicleInfoRead event
        if vehicle is found.

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn

        Return Value: bool. if succeeded true.

        --*/
        bool ReadVehicleData(int vehicleId);

        /*++
        Routine Description:

        Add new vehicle information into sql database

        Arguments:

        vehicleInfo - Contain data to be write into database.

        Return Value: bool. if succeeded true.
        --*/
        bool addVehicle(VehicleInfo^ vehicleInfo);

        /*++
        Routine Description:

        Update the existing vehicle information in sql database

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn
        vehicleInfo - Contain data to be update into database.

        Return Value: bool. if succeeded true.
        --*/
        bool updateVehicle(int vehicleId, VehicleInfo^ vehicleInfo);

        /*++
        Routine Description:

        Update the existing vehicle information in sql database

        Arguments:

        vehicleId - Index of vehicle, VECHILES table VehicleID columnn
        newImagePath - Path of new image

        Return Value: bool. if succeeded true.
        --*/
        bool updateVehicleImagePath(int vehicleId, Platform::String^ newImagePath);

        /*++
        Routine Description:

        Add fueling event of the existing vehicle

        Arguments:

        fuelingInfo - Fueling data entered by end user

        Return Value: bool. if succeeded true.
        --*/
        bool addFuelingInfo(FuelingInfo^ fuelingInfo);

        /*++
        Routine Description:

        Update existing fueling info

        Arguments:

        primaryKey - primary key of FUELING table
        fuelingInfo - fueling data updated by end user

        Return Value: bool. if succeeded true.
        --*/
        bool updateFuelingInfo(int primaryKey, FuelingInfo^ fuelingInfo);

        /*++
        Routine Description:

        Add service event of the existing vehicle

        Arguments:

        serviceInfo - Service data entered by end user

        Return Value: bool. if succeeded true.
        --*/
        bool addServiceInfo(ServiceInfo^ serviceInfo);

        /*++
        Routine Description:

        Update existing service info

        Arguments:

        primaryKey - primary key of FUELING table
        serviceInfo - service data updated by end user

        Return Value: bool. if succeeded true.
        --*/
        bool updateServiceInfo(int primaryKey, ServiceInfo^ serviceInfo);

        /*++
        Routine Description:

        Add event of the existing vehicle

        Arguments:

        eventInfo - Event data entered by end user

        Return Value: bool. if succeeded true.
        --*/
        bool addEventInfo(EventInfo^ eventInfo);

        /*++
        Routine Description:

        Update existing event info

        Arguments:

        primaryKey - primary key of EVENTS table
        eventInfo - Event data updated by end user

        Return Value: bool. if succeeded true.
        --*/
        bool updateEventInfo(int primaryKey, EventInfo^ eventInfo);

    private:
        /*++
        Routine Description:

        Open SQLite database file

        Arguments:

        filename - Database filename (UTF-16)

        Return Value: bool. true if database opened succesfully.
        --*/
        bool openDatabase(const void *filename);

        /*++
        Routine Description:

        Validate opened SQLite database

        Return Value: bool. true if database has valid Vehicle data tables
        --*/
        bool validateDatabase();

        /*++
        Routine Description:

        Callback for the SQLite database. Validate databases.
        This method set bits of static variable mTableIdentifierMask accoring founded tables.

        Return Value: int. Always 0.
        --*/
        static int validateCB(void *NotUsed, int argc, char **argv, char **azColName);

        /*++
        Routine Description:

        Callback for the SQLite database query: SELECT * FROM VECHILES

        Return Value: int. Always 0.
        --*/
        static int readFullVehicleInfoCB(void * NotUsed, int argc, char ** argv, char ** azColName);

        /*++
        Routine Description:

        Callback for the SQLite database query: SELECT * FROM VECHILES

        Return Value: int. Always 0.
        --*/
        static int readShortVehicleInfoCB(void * NotUsed, int argc, char ** argv, char ** azColName);

    };
}
