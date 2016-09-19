/*++

Module Name:

pch.h

Abstract:

Header for standard system include files.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

--*/


#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <memory.h>
#include <fstream>


#include "App.xaml.h"
#include "dataobjects.h"
#include "datamodels.h"

#include "WellcomePage.xaml.h"
#include "MasterDetailPage.xaml.h"
#include "NewVehiclePage.xaml.h"
#include "NewFuelingPage.xaml.h"
#include "NewEventPage.xaml.h"
#include "NewServicePage.xaml.h"
#include "EventListpage.xaml.h"
#include "DetailPage.xaml.h"

#include "dbengine.h"
#include "sql_lite/sqlite3.h"
#include "enginepool.h"
#include "debug.h"
#include "winrt_utils.h"

extern MasterDetailApp::EnginePool^ gEnginePool;


