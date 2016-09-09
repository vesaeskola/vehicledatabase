/*++

Module Name:

enginepool.h

Abstract:

This module implements EnginePool. Purpose is to instantiate and maintain all the engine
objects. Engine pool provide access of engine objects for the page object when ever requested.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/


#pragma once

#include "pch.h"

namespace MasterDetailApp
{
    public ref class EnginePool sealed
    {
    public:
        /*++
        Routine Description:

        Constructor

        --*/
        EnginePool();

        /*++
        Routine Description:

        Destructor

        --*/
        virtual ~EnginePool();


        /*++
        Routine Description:

        Return the requsted engine object. Create the object if needed.
        "DatabaseEngine": Open and validate database. Note: Currently the application is able to open files
        only from ApplicationData::Current->LocalFolder->Path

        Arguments:

        engineName - Name of the engine.
        TBD: Change enginename from String to enum type

        Return Value: void*. Pointer to engine object, NULL if engine creation fails.
        --*/
         Platform::Object^ GetEngineObject(Platform::String^ engineName);


    };
}
