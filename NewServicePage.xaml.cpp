/*++

Module Name:

NewServicePage.xaml.cpp

Abstract:

This module implement NewServicePage class. NewServicePage implement user interface to enter and edit service of vehicle.
Service could contain service type, mileage, date, cost and general description.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

*/

#include "pch.h"
#include "NewServicePage.xaml.h"

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

extern MasterDetailApp::DatabaseEngine^ gDatabase;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

NewServicePage::NewServicePage()
{
    InitializeComponent();
}

/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewServicePage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);

    PageNavigateArgs^ args = dynamic_cast<PageNavigateArgs^>(e->Parameter);
    if (args != nullptr)
    {
        if (args->PageArguments == PageArgs::PageArgsEditAction)
        {
            mActionInfo = dynamic_cast<ActionInfo^>(args->Object);
            if (mActionInfo != nullptr)
            {
                ServiceInfo^ serviceInfo = dynamic_cast<ServiceInfo^>(mActionInfo->getActionObject());
                if (serviceInfo != nullptr)
                {
                    //Populate UI from FuelingInfo object, existing event will be modified
                    mPrimaryKey = serviceInfo->PrimaryKey;

                    page_title->Text = L"Edit Service";

                    // Here we use C++11 to convert integer to wchar*
                    std::wstring wideString = std::to_wstring(serviceInfo->Mileage);
                    textBox_odometer->Text = ref new String(wideString.c_str());

                    textBox_price->Text = IntToPlatformString(serviceInfo->Price);
                    textBox_description->Text = serviceInfo->Description;

                    int serviceType = serviceInfo->Type;

                    if (serviceType & 0x01) checkBox_oil->IsChecked = true;
                    if (serviceType & 0x02) checkBox_oil_filter->IsChecked = true;
                    if (serviceType & 0x04) checkBox_air_filter->IsChecked = true;
                    if (serviceType & 0x08) checkBox_fuel_filter->IsChecked = true;
                    if (serviceType & 0x10) checkBox_other->IsChecked = true;

                    DateTime dateTime;
                    dateTime.UniversalTime = serviceInfo->Date;
                    date_picker->Date = dateTime;
                }
            }
        }
        else if (args->PageArguments == PageArgs::PageArgsNone)
        {
            mVehicleID = args->VehicleID;
        }
    }
    // Register for hardware and software back request from the system
    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    m_backRequestedEventRegistrationToken =
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs ^>(this, &NewServicePage::NewServicePage_BackRequested);
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
}

/*++
Routine Description:

Invoked when the Page is unloaded.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewServicePage::OnNavigatedFrom(NavigationEventArgs ^ e)
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
void NewServicePage::OnBack_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
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
void NewServicePage::OnDone_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (gDatabase != nullptr)
    {
        ServiceInfo^ serviceInfo = PopulateServiceInfoObject();

        if (mPrimaryKey != 0)
        {
            // Update existing service action
            if (gDatabase->updateServiceInfo(mPrimaryKey, serviceInfo) == false)
            {
                // TBD: Open error dialog from here
                DebugOut("NewServicePage::OnDone_Click: updateServiceInfo failed");
            }
        }
        else if (mVehicleID != -1)
        {
            if (gDatabase->addServiceInfo(serviceInfo) == false)
            {
                // TBD: Open error dialog from here
                DebugOut("NewServicePage::OnDone_Click: addServiceInfo failed");
            }
        }
    }

    // TBD: Opening page should be updated to show new entry ?
    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Event handler for the hardware and software back request from the system

Arguments:

e - Event arguments

--*/
void NewServicePage::NewServicePage_BackRequested(Platform::Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ args)
{
    // Mark event as handled so we don't get bounced out of the app.
    args->Handled = true;

    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Populate data from Ui to ServiceInfo object

Return Value: EventInfo^. ServiceInfo object contain data from UI.

--*/
ServiceInfo^ NewServicePage::PopulateServiceInfoObject()
{
    ServiceInfo^ serviceInfo = ref new ServiceInfo();

    // 'VehicleID'
    serviceInfo->VehicleID = mVehicleID;

    if (textBox_price->Text->Length())
    {
        serviceInfo->Price = PlatformStringToInt(textBox_price->Text);

    }
    if (textBox_odometer->Text->Length())
    {
        serviceInfo->Mileage = _wtoi(textBox_odometer->Text->Data());
    }

    int serviceType = 0;
    serviceType |= (checkBox_oil->IsChecked->Value == true) ? 0x01 : 0x00;
    serviceType |= (checkBox_oil_filter->IsChecked->Value == true) ? 0x02 : 0x00;
    serviceType |= (checkBox_air_filter->IsChecked->Value == true) ? 0x04 : 0x00;
    serviceType |= (checkBox_fuel_filter->IsChecked->Value == true) ? 0x08 : 0x00;
    serviceType |= (checkBox_other->IsChecked->Value == true) ? 0x10 : 0x00;
    serviceInfo->Type = serviceType;

    DateTime dateTime = date_picker->Date;
    int64 globalDate = dateTime.UniversalTime;
    serviceInfo->Date = globalDate;

    serviceInfo->Description = textBox_description->Text;

    return serviceInfo;
}
