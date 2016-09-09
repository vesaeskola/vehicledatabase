/*++

Module Name:

DetailPage.xaml.cpp

Abstract:

This module implements DetailPage class. DetailPage implement detail view user interface for
the Vehicle Database application.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/

//
// DetailPage.xaml.cpp
// Implementation of the DetailPage class
//

#include "pch.h"
#include "DetailPage.xaml.h"

using namespace MasterDetailApp;
//using namespace MasterDetailApp::Data;
using namespace MasterDetailApp::ViewModels;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

/*++
Routine Description:

Constructor

--*/
DetailPage::DetailPage()
    : m_backRequestedEventRegistrationToken(), m_sizeChangedEventRegistrationToken()
{
    InitializeComponent();
}

// See https://msdn.microsoft.com/en-us/library/hh920267.aspx
DependencyProperty^ DetailPage::s_itemProperty = nullptr;

/*++
Routine Description:

Called from App.xaml.cpp. Create dependency property from ItemViewModel object to DetailPage object.
This ItemViewModel object is set in the OnNavigateTo method to point to selected item of ListView

--*/
void DetailPage::RegisterDependencyProperties()
{
    s_itemProperty =
        DependencyProperty::Register(
            L"SelectedVehicle",
            TypeName(VehicleListItem::typeid),
            TypeName(DetailPage::typeid),
            ref new PropertyMetadata(nullptr)
            );
}

/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.

Arguments:

e - Event data that can be examined by overriding code

--*/
void DetailPage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);

    // Parameter is selected item object from the list view
    VehicleListItem^ vehicleListItem = dynamic_cast<VehicleListItem^>(e->Parameter);
    if (vehicleListItem != nullptr)
    {
        SelectedVehicle = vehicleListItem;

        // TBD: binding using Selectedvehicle does not work, workaround is here
        this->make->Text = vehicleListItem->Make;
        this->model->Text = vehicleListItem->Model;
        this->regplate->Text = vehicleListItem->RegPlate;
        this->vincode->Text = vehicleListItem->VinCode;

        DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

        // Read last events:
        EventInfo^ eventInfo = database->ReadLastEventInfoRow(SelectedVehicle->VehicleId);
        ServiceInfo^ serviceInfo = database->ReadLastServiceInfoRow(SelectedVehicle->VehicleId);
        FuelingInfo^ fuelingInfo = database->ReadLastFuellingInfoRow(SelectedVehicle->VehicleId);

        DateTime dateTime1;
        DateTime dateTime2;
        DateTime dateTime3;

        //VehicleInfo->VehicleID = converted > 0 ? vehicleID : 0;

        dateTime1.UniversalTime = eventInfo == nullptr ? 0 : eventInfo->Date;
        dateTime2.UniversalTime = serviceInfo == nullptr ? 0 : serviceInfo->Date;
        dateTime3.UniversalTime = fuelingInfo == nullptr ? 0 : fuelingInfo->Date;

        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = Windows::Globalization::DateTimeFormatting::DateTimeFormatter::ShortDate::get();

        String^ _DetailContent3_1 = ref new String;
        _DetailContent3_1 = L"Event: ";
        _DetailContent3_1 += dateTime1.UniversalTime != 0 ? dtf->Format(dateTime1) + L" [" + eventInfo->Mileage + L" km]" : "---";
        this->DetailContent3_1->Text = _DetailContent3_1;

        String^ _DetailContent3_2 = ref new String;
        _DetailContent3_2 = L"Service: ";
        _DetailContent3_2 += dateTime2.UniversalTime != 0 ? dtf->Format(dateTime2) + L" [" + serviceInfo->Mileage + L" km]" : "---";
        this->DetailContent3_2->Text = _DetailContent3_2;

        String^ _DetailContent3_3 = ref new String;
        _DetailContent3_3 = L"Fueling: ";
        _DetailContent3_3 += dateTime3.UniversalTime != 0 ? dtf->Format(dateTime3) + L" [" + fuelingInfo->Mileage + L" km]" : "---";
        this->DetailContent3_3->Text = _DetailContent3_3;

        this->go_event_action->Width = eventInfo != nullptr ? 85 : 0;
        this->go_service_action->Width = serviceInfo != nullptr ? 85 : 0;
        this->go_fueling_action->Width = fuelingInfo != nullptr ? 85 : 0;
    }


    auto backStack = Frame->BackStack;
    auto backStackCount = backStack->Size;

    if (backStackCount > 0)
    {
        auto masterPageEntry = backStack->GetAt(backStackCount - 1);
        backStack->RemoveAt(backStackCount - 1);

        // Doctor the navigation parameter for the master page so it
        // will show the correct item in the side-by-side view.
        auto modifiedEntry = ref new PageStackEntry(
            masterPageEntry->SourcePageType,
            SelectedVehicle,   // vai Selectedvehicle->VehicleID  ??
            masterPageEntry->NavigationTransitionInfo
            );
        backStack->Append(modifiedEntry);
    }

    // Register for hardware and software back request from the system
    SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
    m_backRequestedEventRegistrationToken =
        systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs ^>(this, &DetailPage::DetailPage_BackRequested);
    systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
}

/*++
Routine Description:

Invoked when the Page is unloaded.

Arguments:

e - Event data that can be examined by overriding code

--*/
void DetailPage::OnNavigatedFrom(NavigationEventArgs ^ e)
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
void DetailPage::OnBack_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

/*++
Routine Description:

Event handler for menu panel. Open new fuelling page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnFueling_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewFuellingPage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for menu panel. Open New Service page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnService_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (SelectedVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewServicePage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for menu panel. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnEvent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (SelectedVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewEventPage::typeid), args);
    }
}

void DetailPage::OnTest_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    /*
    if (m_lastSelectedItem != nullptr)
    {
        int vehicleID = m_lastSelectedItem->VehicleId;
        DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

        if (database->ReadAllEvents(vehicleID) == false)
        {
            NotifyUser("Events reading failed", NotifyType::ErrorMessage);
        }
        else
        {
            NotifyUser("All events read succesfully", NotifyType::StatusMessage);
        }
    }
    */
}

void DetailPage::OnEditVehicle_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsEditVehicle, nullptr);
    this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewVehiclePage::typeid), args);
}

void DetailPage::OnLastEvent_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr && this->go_event_action->Width)
    {
        // TBD: Here we need much more intelligent method to deliver basic vehicle data between pages
        VehicleInfo^ currentVehicle = ref new VehicleInfo;
        currentVehicle->Make = SelectedVehicle->Make;
        currentVehicle->Model = SelectedVehicle->Model;
        currentVehicle->RegPlate = SelectedVehicle->RegPlate;
        currentVehicle->VinCode = SelectedVehicle->VinCode;

        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsEventsList, safe_cast<Platform::Object^>(currentVehicle));
        this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
}

void DetailPage::OnLastService_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr && this->go_service_action->Width)
    {
        // TBD: Here we need much more intelligent method to deliver basic vehicle data between pages
        VehicleInfo^ currentVehicle = ref new VehicleInfo;
        currentVehicle->Make = SelectedVehicle->Make;
        currentVehicle->Model = SelectedVehicle->Model;
        currentVehicle->RegPlate = SelectedVehicle->RegPlate;
        currentVehicle->VinCode = SelectedVehicle->VinCode;

        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsServiceList, safe_cast<Platform::Object^>(currentVehicle));
        this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
}

void DetailPage::OnLastFueling_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr && this->go_fueling_action->Width)
    {
        // TBD: Here we need much more intelligent method to deliver basic vehicle data between pages
        VehicleInfo^ currentVehicle = ref new VehicleInfo;
        currentVehicle->Make = SelectedVehicle->Make;
        currentVehicle->Model = SelectedVehicle->Model;
        currentVehicle->RegPlate = SelectedVehicle->RegPlate;
        currentVehicle->VinCode = SelectedVehicle->VinCode;

        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsFuelingList, safe_cast<Platform::Object^>(currentVehicle));
        this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
}
/*++
Routine Description:

Event handler for the list view. Occurs after a control transitions to a different state.

Arguments:

sender - sender object of this event
e - Event data that can be examined by overriding code

--*/
void DetailPage::AdaptiveStates_CurrentStateChanged(Object^ sender, VisualStateChangedEventArgs^ e)
{
    UpdateForVisualState(e->NewState, e->OldState);
}

/*++
Routine Description:

Handle list view item selection if page size or orientation is changed

Arguments:

newState - New visual state
oldState - Old visual state

--*/
void DetailPage::UpdateForVisualState(VisualState ^ newState, VisualState ^ oldState)
{
    auto isNarrow = newState == NarrowState;
    // TBD: Is there somethig to be done ?
}

/*++
Routine Description:

???

Arguments:

useTransition - true if transition should be used

--*/
void DetailPage::NavigateBackForWideState(bool useTransition)
{
    // Evict this page from the cache as we may not need it again.
    NavigationCacheMode = Windows::UI::Xaml::Navigation::NavigationCacheMode::Disabled;

    if (useTransition)
    {
        Frame->GoBack(ref new EntranceNavigationTransitionInfo());
    }
    else
    {
        Frame->GoBack(ref new SuppressNavigationTransitionInfo());
    }
}

/*++
Routine Description:

Check if current page is wide (> 720.0 pixels)

Return Value: bool. true if current page is < 720 pixels wide

--*/
bool DetailPage::ShouldGoToWideState()
{
    return Window::Current->Bounds.Width >= 720.0;
}

/*++
Routine Description:

Handler for page loading

Arguments:

e - Event arguments

--*/
void DetailPage::PageRoot_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    if (ShouldGoToWideState())
    {
        // We shouldn't see this page since we are in "wide master-detail" mode.
        // Play a transition as we are likely navigating back from a separate page.
        NavigateBackForWideState(/* useTransition */ true);
    }
    else
    {
        // Realize the main page content.
        FindName("RootPanel");
    }

    m_sizeChangedEventRegistrationToken =
        Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &DetailPage::Window_SizeChanged);
}

/*++
Routine Description:

Handler for page unloading

Arguments:

e - Event arguments

--*/
void DetailPage::PageRoot_Unloaded(Object^ sender, RoutedEventArgs^ e)
{
    Window::Current->SizeChanged -= m_sizeChangedEventRegistrationToken;
}

/*++
Routine Description:

Handler for window size change

Arguments:

e - Event arguments

--*/
void DetailPage::Window_SizeChanged(Object ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e)
{
    if (ShouldGoToWideState())
    {
        // Make sure we are no longer listening to window change events.
        Window::Current->SizeChanged -= m_sizeChangedEventRegistrationToken;

        // We shouldn't see this page since we are in "wide master-detail" mode.
        NavigateBackForWideState(/* useTransition */ false);
    }
}

/*++
Routine Description:

Event handler for the hardware and software back request from the system

Arguments:

e - Event arguments

--*/
void DetailPage::DetailPage_BackRequested(Object ^sender, BackRequestedEventArgs ^args)
{
    // Mark event as handled so we don't get bounced out of the app.
    args->Handled = true;

    Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}



