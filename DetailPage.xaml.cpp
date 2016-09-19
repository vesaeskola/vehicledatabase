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
using namespace MasterDetailApp::ViewModels;
using namespace concurrency;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
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
using namespace Windows::UI::Xaml::Media::Imaging;

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
            TypeName(MasterDetailApp::VehicleInfo::typeid),
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
    VehicleInfo^ vehicle = dynamic_cast<VehicleInfo^>(e->Parameter);
    if (vehicle != nullptr)
    {
        // Store as shared property
        SelectedVehicle = vehicle;

        // TBD: binding using Selectedvehicle does not work, workaround is here
        make->Text = vehicle->Make;
        model->Text = vehicle->Model;
        regplate->Text = vehicle->RegPlate;
        vincode->Text = vehicle->VinCode;

        DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

        // Read last events:
        EventInfo^ eventInfo = database->ReadLastEventInfoRow(vehicle->VehicleId);
        ServiceInfo^ serviceInfo = database->ReadLastServiceInfoRow(vehicle->VehicleId);
        FuelingInfo^ fuelingInfo = database->ReadLastFuelingInfoRow(vehicle->VehicleId);

        DateTime dateTime1;
        DateTime dateTime2;
        DateTime dateTime3;

        //VehicleInfo->VehicleID = converted > 0 ? vehicleID : 0;

        dateTime1.UniversalTime = eventInfo == nullptr ? 0 : eventInfo->Date;
        dateTime2.UniversalTime = serviceInfo == nullptr ? 0 : serviceInfo->Date;
        dateTime3.UniversalTime = fuelingInfo == nullptr ? 0 : fuelingInfo->Date;

        Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dateForm = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("month.abbreviated day year");

        String^ odoMeterUnit = vehicle->OdometerUnitId == 0 ? L" km " : L" mil. ";

        String^ _DetailContent3_1 = ref new String;
        _DetailContent3_1 = dateTime1.UniversalTime != 0 ? L"Event: " + dateForm->Format(dateTime1) + L" " + eventInfo->Mileage + odoMeterUnit : "No Events";
        DetailContent3_1->Text = _DetailContent3_1;

        String^ _DetailContent3_2 = ref new String;
        _DetailContent3_2 = dateTime2.UniversalTime != 0 ? L"Service: " + dateForm->Format(dateTime2) + L" " + serviceInfo->Mileage + odoMeterUnit : "No Services";
        DetailContent3_2->Text = _DetailContent3_2;

        String^ _DetailContent3_3 = ref new String;
        _DetailContent3_3 = dateTime3.UniversalTime != 0 ? L"Refueling: " + dateForm->Format(dateTime3) + L" " + fuelingInfo->Mileage + odoMeterUnit : "No Refuelings";
        DetailContent3_3->Text = _DetailContent3_3;

        go_event_action->Visibility = eventInfo != nullptr ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
        go_service_action->Visibility = serviceInfo != nullptr ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
        go_fueling_action->Visibility = fuelingInfo != nullptr ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;


        if (vehicle->ImagePath->Length())
        {
            create_task(StorageFile::GetFileFromPathAsync(vehicle->ImagePath)).then([this](task<StorageFile^> t)
            {
                auto storageFile = t.get();
                create_task(storageFile->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this](IRandomAccessStream^ fileStream)
                {
                    BitmapImage^ bitmapImage = ref new BitmapImage();
                    bitmapImage->DecodePixelHeight = (int)this->detail_image_narrow->Height;
                    bitmapImage->SetSource(fileStream);
                    detail_image_narrow->Source = bitmapImage;
                });
            });
        }
        else
        {
            String^ path = ref new String(L"Assets/your_vehicle.png");
            detail_image_narrow->Source = ref new BitmapImage(ref new Windows::Foundation::Uri("ms-appx:///Assets/your_vehicle.png"));
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
                SelectedVehicle,
                masterPageEntry->NavigationTransitionInfo
            );
            backStack->Append(modifiedEntry);
        }
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

Event handler for the button. Open wellcome page.

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

Event handler for the button. Open new fueling page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnFueling_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewFuelingPage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for the button. Open New Service page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnService_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (SelectedVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewServicePage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for the button. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnEvent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (SelectedVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewEventPage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for the button. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnTest_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}

/*++
Routine Description:

Event handler for the button. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnEditVehicle_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsEditVehicle, nullptr);
    Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewVehiclePage::typeid), args);
}

/*++
Routine Description:

Event handler for the button. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnLastEvent_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr && go_event_action->Width)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsEventsList, safe_cast<Platform::Object^>(SelectedVehicle));
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for the button. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnLastService_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr && go_service_action->Width)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsServiceList, safe_cast<Platform::Object^>(SelectedVehicle));
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for the button. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void DetailPage::OnLastFueling_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SelectedVehicle != nullptr && go_fueling_action->Width)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(SelectedVehicle->VehicleId, PageArgs::PageArgsFuelingList, safe_cast<Platform::Object^>(SelectedVehicle));
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
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

User can resize the page view which could case demand to navigate back to detail view, calling
do the trick.

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

/*++
Routine Description:

Event handler for vehicle image tapping. Select a new image for the vehicle.

Arguments:

sender - sender object of this event
e - Event data that can be examined by overriding code

--*/
void DetailPage::OnSelectVehicleImage(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::Thumbnail;
    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    openPicker->FileTypeFilter->Append(".jpg");
    openPicker->FileTypeFilter->Append(".png");
    //openPicker->FileTypeFilter->Append(".sqlite");

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            create_task(file->CopyAsync(ApplicationData::Current->LocalFolder, file->Name, NameCollisionOption::ReplaceExisting)).then([this](StorageFile^ newFile)
            {
                String^ path = newFile->Path;
                this->SetVehicleImage(newFile);
                DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));
                if (database != nullptr && SelectedVehicle != nullptr)
                {
                    database->updateVehicleImagePath(SelectedVehicle->VehicleId, newFile->Path);
                }

            });
        }
    });
}

/*++
Routine Description:

open and set the given file into vehicle image of the page

Arguments:

file - The file to be load

--*/
void DetailPage::SetVehicleImage (StorageFile^ file)
{
    Platform::String^ filePath = file->Path;

    // Ensure the stream is disposed once the image is loaded
    create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this](IRandomAccessStream^ fileStream)
    {
        // Set the image source to the selected bitmap
        BitmapImage^ bitmapImage = ref new BitmapImage();
        bitmapImage->DecodePixelHeight = (int)300;
        bitmapImage->SetSource(fileStream);
        detail_image_narrow->Source = bitmapImage;
    });
}
