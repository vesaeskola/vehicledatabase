/*++

Module Name:

NewFuellingPage.xaml.cpp

Abstract:

This module declare NewFuellingPage class. NewFuellingPage implement user interface to enter and edit fuelling of vehicle.
Fuelling could contain mileage, date, amunt, cost and general description.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

*/

#include "pch.h"
#include "NewFuellingPage.xaml.h"

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
extern MasterDetailApp::DatabaseEngine^ gDatabase;

NewFuellingPage::NewFuellingPage()
    : mVehicleID (-1)
{
    InitializeComponent();
}

/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewFuellingPage::OnNavigatedTo(NavigationEventArgs ^ e)
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
                FuelingInfo^ fuelingInfo = dynamic_cast<FuelingInfo^>(mActionInfo->getActionObject());
                if (fuelingInfo != nullptr)
                {
                    //Populate UI from FuelingInfo object, existing event will be modified
                    mPrimaryKey = fuelingInfo->PrimaryKey;

                    this->page_title->Text = L"Edit Fueling";

                    // Here we use C++11 to convert integer to wchar*
                    std::wstring wideString = std::to_wstring(fuelingInfo->Mileage);
                    textBox_odometer->Text = ref new String(wideString.c_str());

                    wideString = std::to_wstring(fuelingInfo->Amount );
                    textBox_amount->Text = ref new String(wideString.c_str());

                    wideString = std::to_wstring(fuelingInfo->Prise);
                    textBox_price->Text = ref new String(wideString.c_str());

                    textBox_description->Text = fuelingInfo->Description;

                    DateTime dateTime;
                    dateTime.UniversalTime = fuelingInfo->Date;
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
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs ^>(this, &NewFuellingPage::NewFuelingPage_BackRequested);
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
}

/*++
Routine Description:

Invoked when the Page is unloaded.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewFuellingPage::OnNavigatedFrom(NavigationEventArgs ^ e)
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
void NewFuellingPage::OnBack_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
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
void NewFuellingPage::OnDone_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (gDatabase != nullptr)
    {
        if (mPrimaryKey != 0)
        {
            // Update existing service action
            FuelingInfo^ fuelingInfo = PopulateFuelingInfoObject();
            if (gDatabase->updateFuelingInfo(mPrimaryKey, fuelingInfo) == false)
            {
                // TBD: Open error dialog from here
                DebugOut("NewFuelingPage::OnDone_Click: updateServiceInfo failed");
            }
        }
        else if (mVehicleID != -1)
        {
            FuelingInfo^ fuelingInfo = ref new FuelingInfo();

            if (textBox_amount->Text->Length())
            {
                fuelingInfo->Amount = _wtoi(textBox_amount->Text->Data());
            }
            if (textBox_odometer->Text->Length())
            {
                fuelingInfo->Mileage = _wtoi(textBox_odometer->Text->Data());
            }
            if (textBox_price->Text->Length())
            {
                fuelingInfo->Prise = _wtoi(textBox_price->Text->Data());
            }

            DateTime dateTime = this->date_picker->Date;
            int64 globalDate = dateTime.UniversalTime;
            fuelingInfo->Date = globalDate;

            fuelingInfo->Description = textBox_description->Text;
            fuelingInfo->VehicleID = mVehicleID;

            if (gDatabase->addFuellingInfo(fuelingInfo) == false)
            {
                // TBD: Open error dialog from here
                DebugOut("NewFuellingPage::OnDone_Click: addFuellingInfo failed");
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
void NewFuellingPage::NewFuelingPage_BackRequested(Platform::Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ args)
{
    // Mark event as handled so we don't get bounced out of the app.
    args->Handled = true;

    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Populate data from UI to fuelingInfo object

Return Value: FuelingInfo^. FuelingInfo object contain data from UI.

--*/
FuelingInfo^ NewFuellingPage::PopulateFuelingInfoObject()
{
    FuelingInfo^ fuelingInfo = ref new FuelingInfo();

    if (textBox_price->Text->Length())
    {
        fuelingInfo->Prise = _wtoi(textBox_price->Text->Data());
    }
    if (textBox_odometer->Text->Length())
    {
        fuelingInfo->Mileage = _wtoi(textBox_odometer->Text->Data());
    }

    fuelingInfo->Amount = _wtoi(textBox_amount->Text->Data());

    DateTime dateTime = this->date_picker->Date;
    int64 globalDate = dateTime.UniversalTime;
    fuelingInfo->Date = globalDate;

    fuelingInfo->Description = textBox_description->Text;

    return fuelingInfo;
}
