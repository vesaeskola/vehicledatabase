/*++fd

Module Name:

dbengine.cpp

Abstract:

This module implements DatabaseEngine for the SQLite database.
Purpose is to store relational database tables into one file.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/


#include "pch.h"

using namespace Platform;
using namespace MasterDetailApp;
using namespace Windows::Storage::Streams;

/*++
Datatype Description:

Bitmask to identify database tables. idea is make a SQL database query to get all tables
and set each bit in result callback calls. If all bits are set all tables are found.

--*/
typedef enum TableIdentifier_tag
{
    TableIdentifier_Vehicles          = 0x01,     // VECHILES
    TableIdentifier_Fueling           = 0x02,     // FUELING
    TableIdentifier_Events            = 0x04,     // EVENTS
    TableIdentifier_Service           = 0x08,     // SERVICE
    TableIdentifier_ServiceType       = 0x10,     // SERVICE_TYPE
    TableIdentifier_EventType         = 0x20,     // EVENT_TYPE

    TableIdentifier_All               = 0x3F,     // Every table

} TableIdentifier;


// TBD: Should/could be private member, how to define while using namespaces and WinRT component ?
static sqlite3* pDatabase = NULL;

// TBD: Should/could be private member, how to define while using namespaces and WinRT component ?
// SQLite static callback will fill bits of this bit mask
static uint16 mTableIdentifierMask;

// SQLite static callback will increase hs counter
static uint32 mVehicleCount = 0;

/*++
Routine Description:

Constructor

--*/
DatabaseEngine::DatabaseEngine()
{
}

/*++
Routine Description:

Destructor

--*/
DatabaseEngine::~DatabaseEngine()
{
    Close();
}

/*++
Routine Description:

Open and validate database. Set Ready property if database is OK

Arguments:

fullPath - Full path to SQLite database filename

Return Value: bool. true if database has succesfully opened and it contain valid Vehicle data tables.
--*/
bool DatabaseEngine::Open(Platform::String^ fullPath)
{
    _databaseReady = false;

    if (openDatabase(fullPath->Data()) == false)
    {
        return false;
    }

    if (validateDatabase() == false)
    {
        // Probably an empty db file was created, need to create tables
        Close();

        if (Create(fullPath) == false)
        {
            return false;
        }
    }
    _databaseReady = true;

    return true;
}

/*++
Routine Description:

Create new database with tables

Arguments:

fullPath - Full path to SQLite database filename

Return Value: bool. true if database has succesfully created and opened.
--*/
bool DatabaseEngine::Create(Platform::String^ fullPath)
{
    _databaseReady = false;
    int32 rc = 0;

    rc = sqlite3_open16(fullPath->Data(), &pDatabase);

    if (rc != SQLITE_OK)
    {
        return false;
    }
    else
    {
        //TBD: Utilize zErrMsg parameter of sqlite3_exec method to get error message string:
        //   char *zErrMsg = NULL;
        //   sqlite3_free(zErrMsg);

        // Create VECHILES table to hold basic car data
        {
            char* sql = "CREATE TABLE VECHILES("  \
                "VehicleID       INTEGER PRIMARY KEY     NOT NULL," \
                "VinCode         TEXT," \
                "Make            TEXT," \
                "Model           TEXT," \
                "Year            INT," \
                "RegPlate        TEXT," \
                "Description     TEXT," \
                "FuelUnitId      INT," \
                "OdometerUnitId  INT," \
                "binData         BLOB);";

            /* Execute SQL statement */
            rc = sqlite3_exec(pDatabase, sql, NULL, 0, NULL);
            if (rc != SQLITE_OK)
            {
                return false;
            }
        }

        // Create FUELLING table which store service event
        {
            char* sql = "CREATE TABLE FUELING("  \
                "PrimaryKey      INTEGER PRIMARY KEY     NOT NULL," \
                "VehicleID       INT              NOT NULL," \
                "Date            DATETIME DEFAULT CURRENT_TIMESTAMP," \
                "Amount          INT              NOT NULL," \
                "Mileage         INT," \
                "Prise           INT," \
                "Description     TEXT);";

            /* Execute SQL statement */
            rc = sqlite3_exec(pDatabase, sql, NULL, 0, NULL);
            if (rc != SQLITE_OK)
            {
                return false;
            }
        }

        // Create EVENTS table which store common event
        {
            char* sql = "CREATE TABLE EVENTS("  \
                "PrimaryKey      INTEGER PRIMARY KEY     NOT NULL," \
                "VehicleID       INT            NOT NULL," \
                "Date            DATETIME DEFAULT CURRENT_TIMESTAMP," \
                "EventID         INT            NOT NULL," \
                "Mileage         INT," \
                "Prise           INT," \
                "Description     TEXT);";

                // db.execSQL("INSERT INTO EVENTS(vehicleID, created_at) VALUES(2, 'datetime()'");

            /* Execute SQL statement */
            rc = sqlite3_exec(pDatabase, sql, NULL, 0, NULL);
            if (rc != SQLITE_OK)
            {
                return false;
            }
        }

        // Create SERVICE table which store service event
        {
            char* sql = "CREATE TABLE SERVICE("  \
                "PrimaryKey      INTEGER PRIMARY KEY     NOT NULL," \
                "VehicleID       INT             NOT NULL," \
                "Date            DATETIME DEFAULT CURRENT_TIMESTAMP," \
                "ServiceType     INT             NOT NULL," \
                "Mileage         INT," \
                "Prise           INT," \
                "Description     TEXT);";

            /* Execute SQL statement */
            rc = sqlite3_exec(pDatabase, sql, NULL, 0, NULL);
            if (rc != SQLITE_OK)
            {
                return false;
            }
        }

        // Create SERVICE_TYPE table which store description of service
        {
            char* sql = "CREATE TABLE SERVICE_TYPE("  \
                "ServiceType     INTEGER PRIMARY KEY      NOT NULL," \
                "Description     TEXT);";

            /* Execute SQL statement */
            rc = sqlite3_exec(pDatabase, sql, NULL, 0, NULL);
            if (rc != SQLITE_OK)
            {
                return false;
            }
        }

        // Create EVENT_TYPE table which store description of event
        {
            char* sql = "CREATE TABLE EVENT_TYPE("  \
                "EventType       INTEGER PRIMARY KEY      NOT NULL," \
                "Description     TEXT);";

            /* Execute SQL statement */
            rc = sqlite3_exec(pDatabase, sql, NULL, 0, NULL);
            if (rc != SQLITE_OK)
            {
                return false;
            }
        }

    }

    if (validateDatabase() == false)
    {
        return false;
    }
    _databaseReady = true;

    return true;
}

/*++
Routine Description:

Read last row from EVENTS table for given vehicle

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn

Return Value: EventInfo^. Event info object, nullptr if not succeeded

--*/
EventInfo^ DatabaseEngine::ReadLastEventInfoRow(int vehicleId)
{
    EventInfo^ eventInfo = nullptr;

    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadLastEventInfoRow: pDatabase == NULL, database file not opened");
        return nullptr;
    }

    char* sql = "SELECT * FROM EVENTS WHERE VehicleID = ? ORDER by Mileage DESC LIMIT 1";

    sqlite3_stmt *stmt = NULL;
    int32 rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadLastEventInfoRow: Failed to prepare sql statement, err : %d", rc);
        return nullptr;
    }

    rc = sqlite3_bind_int(stmt, 1, vehicleId);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        eventInfo = ref new EventInfo();

        int col1 = sqlite3_column_int(stmt, 0);                     // PrimaryKey
        int col2 = sqlite3_column_int(stmt, 1);                     // VehicleID
        int64 date = sqlite3_column_int64(stmt, 2);                 // Date
        int col3 = sqlite3_column_int(stmt, 3);                     // EventID
        int col4 = sqlite3_column_int(stmt, 4);                     // Mileage
        int col5 = sqlite3_column_int(stmt, 5);                     // Prise
        wchar_t* col6 = (wchar_t*)sqlite3_column_text16(stmt, 6);   // Desciption

        eventInfo->PrimaryKey = col1;
        eventInfo->VehicleID = col2;
        eventInfo->Date = date;
        eventInfo->EventID = col3;
        eventInfo->Mileage = col4;
        eventInfo->Description = ref new Platform::String(col6);

        rc = sqlite3_reset(stmt);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);
    }
    return rc == SQLITE_OK ? eventInfo : nullptr;
}

/*++
Routine Description:

Read last row from FUELLING table for given vehicle

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn

Return Value: FuelingInfo^. Fueling info object, nullptr if not succeeded

--*/
FuelingInfo^ DatabaseEngine::ReadLastFuellingInfoRow(int vehicleId)
{
    FuelingInfo^ fuelingInfo = nullptr;

    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadLastFuellingInfoRow: pDatabase == NULL, database file not opened");
        return nullptr;
    }

    char* sql = "SELECT * FROM FUELING WHERE VehicleID = ? ORDER by Mileage DESC LIMIT 1";

    sqlite3_stmt *stmt = NULL;
    int32 rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadLastFuellingInfoRow: Failed to prepare sql statement, err : %d", rc);
        return nullptr;
    }

    rc = sqlite3_bind_int(stmt, 1, vehicleId);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        fuelingInfo = ref new FuelingInfo();

        int col1 = sqlite3_column_int(stmt, 0);                     // PrimaryKey
        int col2 = sqlite3_column_int(stmt, 1);                     // VehicleID
        int64 date = sqlite3_column_int64(stmt, 2);                 // Date
        int col3 = sqlite3_column_int(stmt, 3);                     // Amount
        int col4 = sqlite3_column_int(stmt, 4);                     // Mileage
        int col5 = sqlite3_column_int(stmt, 5);                     // Prise
        wchar_t* col6 = (wchar_t*)sqlite3_column_text16(stmt, 6);   // Desciption

        fuelingInfo->PrimaryKey = col1;
        fuelingInfo->VehicleID = col2;
        fuelingInfo->Date = date;
        fuelingInfo->Amount = col3;
        fuelingInfo->Mileage = col4;
        fuelingInfo->Prise = col5;
        fuelingInfo->Description = ref new Platform::String(col6);

        rc = sqlite3_reset(stmt);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);
    }
    return rc == SQLITE_OK ? fuelingInfo : nullptr;
}

/*++
Routine Description:

Read last row from FUELLING table for given vehicle

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn

Return Value: FuelingInfo^. Fueling info object, nullptr if not succeeded

--*/
ServiceInfo^ DatabaseEngine::ReadLastServiceInfoRow(int vehicleId)
{
    ServiceInfo^ serviceInfo = nullptr;

    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadLastServiceInfoRow: pDatabase == NULL, database file not opened");
        return nullptr;
    }

    char* sql = "SELECT * FROM SERVICE WHERE VehicleID = ? ORDER by Mileage DESC LIMIT 1";

    sqlite3_stmt *stmt = NULL;
    int32 rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadLastServiceInfoRow: Failed to prepare sql statement, err : %d", rc);
        return nullptr;
    }

    rc = sqlite3_bind_int(stmt, 1, vehicleId);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        serviceInfo = ref new ServiceInfo();

        int col1 = sqlite3_column_int(stmt, 0);                     // PrimaryKey
        int col2 = sqlite3_column_int(stmt, 1);                     // VehicleID
        int64 date = sqlite3_column_int64(stmt, 2);                 // Date
        int col3 = sqlite3_column_int(stmt, 3);                     // ServiceType
        int col4 = sqlite3_column_int(stmt, 4);                     // Mileage
        int col5 = sqlite3_column_int(stmt, 5);                     // Prise
        wchar_t* col6 = (wchar_t*)sqlite3_column_text16(stmt, 6);   // Desciption

        serviceInfo->PrimaryKey = col1;
        serviceInfo->VehicleID = col2;
        serviceInfo->Date = date;
        serviceInfo->Type = col3;
        serviceInfo->Mileage = col4;
        serviceInfo->Prise = col5;
        serviceInfo->Description = ref new Platform::String(col6);

        rc = sqlite3_reset(stmt);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);
    }
    return rc == SQLITE_OK ? serviceInfo : nullptr;
}

/*++
Routine Description:

Read rows from FUELLING table for given vehicle. Trigger as many ActionInfoRead events
as there is found fueling rows for selected the vehicle.

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn

Return Value: bool. true if succeeded.

--*/
bool DatabaseEngine::ReadAllFuelings(int vehicleId)
{
    FuelingInfo^ fuelingInfo = nullptr;

    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadAllFuelings: pDatabase == NULL, database file not opened");
        return false;
    }

    char* sql = "SELECT * FROM FUELING WHERE VehicleID = ? ORDER BY Mileage DESC";
    int row = 0;
    sqlite3_stmt *stmt = NULL;

    int32 rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadAllFuelings: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, vehicleId);

    while (true)
    {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            fuelingInfo = ref new FuelingInfo();

            int col1 = sqlite3_column_int(stmt, 0);                     // PrimaryKey
            int col2 = sqlite3_column_int(stmt, 1);                     // VehicleID
            int64 date = sqlite3_column_int64(stmt, 2);                 // Date
            int col3 = sqlite3_column_int(stmt, 3);                     // Amount
            int col4 = sqlite3_column_int(stmt, 4);                     // Mileage
            int col5 = sqlite3_column_int(stmt, 5);                     // Prise
            wchar_t* col6 = (wchar_t*)sqlite3_column_text16(stmt, 6);   // Desciption

            fuelingInfo->PrimaryKey = col1;
            fuelingInfo->VehicleID = col2;
            fuelingInfo->Date = date;
            fuelingInfo->Amount = col3;
            fuelingInfo->Mileage = col4;
            fuelingInfo->Prise = col5;
            fuelingInfo->Description = ref new Platform::String(col6);

            // Send fueling info to listeners
            FuelingInfoRead(fuelingInfo);

            // Send fueling info capsulated into action info to listeners
            ActionInfo^ actionInfo = ref new ActionInfo;
            actionInfo->setActionObject(dynamic_cast<Platform::Object^>(fuelingInfo));
            ActionInfoRead(actionInfo);

            row += 1;
        }
        else if (rc == SQLITE_DONE)
        {
            DebugOut("DatabaseEngine::ReadAllEventInfo: All rows read succesfully");
            rc = sqlite3_finalize(stmt);
            break;
        }
        else
        {
            // TBD: Should we do reset here or not ?
            // rc = sqlite3_reset(stmt);
            DebugOut("DatabaseEngine::ReadAllEventInfo: Failed to to step data, err : %d", rc);
            break;
        }
    }

    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Read rows from SERVICE table for given vehicle. Trigger as many ActionInfoRead events
as there is found service rows for selected the vehicle.

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn

Return Value: bool. true if succeeded.

--*/
bool DatabaseEngine::ReadAllServices(int vehicleId)
{
    ServiceInfo^ serviceInfo = nullptr;

    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadAllServices: pDatabase == NULL, database file not opened");
        return false;
    }

    char* sql = "SELECT * FROM SERVICE WHERE VehicleID = ? ORDER BY Mileage DESC";
    int row = 0;
    sqlite3_stmt *stmt = NULL;

    int32 rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadAllServices: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, vehicleId);

    while (true)
    {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            serviceInfo = ref new ServiceInfo();

            int col1 = sqlite3_column_int(stmt, 0);                     // PrimaryKey
            int col2 = sqlite3_column_int(stmt, 1);                     // VehicleID
            int64 date = sqlite3_column_int64(stmt, 2);                 // Date
            int col3 = sqlite3_column_int(stmt, 3);                     // ServiceType
            int col4 = sqlite3_column_int(stmt, 4);                     // Mileage
            int col5 = sqlite3_column_int(stmt, 5);                     // Prise
            wchar_t* col6 = (wchar_t*)sqlite3_column_text16(stmt, 6);   // Desciption

            serviceInfo->PrimaryKey = col1;
            serviceInfo->VehicleID = col2;
            serviceInfo->Date = date;
            serviceInfo->Type = col3;
            serviceInfo->Mileage = col4;
            serviceInfo->Prise = col5;
            serviceInfo->Description = ref new Platform::String(col6);

            // Send service info to listeners
            ServiceInfoRead(serviceInfo);

            // Send fueling info capsulated into action info to listeners
            ActionInfo^ actionInfo = ref new ActionInfo;
            actionInfo->setActionObject(dynamic_cast<Platform::Object^>(serviceInfo));
            ActionInfoRead(actionInfo);

            row += 1;
        }
        else if (rc == SQLITE_DONE)
        {
            DebugOut("DatabaseEngine::ReadAllEventInfo: All rows read succesfully");
            rc = sqlite3_finalize(stmt);
            break;
        }
        else
        {
            // TBD: Should we do reset here or not ?
            // rc = sqlite3_reset(stmt);
            DebugOut("DatabaseEngine::ReadAllEventInfo: Failed to to step data, err : %d", rc);
            break;
        }
    }

    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Read rows from EVENTS table for given vehicle. Trigger as many ActionInfoRead events
as there is found events rows for selected the vehicle.

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn

Return Value: bool. true if succeeded.

--*/
bool DatabaseEngine::ReadAllEvents(int vehicleId)
{
    EventInfo^ eventInfo = nullptr;

    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadAllEventInfo: pDatabase == NULL, database file not opened");
        return false;
    }

    char* sql = "SELECT * FROM EVENTS WHERE VehicleID = ? ORDER BY Mileage DESC";
    int row = 0;
    sqlite3_stmt *stmt = NULL;

    int32 rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadAllEventInfo: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, vehicleId);

    while (true)
    {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            eventInfo = ref new EventInfo();

            int col1 = sqlite3_column_int(stmt, 0);                     // PrimaryKey
            int col2 = sqlite3_column_int(stmt, 1);                     // VehicleID
            int64 date = sqlite3_column_int64(stmt, 2);                 // Date
            int col3 = sqlite3_column_int(stmt, 3);                     // EventID
            int col4 = sqlite3_column_int(stmt, 4);                     // Mileage
            int col5 = sqlite3_column_int(stmt, 5);                     // Prise
            wchar_t* col6 = (wchar_t*)sqlite3_column_text16(stmt, 6);   // Desciption

            eventInfo->PrimaryKey = col1;
            eventInfo->VehicleID = col2;
            eventInfo->Date = date;
            eventInfo->EventID = col3;
            eventInfo->Mileage = col4;
            eventInfo->Prise = col5;
            eventInfo->Description = ref new Platform::String(col6);

            // Send event info to listeners
            EventInfoRead(eventInfo);

            // Send fueling info capsulated into action info to listeners
            ActionInfo^ actionInfo = ref new ActionInfo;
            actionInfo->setActionObject(dynamic_cast<Platform::Object^>(eventInfo));
            ActionInfoRead(actionInfo);

            row += 1;
        }
        else if (rc == SQLITE_DONE)
        {
            DebugOut("DatabaseEngine::ReadAllEventInfo: All rows read succesfully");
            rc = sqlite3_finalize(stmt);
            break;
        }
        else
        {
            // TBD: Should we do reset here or not ?
            // rc = sqlite3_reset(stmt);
            DebugOut("DatabaseEngine::ReadAllEventInfo: Failed to to step data, err : %d", rc);
            break;
        }
    }

    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Read all vehicles from vehicles table of database

Return Value: integer Count of vehicles in the database.
--*/
int DatabaseEngine::ReadShortVehicleInfo()
{
    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadShortVehicleInfo: pDatabase == NULL, database file not opened");
        return -1;
    }

    mVehicleCount = 0;

    char *sql = "SELECT VehicleID, Make, RegPlate, Model, VinCode FROM VECHILES";
    char *err_msg = NULL;

    int32 rc = sqlite3_exec(pDatabase, sql, readShortVehicleInfoCB, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadShortVehicleInfo: Failed to select data, err : %d [%s]", rc, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    return mVehicleCount;
}

/*++
Routine Description:

Read full vehicle data of given vehicle ID from VECHILES table of database. Trigger VehicleInfoRead event
if vehicle is found.

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn

Return Value: bool. if succeeded true.

--*/
bool DatabaseEngine::ReadVehicleData(int vehicleId)
{
    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::ReadVehicleData: pDatabase == NULL, database file not opened");
        return false;
    }

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;
    char *err_msg = NULL;

    char *sql = "SELECT * FROM VECHILES WHERE VehicleID=?;";

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadVehicleData: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, vehicleId);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::ReadVehicleData #3: Failed to bind data, err : %d", rc);
        return false;
    }

    // TBD: while step return SQLITE_ROW
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        VehicleInfo^ VehicleInfo = ref new MasterDetailApp::VehicleInfo;

        VehicleInfo->VehicleId = sqlite3_column_int(stmt, 0);
        VehicleInfo->VinCode = ref new Platform::String((wchar_t*)sqlite3_column_text16(stmt, 1));
        VehicleInfo->Make = ref new Platform::String((wchar_t*)sqlite3_column_text16(stmt, 2));
        VehicleInfo->Model = ref new Platform::String((wchar_t*)sqlite3_column_text16(stmt, 3));
        VehicleInfo->Year = sqlite3_column_int(stmt, 4);
        VehicleInfo->RegPlate = ref new Platform::String((wchar_t*)sqlite3_column_text16(stmt, 5));
        VehicleInfo->Description = ref new Platform::String((wchar_t*)sqlite3_column_text16(stmt, 6));
        VehicleInfo->OdometerUnitId = sqlite3_column_int(stmt, 7);
        VehicleInfo->FuelUnitId = sqlite3_column_int(stmt, 8);

        rc = sqlite3_reset(stmt);

        VehicleInfoRead(VehicleInfo);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);
    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Add new vehicle information into sql database

Arguments:

vehicleInfo - Contain data to be write into database.

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::addVehicle(VehicleInfo^ vehicleInfo)
{
    char* sql = "INSERT INTO VECHILES(Make, Model, Year, FuelUnitId, OdometerUnitId) VALUES(? , ?, ?, ?, ?)";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
    sql,
    -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addVehicle: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'Make'
    if (vehicleInfo->Make->Length())
    {
        std::wstring szMake(vehicleInfo->Make->Data());
        rc = sqlite3_bind_text16(stmt, 1, szMake.c_str(), szMake.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::addVehicle #1: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #2 Column: 'Model'
    if (vehicleInfo->Model->Length())
    {
        std::wstring szModel(vehicleInfo->Model->Data());
        rc = sqlite3_bind_text16(stmt, 2, szModel.c_str(), szModel.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::addVehicle #2: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #3 Column: 'Year'
    if (vehicleInfo->Year > 0)
    {
        rc = sqlite3_bind_int(stmt, 3, vehicleInfo->Year);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::addVehicle #3: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #4 Column: 'FuelUnitId'
    rc = sqlite3_bind_int(stmt, 4, vehicleInfo->FuelUnitId);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addVehicle #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'OdometerUnitId'
    rc = sqlite3_bind_int(stmt, 5, vehicleInfo->FuelUnitId);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addVehicle #5: Failed to bind data, err : %d", rc);
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);

    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Update the existing vehicle information in sql database

Arguments:

vehicleId - Index of vehicle, VECHILES table VehicleID columnn
vehicleInfo - Contain data to be update into database.

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::updateVehicle(int vehicleId, VehicleInfo^ vehicleInfo)
{
    char* sql = "UPDATE VECHILES SET VinCode = ?, Make = ?, Model = ?, Year = ?, RegPlate = ?, Description = ?, FuelUnitId = ?, OdometerUnitId = ? WHERE VehicleID = ?";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateVehicle: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'VinCode'
    if (vehicleInfo->VinCode->Length())
    {
        std::wstring szDescription(vehicleInfo->VinCode->Data());
        rc = sqlite3_bind_text16(stmt, 1, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateVehicle #1: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #2 Column: 'Make'
    if (vehicleInfo->Make->Length())
    {
        std::wstring szDescription(vehicleInfo->Make->Data());
        rc = sqlite3_bind_text16(stmt, 2, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateVehicle #2: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #3 Column: 'Model'
    if (vehicleInfo->Model->Length())
    {
        std::wstring szDescription(vehicleInfo->Model->Data());
        rc = sqlite3_bind_text16(stmt, 3, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateVehicle #3: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #4 Column: 'Year'
    rc = sqlite3_bind_int(stmt, 4, vehicleInfo->Year);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateVehicle #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'RegPlate'
    if (vehicleInfo->RegPlate->Length())
    {
        std::wstring szDescription(vehicleInfo->RegPlate->Data());
        rc = sqlite3_bind_text16(stmt, 5, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateVehicle #5: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #6 Column: 'Description'
    if (vehicleInfo->Description->Length())
    {
        std::wstring szDescription(vehicleInfo->Description->Data());
        rc = sqlite3_bind_text16(stmt, 6, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateVehicle #5: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #7 Column: 'FuelUnitId'
    rc = sqlite3_bind_int(stmt, 7, vehicleInfo->FuelUnitId);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateVehicle #7: Failed to bind data, err : %d", rc);
        return false;
    }

    // #8 Column: 'OdometerUnitId'
    rc = sqlite3_bind_int(stmt, 8, vehicleInfo->OdometerUnitId);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateVehicle #8: Failed to bind data, err : %d", rc);
        return false;
    }

    rc = sqlite3_bind_int(stmt, 9, vehicleId);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateVehicle #9 Failed to bind data, err : %d", rc);
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);

    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Add fuelling event of the existing vehicle

Arguments:

fuellingInfo - Fueling data entered by end user

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::addFuellingInfo(FuelingInfo ^ fuellingInfo)
{
    char* sql = "INSERT INTO FUELING(VehicleID, Date, Amount, Mileage, Prise, Description) VALUES(?,?,?,?,?,?)";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addFuellingInfo: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'VehicleID'
    rc = sqlite3_bind_int(stmt, 1, fuellingInfo->VehicleID);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addFuellingInfo #1: Failed to bind data, err : %d", rc);
        return false;
    }

    // #2 Column: 'Date'
    int64 date = fuellingInfo->Date;
    rc = sqlite3_bind_int64(stmt, 2, date);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addEventInfo #2: Failed to bind data, err : %d", rc);
        return false;
    }

    // #3 Column: 'Amount'
    rc = sqlite3_bind_int(stmt, 3, fuellingInfo->Amount);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addFuellingInfo #3: Failed to bind data, err : %d", rc);
        return false;
    }

    // #4 Column: 'Mileage'
    rc = sqlite3_bind_int(stmt, 4, fuellingInfo->Mileage);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addFuellingInfo #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'Prise'
    rc = sqlite3_bind_int(stmt, 5, fuellingInfo->Prise);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addFuellingInfo #5: Failed to bind data, err : %d", rc);
        return false;
    }

    // #6 Column: 'Description'
    if (fuellingInfo->Description->Length())
    {
        std::wstring szDescription(fuellingInfo->Description->Data());
        rc = sqlite3_bind_text16(stmt, 6, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::addFuellingInfo #5: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);

    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Update existing fueling info

Arguments:

primaryKey - primary key of FUELING table
fuelingInfo - fueling data updated by end user

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::updateFuelingInfo(int primaryKey, FuelingInfo ^ fuelingInfo)
{
    char* sql = "UPDATE FUELING SET Date = ?, Amount = ?, Mileage = ?, Prise = ?, Description = ? WHERE PrimaryKey = ?";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateFuelingInfo: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'Date'
    int64 date = fuelingInfo->Date;
    rc = sqlite3_bind_int64(stmt, 1, date);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateFuelingInfo #1: Failed to bind data, err : %d", rc);
        return false;
    }

    // #2 Column: 'Amount'
    rc = sqlite3_bind_int(stmt, 2, fuelingInfo->Amount);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateFuelingInfo #2: Failed to bind data, err : %d", rc);
        return false;
    }

    // #3 Column: 'Mileage'
    rc = sqlite3_bind_int(stmt, 3, fuelingInfo->Mileage);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateFuelingInfo #3: Failed to bind data, err : %d", rc);
        return false;
    }

    // #4 Column: 'Prise'
    rc = sqlite3_bind_int(stmt, 4, fuelingInfo->Prise);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateFuelingInfo #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'Description'
    if (fuelingInfo->Description->Length())
    {
        std::wstring szDescription(fuelingInfo->Description->Data());
        rc = sqlite3_bind_text16(stmt, 5, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateFuelingInfo #5: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #6 Condition Column: 'PrimaryKey'
    rc = sqlite3_bind_int(stmt, 6, primaryKey);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateEventInfo #6: Failed to bind data, err : %d", rc);
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);

    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Add service event of the existing vehicle

Arguments:

serviceInfo - Service data entered by end user

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::addServiceInfo(ServiceInfo^ serviceInfo)
{
    char* sql = "INSERT INTO SERVICE(VehicleID, Date, ServiceType, Mileage, Prise, Description) VALUES(?,?,?,?,?,?)";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addServiceInfo: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'VehicleID'
    rc = sqlite3_bind_int(stmt, 1, serviceInfo->VehicleID);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addServiceInfo #1: Failed to bind data, err : %d", rc);
        return false;
    }

    // #2 Column: 'Date'
    int64 date = serviceInfo->Date;
    rc = sqlite3_bind_int64(stmt, 2, date);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addServiceInfo #2: Failed to bind data, err : %d", rc);
        return false;
    }

    // #3 Column: 'ServiceType'
    rc = sqlite3_bind_int(stmt, 3, serviceInfo->Type);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addServiceInfo #3: Failed to bind data, err : %d", rc);
        return false;
    }

    // #4 Column: 'Mileage'
    rc = sqlite3_bind_int(stmt, 4, serviceInfo->Mileage);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addServiceInfo #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'Prise'
    rc = sqlite3_bind_int(stmt, 5, serviceInfo->Prise);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addServiceInfo #5: Failed to bind data, err : %d", rc);
        return false;
    }

    // #6 Column: 'Description'
    if (serviceInfo->Description->Length())
    {
        std::wstring szDescription(serviceInfo->Description->Data());
        rc = sqlite3_bind_text16(stmt, 6, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::addFuellingInfo #6: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);
    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Update existing service info

Arguments:

primaryKey - primary key of FUELING table
serviceInfo - service data updated by end user

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::updateServiceInfo(int primaryKey, ServiceInfo ^ serviceInfo)
{
    char* sql = "UPDATE SERVICE SET Date = ?, ServiceType = ?, Mileage = ?, Prise = ?, Description = ? WHERE PrimaryKey = ?";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateServiceInfo: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'Date'
    int64 date = serviceInfo->Date;
    rc = sqlite3_bind_int64(stmt, 1, date);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateServiceInfo #1: Failed to bind data, err : %d", rc);
        return false;
    }

    // #2 Column: 'ServiceType'
    rc = sqlite3_bind_int(stmt, 2, serviceInfo->Type);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateServiceInfo #2: Failed to bind data, err : %d", rc);
        return false;
    }

    // #3 Column: 'Mileage'
    rc = sqlite3_bind_int(stmt, 3, serviceInfo->Mileage);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateServiceInfo #3: Failed to bind data, err : %d", rc);
        return false;
    }

    // #4 Column: 'Prise'
    rc = sqlite3_bind_int(stmt, 4, serviceInfo->Prise);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateServiceInfo #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'Description'
    if (serviceInfo->Description->Length())
    {
        std::wstring szDescription(serviceInfo->Description->Data());
        rc = sqlite3_bind_text16(stmt, 5, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateServiceInfo #5: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #6 Condition Column: 'PrimaryKey'
    rc = sqlite3_bind_int(stmt, 6, primaryKey);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateServiceInfo #6: Failed to bind data, err : %d", rc);
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);

    }
    return rc == SQLITE_OK ? true : false;

    return false;
}

/*++
Routine Description:

Add event of the existing vehicle

Arguments:

eventInfo - Event data entered by end user

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::addEventInfo(EventInfo^ eventInfo)
{
    char* sql = "INSERT INTO EVENTS(VehicleID, Date, EventID, Mileage, Prise, Description) VALUES(?,?,?,?,?,?)";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addEventInfo: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'VehicleID'
    rc = sqlite3_bind_int(stmt, 1, eventInfo->VehicleID);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addEventInfo #1: Failed to bind data, err : %d", rc);
        return false;
    }

    // #2 Column: 'Date'
    int64 date = eventInfo->Date;
    rc = sqlite3_bind_int64(stmt, 2, date);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addEventInfo #2: Failed to bind data, err : %d", rc);
        return false;
    }

    // #3 Column: 'EventID'
    rc = sqlite3_bind_int(stmt, 3, eventInfo->EventID);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addEventInfo #3: Failed to bind data, err : %d", rc);
        return false;
    }

    // #4 Column: 'Mileage'
    rc = sqlite3_bind_int(stmt, 4, eventInfo->Mileage);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addEventInfo #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'Prise'
    rc = sqlite3_bind_int(stmt, 5, eventInfo->Prise);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::addServiceInfo #5: Failed to bind data, err : %d", rc);
        return false;
    }

    // #6 Column: 'Description'
    if (eventInfo-> Description->Length())
    {
        std::wstring szDescription(eventInfo->Description->Data());
        rc = sqlite3_bind_text16(stmt, 6, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::addEventInfo #4: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);

    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Update existing event info

Arguments:

primaryKey - primary key of EVENTS table
eventInfo - Event data updated by end user

Return Value: bool. if succeeded true.
--*/
bool DatabaseEngine::updateEventInfo(int primaryKey, EventInfo^ eventInfo)
{
    char* sql = "UPDATE EVENTS SET Date = ?, EventID = ?, Mileage = ?, Prise = ?, Description = ? WHERE PrimaryKey = ?";

    sqlite3_stmt *stmt = NULL;
    int32 rc = 0;

    rc = sqlite3_prepare_v2(pDatabase,
        sql,
        -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateEventInfo: Failed to prepare sql statement, err : %d", rc);
        return false;
    }

    // #1 Column: 'Date'
    int64 date = eventInfo->Date;
    rc = sqlite3_bind_int64(stmt, 1, date);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateEventInfo #1: Failed to bind data, err : %d", rc);
        return false;
    }

    // #2 Column: 'EventID'
    rc = sqlite3_bind_int(stmt, 2, eventInfo->EventID);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateEventInfo #2: Failed to bind data, err : %d", rc);
        return false;
    }

    // #3 Column: 'Mileage'
    rc = sqlite3_bind_int(stmt, 3, eventInfo->Mileage);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateEventInfo #3: Failed to bind data, err : %d", rc);
        return false;
    }

    // #4 Column: 'Prise'
    rc = sqlite3_bind_int(stmt, 4, eventInfo->Prise);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateEventInfo #4: Failed to bind data, err : %d", rc);
        return false;
    }

    // #5 Column: 'Description'
    if (eventInfo->Description->Length())
    {
        std::wstring szDescription(eventInfo->Description->Data());
        rc = sqlite3_bind_text16(stmt, 5, szDescription.c_str(), szDescription.size() * sizeof(wchar_t), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::updateEventInfo #5: Failed to bind data, err : %d", rc);
            return false;
        }
    }

    // #6 Condition Column: 'PrimaryKey'
    rc = sqlite3_bind_int(stmt, 6, primaryKey);
    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::updateEventInfo #6: Failed to bind data, err : %d", rc);
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        rc = sqlite3_finalize(stmt);

    }
    return rc == SQLITE_OK ? true : false;
}

/*++
Routine Description:

Close database

--*/
void DatabaseEngine::Close()
{
    if (pDatabase != NULL)
    {
        sqlite3_close(pDatabase);
        pDatabase = NULL;
    }
}

/*++
Routine Description:

Open SQLite database file

Arguments:

fileName - Full path of database filename (UTF-16)

Return Value: bool. true if database opened succesfully.
--*/
bool DatabaseEngine::openDatabase(const void * fileName)
{
    if (pDatabase != NULL)
    {
        DebugOut("DatabaseEngine::openDatabase: Database is already opened");
        return false;
    }
    else
    {
        int32 rc = sqlite3_open16(fileName, &pDatabase);
        if (rc != SQLITE_OK)
        {
            DebugOut("DatabaseEngine::openDatabase: Could not open database, err : %d", rc);
            return false;
        }
    }
    return true;
}

/*++
Routine Description:

Validate opened SQLite database

Return Value: bool. true if database has valid Vehicle data tables
--*/
bool DatabaseEngine::validateDatabase()
{
    if (pDatabase == NULL)
    {
        DebugOut("DatabaseEngine::validateDatabase: pDatabase == NULL, database file not opened");
        return false;
    }

    mTableIdentifierMask = 0;

    char *sql = "SELECT name FROM sqlite_master WHERE type='table'";
    char *err_msg = NULL;

    int32 rc = sqlite3_exec(pDatabase, sql, validateCB, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        DebugOut("DatabaseEngine::validateDatabase: Failed to select data, err : %d", rc);
        sqlite3_free(err_msg);
        return false;
    }

    return (mTableIdentifierMask == TableIdentifier_All) ? true : false;
}

/*++
Routine Description:

Callback for the SQLite database query: SELECT name FROM sqlite_master WHERE type='table'.
Set table indetifier bits of mTableIdentifierMask variable

Return Value: int. Always 0.
--*/
int DatabaseEngine::validateCB(void * NotUsed, int argc, char ** argv, char ** azColName)
{
    NotUsed = 0;
    for (int i = 0; i < argc; i++)
    {
        DebugOut("DatabaseEngine@validateCB: %s", argv[i] ? argv[i] : "NULL");
        if (argv[i] != NULL)
        {
            (strcmp(argv[i], "VECHILES"))     ? mTableIdentifierMask |= 0 : mTableIdentifierMask |= TableIdentifier_Vehicles;
            (strcmp(argv[i], "FUELING"))      ? mTableIdentifierMask |= 0 : mTableIdentifierMask |= TableIdentifier_Fueling;
            (strcmp(argv[i], "EVENTS"))       ? mTableIdentifierMask |= 0 : mTableIdentifierMask |= TableIdentifier_Events;
            (strcmp(argv[i], "SERVICE"))      ? mTableIdentifierMask |= 0 : mTableIdentifierMask |= TableIdentifier_Service;
            (strcmp(argv[i], "SERVICE_TYPE")) ? mTableIdentifierMask |= 0 : mTableIdentifierMask |= TableIdentifier_ServiceType;
            (strcmp(argv[i], "EVENT_TYPE"))   ? mTableIdentifierMask |= 0 : mTableIdentifierMask |= TableIdentifier_EventType;
        }

    }
    return 0;
}

/*++
Routine Description:

Callback for the SQLite database query: SELECT * FROM VECHILES. Every founded row (VehicleID)
cause a callback to this method
TBD: Eliminate this callback and use straight next while loop/SQLITE_ROW approach. -> wide characters OK

Return Value: int. Always 0.
--*/
int DatabaseEngine::readFullVehicleInfoCB(void * NotUsed, int argc, char ** argv, char ** azColName)
{
    NotUsed = 0;

    // TBD: If string length exceed MAX_BUFFER it will crash the app
    #define MAX_BUFFER 512
    static wchar_t buffer[MAX_BUFFER];

    VehicleInfo^ VehicleInfo = ref new MasterDetailApp::VehicleInfo;

    for (int i = 0; i < argc; i++)
    {
        DebugOut("readFullVehicleInfoCB: %s", argv[i] ? argv[i] : "NULL");
        //Platform::String^ current = nullptr;

        if (i == 0 && argv[i] != NULL)     // 'VehicleID'
        {
            int vehicleID = 0;
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            int vehicleId = (int)wcstol(buffer, NULL, 10);
            if (vehicleId == 0 ||
                vehicleId == LONG_MAX ||
                vehicleId == LONG_MIN)
            {
                DebugOut("readFullVehicleInfoCB: Error while converting vehicleID string into integer");
            }
            VehicleInfo->VehicleId = converted > 0 ? vehicleID : 0;
        }
        else if (i == 1 && argv[i] != NULL)     // 'VinCode'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->VinCode = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 2 && argv[i] != NULL)          // 'Make'
        {
               
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->Make = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 3 && argv[i] != NULL)     // 'Model'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->Model = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 4 && argv[i] != NULL)     // 'Year'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            int iYear = (int)wcstol(buffer, NULL, 10);
            if (iYear == 0 ||
                iYear == LONG_MAX ||
                iYear == LONG_MIN)
            {
                DebugOut("readFullVehicleInfoCB: Error while converting Year string into integer");
            }
            VehicleInfo->Year = converted > 0 ? iYear : 0;
        }
        else if (i == 5 && argv[i] != NULL)     // 'RegPlate'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->RegPlate = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 6 && argv[i] != NULL)     // 'Description'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->Description = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 7 && argv[i] != NULL)     // 'FuelUnitId'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            int iFuelUnitID = (int)wcstol(buffer, NULL, 10);
            if (iFuelUnitID == 0 ||
                iFuelUnitID == LONG_MAX ||
                iFuelUnitID == LONG_MIN)
            {
                DebugOut("readFullVehicleInfoCB: Error while converting FuelUnitID string into integer");
            }
            VehicleInfo->FuelUnitId = converted > 0 ? iFuelUnitID : 0;
        }
        else if (i ==  8 && argv[i] != NULL)     // 'OdometerUnitId'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            int iOdometerUnitID = (int)wcstol(buffer, NULL, 10);
            if (iOdometerUnitID == 0 ||
                iOdometerUnitID == LONG_MAX ||
                iOdometerUnitID == LONG_MIN)
            {
                DebugOut("readFullVehicleInfoCB: Error while converting OdometerUnitID string into integer");
            }
            VehicleInfo->OdometerUnitId = converted > 0 ? iOdometerUnitID : 0;
        }
    }

    DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));
    if (database != nullptr)
    {
        database->SendVehicleInfoReadEvent(VehicleInfo);
    }

    mVehicleCount += 1;

    return 0;
}

/*++
Routine Description:

Callback for the SQLite database query: SELECT * FROM VECHILES. Every founded row (VehicleID)
cause a callback to this method.
TBD: Eliminate this callback and use straight next while loop/SQLITE_ROW approach. -> wide characters OK


Return Value: int. Always 0.
--*/
int DatabaseEngine::readShortVehicleInfoCB(void * NotUsed, int argc, char ** argv, char ** azColName)
{
    NotUsed = 0;

    // TBD: If string length exceed MAX_BUFFER it will crash the app
#define MAX_BUFFER 512
    static wchar_t buffer[MAX_BUFFER];

    // Short vehicle info contain vehicleID, Make, regPlate, Model and VinCode, other
    // VehicleInfo fiels will be empty
    VehicleInfo^ VehicleInfo = ref new MasterDetailApp::VehicleInfo;

    for (int i = 0; i < argc; i++)
    {
        DebugOut("readShortVehicleInfoCB: %s", argv[i] ? argv[i] : "NULL");
        //Platform::String^ current = nullptr;

        if (i == 0 && argv[i] != NULL)     // 'VehicleID'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            int vehicleId = (int)wcstol(buffer, NULL, 10);
            if (vehicleId == 0 ||
                vehicleId == LONG_MAX ||
                vehicleId == LONG_MIN)
            {
                DebugOut("readShortVehicleInfoCB: Error while converting vehicleID string into integer");
            }
            VehicleInfo->VehicleId = converted > 0 ? vehicleId : 0;
        }
        else if (i == 1 && argv[i] != NULL)          // 'Make'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->Make = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 2 && argv[i] != NULL)         // 'RegPlate'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->RegPlate = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 3 && argv[i] != NULL)     // 'Model'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->Model = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
        else if (i == 4 && argv[i] != NULL)     // 'VinCode'
        {
            size_t converted = 0;
            mbstowcs_s(&converted, buffer, argv[i], MAX_BUFFER);
            VehicleInfo->VinCode = converted > 0 ? ref new Platform::String(buffer) : nullptr;
        }
    }

    DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));
    if (database != nullptr)
    {
        database->SendVehicleInfoReadEvent(VehicleInfo);
    }

    mVehicleCount += 1;

    return 0;
}


