/*++

Module Name:

EventListPage.xaml.cpp

Abstract:

This module implement EventListPage class. EventListPage implement view with ListView control to show
vehicle actions like services, fuelings and other events.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/

#include "pch.h"
#include "EventListPage.xaml.h"

using namespace MasterDetailApp;
using namespace MasterDetailApp::ViewModels;


using namespace Platform;
using namespace Platform::Collections;

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
EventListPage::EventListPage()
    : m_backRequestedEventRegistrationToken()
{
    InitializeComponent();
}

/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.

Arguments:

e - Event data that can be examined by overriding code

--*/
void EventListPage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);

    // Connect datasource and list view together. If necessary create data source object
    ActionListItemCollector^ actionListCollector = dynamic_cast<ActionListItemCollector^>(gEnginePool->GetEngineObject(L"ActionListItemCollector"));
    Vector<ActionListItem^>^ actionListItems = dynamic_cast<Vector<ActionListItem^>^>(gEnginePool->GetEngineObject(L"ActionList"));
    DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

    if (actionListCollector == nullptr ||
        actionListItems == nullptr ||
        database == nullptr)
    {
        auto exception = ref new Exception(-1, "Fatal SW error. Engine objects can't be found.");
        throw exception;
    }

    PageNavigateArgs^ args = dynamic_cast<PageNavigateArgs^>(e->Parameter);
    if (args != nullptr)
    {
        // Set event sink to catch read rows from database
        Windows::Foundation::EventRegistrationToken cookie = database->ActionInfoRead += ref new MasterDetailApp::ActionInfoHandler(actionListCollector, &ActionListItemCollector::ActionInfoEventHandler);

        VehicleInfo^ vehicleInfo = dynamic_cast<VehicleInfo^>(args->Object);
        if (vehicleInfo != nullptr)
        {
            make->Text = vehicleInfo->Make;
            model->Text = vehicleInfo->Model;
            regplate->Text = vehicleInfo->RegPlate;
            vincode->Text = vehicleInfo->VinCode;
        }
        actionListItems->Clear();

        switch (args->PageArguments)
        {
            case PageArgs::PageArgsEventsList:
            {
                if (database->ReadAllEvents(args->VehicleID) == false)
                {
                    DebugOut("EventListPage::OnNavigatedTo: ReadAllEvents failed");
                }
                title->Text = L"Events";
                break;
            }
            case PageArgs::PageArgsServiceList:
            {
                if (database->ReadAllServices(args->VehicleID) == false)
                {
                    DebugOut("EventListPage::OnNavigatedTo: ReadAllServices failed");
                }
                title->Text = L"Services";
                break;
            }
            case PageArgs::PageArgsFuelingList:
            {
                if (database->ReadAllFuelings(args->VehicleID) == false)
                {
                    DebugOut("EventListPage::OnNavigatedTo: ReadAllFuelings failed");
                }
                title->Text = L"Fuelings";
                break;
            }
            default:
            {
                DebugOut("EventListPage::OnNavigatedTo: Invalid page navigate arguments");
                break;
            }
        }


        database->ActionInfoRead -= cookie;

        if (actionListItems->Size)
        {
            EventListView->ItemsSource = actionListItems;
            EventListView->SelectedIndex = 0;
        }
    }

    // Register for hardware and software back request from the system
    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    m_backRequestedEventRegistrationToken =
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs ^>(this, &EventListPage::EventListPage_BackRequested);
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
}

/*++
Routine Description:

Invoked when the Page is unloaded.

Arguments:

e - Event data that can be examined by overriding code

--*/
void EventListPage::OnNavigatedFrom(NavigationEventArgs ^ e)
{
    Page::OnNavigatedFrom(e);

    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    systemNavigationManager->BackRequested -= m_backRequestedEventRegistrationToken;
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Collapsed;
}

/*++
Routine Description:

TBD: Clarify if this is needed

Arguments:

e - Event data that can be examined by overriding code

--*/
void EventListPage::LayoutRoot_Loaded(Object^ sender, RoutedEventArgs^ e)
{
}

/*++
Routine Description:

Event handler for the list view. Occurs after a control transitions to a different state.
TBD: Clarify if this is needed

Arguments:

sender - sender object of this event
e - Event data that can be examined by overriding code

--*/
void EventListPage::AdaptiveStates_CurrentStateChanged(Object^ sender, VisualStateChangedEventArgs^ e)
{
    UpdateForVisualState(e->NewState, e->OldState);
}

/*++
Routine Description:

Handle list view item selection if page size or orientation is changed
TBD: Clarify if this is needed

Arguments:

newState - New visual state
oldState - Old visual state

--*/
void EventListPage::UpdateForVisualState(VisualState ^ newState, VisualState ^ oldState)
{
}

/*++
Routine Description:

Event handler for the list view item click. Navigate and open in edit mode NewEvent, NewFueling or NewService page to allow
user edit the action item.

Arguments:

sender - sender object of this event
e - Event data that can be examined by overriding code

--*/
void EventListPage::EventListView_EventItemClick(Object^ sender, ItemClickEventArgs^ e)
{
    ActionListItem^ clickedItem = dynamic_cast<ActionListItem^>(e->ClickedItem);
    if (clickedItem != nullptr)
    {
        ActionInfo^ actionInfo = clickedItem->GetActionInfo();

        if (actionInfo != nullptr)
        {
            Platform::Object^ object = actionInfo->getActionObject();

            if (dynamic_cast<ServiceInfo^>(object) != nullptr)
            {
                PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsEditAction, safe_cast<Platform::Object^>(actionInfo));
                Frame->Navigate(::Interop::TypeName(NewServicePage::typeid), args);
            }
            else if (dynamic_cast<FuelingInfo^>(object) != nullptr)
            {
                PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsEditAction, safe_cast<Platform::Object^>(actionInfo));
                Frame->Navigate(::Interop::TypeName(NewFuellingPage::typeid), args);
            }
            else if (dynamic_cast<EventInfo^>(object) != nullptr)
            {
                PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsEditAction, safe_cast<Platform::Object^>(actionInfo));
                Frame->Navigate(::Interop::TypeName(NewEventPage::typeid), args);
            }
        }
    }
}

/*++
Routine Description:

Event handler back button. Navigate to previous page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void EventListPage::OnBack_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Event handler for the hardware and software back request from the system

Arguments:

e - Event arguments

--*/
void EventListPage::EventListPage_BackRequested(Object ^sender, BackRequestedEventArgs ^args)
{
    // Mark event as handled so we don't get bounced out of the app.
    args->Handled = true;

    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

