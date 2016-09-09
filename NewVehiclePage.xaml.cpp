/*++

Module Name:

NewServicePage.xaml.cpp

Abstract:

This module implement NewVehiclePage class. NewVehiclePage implement user interface to enter and edit vehicle information.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

*/

#include "pch.h"
#include "NewVehiclePage.xaml.h"

using namespace MasterDetailApp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

/*++
Routine Description:

Constructor

--*/
NewVehiclePage::NewVehiclePage()
{
    InitializeComponent();
}

/*++
Routine Description:

Invoked when DatabaseEngine object has read data of current vehicle from database.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewVehiclePage::VehicleInfoEventHandler(VehicleInfo^ vehicle)
{
    DebugOut("NewVehiclePage::VehicleInfoEventHandler");

    textBox_Make->Text = vehicle->Make;
    textBox_Model->Text = vehicle->Model;
    // Here we use C++11 to convert integer to wchar*
    if (vehicle->Year > 0)
    {
        std::wstring wideString = std::to_wstring(vehicle->Year);
        textBox_Year->Text = ref new String(wideString.c_str());
    }
    
    RadioButton_unit_gallon->IsChecked = vehicle->FuelUnitId == 1 ? true : false;
    RadioButton_unit_liter->IsChecked = vehicle->FuelUnitId == 1 ? false : true;
    RadioButton_unit_km->IsChecked = vehicle->OdometerUnitId == 1 ? false : true;
    RadioButton_unit_miles->IsChecked = vehicle->OdometerUnitId == 1 ? true : false;
    textBox_description->Text = vehicle->Description;
    textBox_RegPlate->Text = vehicle->RegPlate;
    textBox_VIN->Text = vehicle->VinCode;
}


/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewVehiclePage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);

    PageNavigateArgs^ args = dynamic_cast<PageNavigateArgs^>(e->Parameter);
    if (args != nullptr)
    {
        if (args->PageArguments == PageArgs::PageArgsEditVehicle)
        {
            DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));
            if (database == nullptr)
            {
                auto e = ref new Exception(-1, "Fatal SW error. Database engine object is not instantiated.");
                throw e;
            }

            page_title->Text = L"Edit Vehicle";

            // Existing vehicle will be modified
            mVehicleID = args->VehicleID;

            // Read current vehicle data into UI, event handler populate UI
            EventRegistrationToken token = database->VehicleInfoRead += ref new MasterDetailApp::VehicleInfoHandler(this, &NewVehiclePage::VehicleInfoEventHandler);
            if (database->ReadVehicleData(mVehicleID) == false)
            {
                DebugOut("NewVehiclePage::OnNavigatedTo: ReadVehicleData failed");
            }
            database->VehicleInfoRead -= token;

        }
        else if (args->PageArguments == PageArgs::PageArgsNone)
        {
            // New vehicle will be entered
            mVehicleID = -1;
        }
    }
    // Register for hardware and software back request from the system
    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    m_backRequestedEventRegistrationToken =
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs ^>(this, &NewVehiclePage::NewVehiclePage_BackRequested);
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
}

/*++
Routine Description:

Invoked when the Page is unloaded.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewVehiclePage::OnNavigatedFrom(NavigationEventArgs ^ e)
{
    Page::OnNavigatedFrom(e);

    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    systemNavigationManager->BackRequested -= m_backRequestedEventRegistrationToken;
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Collapsed;
}

/*++
Routine Description:

Event handler for menu panel. Open wellcome page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailApp::NewVehiclePage::OnBack_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Event handler of the Done button. Navigate back to previous page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void NewVehiclePage::OnDone_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (gEnginePool == nullptr)
    {
        auto e = ref new Exception(-1, "Fatal SW error. Engine pool object is not instantiated.");
        throw e;
    }
    DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));
    if (database == nullptr)
    {
        auto e = ref new Exception(-1, "Fatal SW error. Database engine object is not instantiated.");
        throw e;
    }

    if (mVehicleID == -1)
    {
        VehicleInfo^ vehicleInfo = PopulateVehicleInfoObject();
        if (database->addVehicle(vehicleInfo) == false)
        {
            DebugOut("NewVehiclePage::OnDone_Click: addVehicle failed");
        }
    }
    else
    {
        VehicleInfo^ vehicleInfo = PopulateVehicleInfoObject();
        if (database->updateVehicle(mVehicleID, vehicleInfo) == false)
        {
            DebugOut("NewVehiclePage::OnDone_Click: updateVehicle failed");
        }

    }

    PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsVehicleEntered, nullptr);
    this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(MasterDetailPage::typeid), args);
}

/*++
Routine Description:

Event handler for the hardware and software back request from the system

Arguments:

e - Event arguments

--*/
void NewVehiclePage::NewVehiclePage_BackRequested(Platform::Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ args)
{
    // Mark event as handled so we don't get bounced out of the app.
    args->Handled = true;

    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Populate data from UI to VehicleInfo object

Return Value: VehicleInfo^. VehicleInfo object contain data from UI.

--*/
VehicleInfo^ NewVehiclePage::PopulateVehicleInfoObject()
{
    VehicleInfo^ vehicleInfo = ref new VehicleInfo();

    vehicleInfo->Make = textBox_Make->Text;
    vehicleInfo->Model = textBox_Model->Text;

    if (textBox_Year->Text->Length())
    {
        vehicleInfo->Year = _wtoi(textBox_Year->Text->Data());
    }

    vehicleInfo->Description = textBox_description->Text;
    vehicleInfo->RegPlate = textBox_RegPlate->Text;
    vehicleInfo->VinCode = textBox_VIN->Text;

    vehicleInfo->FuelUnitId = RadioButton_unit_liter->IsChecked ? 0 : 1;     // 0 = liter, 1 = gallons
    vehicleInfo->OdometerUnitId = RadioButton_unit_km->IsChecked ? 0 : 1;    // 0 = km, 1 = mails

    return vehicleInfo;
}

