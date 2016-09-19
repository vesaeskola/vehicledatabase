/*++

Module Name:

NewEventPage.xaml.cpp

Abstract:

This module implement NewEventPage class. NewEventPage implement user interface to enter and edit vehicle event.
Event could be mileage, accesory purchase, trip, memo or other kind of note. Event contain date, mileage value,
price and general description.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

--*/

#include "pch.h"
#include "NewEventPage.xaml.h"

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

/*++
Routine Description:

Constructor

--*/
NewEventPage::NewEventPage()
    : mActionInfo (nullptr),
      mVehicleID (0),
      mPrimaryKey (0)
{
    InitializeComponent();
}

/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewEventPage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);

    PageNavigateArgs^ args = dynamic_cast<PageNavigateArgs^>(e->Parameter);
    if (args != nullptr)
    {
        if (args->PageArguments == PageArgs::PageArgsEditAction)
        {
            //Populate UI from EventInfo object, existing event will be modified
            mActionInfo = dynamic_cast<ActionInfo^>(args->Object);
            if (mActionInfo != nullptr)
            {
                EventInfo^ eventInfo = dynamic_cast<EventInfo^>(mActionInfo->getActionObject());
                if (eventInfo != nullptr)
                {
                    mPrimaryKey = eventInfo->PrimaryKey;

                    page_title->Text = L"Edit Event";

                    // Here we use C++11 to convert integer to wchar*
                    std::wstring wideString = std::to_wstring(eventInfo->Mileage);
                    textBox_odometer->Text = ref new String(wideString.c_str());
                    textBox_price->Text = IntToPlatformString(eventInfo->Price);
                    textBox_description->Text = eventInfo->Description;

                    DateTime dateTime;
                    dateTime.UniversalTime = eventInfo->Date;
                    date_picker->Date = dateTime;

                    int eventType = eventInfo->EventID;
                    if (eventType == 0x01) RadioButton_odometer->IsChecked = true;
                    else if (eventType == 0x02) RadioButton_accessory->IsChecked = true;
                    else if (eventType == 0x03) RadioButton_trip->IsChecked = true;
                    else if (eventType == 0x04) RadioButton_memo->IsChecked = true;
                    else if (eventType == 0x05) RadioButton_other->IsChecked = true;
                }
            }
        }
        else if (args->PageArguments == PageArgs::PageArgsNone)
        {
            // New event will be entered
            mVehicleID = args->VehicleID;
        }
    }

    // Register for hardware and software back request from the system
    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    m_backRequestedEventRegistrationToken =
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs ^>(this, &NewEventPage::NewEventPage_BackRequested);
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
}

/*++
Routine Description:

Invoked when the Page is unloaded.

Arguments:

e - Event data that can be examined by overriding code

--*/
void NewEventPage::OnNavigatedFrom(NavigationEventArgs ^ e)
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
void NewEventPage::OnBack_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
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
void NewEventPage::OnDone_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (gDatabase != nullptr)
    {
        EventInfo^ eventInfo = PopulateEventInfoObject();
        if (mPrimaryKey != 0)
        {
            // Update existing event
            if (gDatabase->updateEventInfo(mPrimaryKey, eventInfo) == false)
            {
                // TBD: Open error dialog from here
                DebugOut("NewEventPage::OnDone_Click: updateEventInfo failed");
            }
        }
        else if (mVehicleID > 0)
        {
            // Create a new event
            if (gDatabase->addEventInfo(eventInfo) == false)
            {
                // TBD: Open error dialog from here
                DebugOut("NewEventPage::OnDone_Click: addEventInfo failed");
            }
        }
        else
        {
            DebugOut("NewEventPage::OnDone_Click: System error, vechileID not available");
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
void NewEventPage::NewEventPage_BackRequested(Object ^sender, BackRequestedEventArgs ^args)
{
    // Mark event as handled so we don't get bounced out of the app.
    args->Handled = true;

    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Populate data from UI to EventInfo object

Return Value: EventInfo^. EventInfo object contain data from UI.

--*/
EventInfo^ NewEventPage::PopulateEventInfoObject()
{
    EventInfo^ eventInfo = ref new EventInfo();

    // 'VehicleID'
    eventInfo->VehicleID = mVehicleID;

    if (textBox_price->Text->Length())
    {
        eventInfo->Price = PlatformStringToInt(textBox_price->Text);
    }
    if (textBox_odometer->Text->Length())
    {
        eventInfo->Mileage = _wtoi(textBox_odometer->Text->Data());
    }
    int eventType = 0;

    if (RadioButton_odometer->IsChecked->Value == true) eventType = 1;
    else if (RadioButton_accessory->IsChecked->Value == true) eventType = 2;
    else if (RadioButton_trip->IsChecked->Value == true) eventType = 3;
    else if (RadioButton_memo->IsChecked->Value == true) eventType = 4;
    else if (RadioButton_other->IsChecked->Value == true) eventType = 5;
    eventInfo->EventID = eventType;

    DateTime dateTime = date_picker->Date;
    int64 globalDate = dateTime.UniversalTime;
    eventInfo->Date = globalDate;

    eventInfo->Description = textBox_description->Text;
    eventInfo->VehicleID = mVehicleID;

    return eventInfo;
}
