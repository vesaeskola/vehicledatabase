/*++

Module Name:

MasterDetailPage.xaml.cpp

Abstract:

This module implement MasterDetailPage class. MasterDetailPage implement list view user interface for
the Vehicle Database application.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

--*/

#include "pch.h"
#include "MasterDetailPage.xaml.h"


using namespace concurrency;        // for create_task method

using namespace MasterDetailApp;
using namespace MasterDetailApp::ViewModels;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Popups;
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
MasterDetailPage::MasterDetailPage()
{
    InitializeComponent();
}

/*++
Routine Description:

Invoked when DatabaseEngine object has read data of current vehicle from database.

Arguments:

e - Event data that can be examined by overriding code

--*/
void MasterDetailPage::VehicleInfoEventHandler(VehicleInfo^ vehicle)
{
    DebugOut("MasterDetailPage::VehicleInfoEventHandler");
    DetailContent6->Text = vehicle->Description;

    DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));
    if (database == nullptr)
    {
        DebugOut("MasterDetailPage::VehicleInfoEventHandler: database not available");
        return;
    }

    // Read last events:
    EventInfo^ eventInfo = database->ReadLastEventInfoRow(vehicle->VehicleId);
    ServiceInfo^ serviceInfo = database->ReadLastServiceInfoRow(vehicle->VehicleId);
    FuelingInfo^ fuelingInfo = database->ReadLastFuelingInfoRow(vehicle->VehicleId);

    DateTime dateTime1;
    DateTime dateTime2;
    DateTime dateTime3;

    dateTime1.UniversalTime = eventInfo == nullptr ? 0 : eventInfo->Date;
    dateTime2.UniversalTime = serviceInfo == nullptr ? 0 : serviceInfo->Date;
    dateTime3.UniversalTime = fuelingInfo == nullptr ? 0 : fuelingInfo->Date;

    Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dtf = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("month.abbreviated day year");

    String^ odoMeterUnit = vehicle->OdometerUnitId == 0 ? L" km " : L" mil. ";

    String^ _DetailContent3_1 = ref new String;
    _DetailContent3_1 = dateTime1.UniversalTime != 0 ? L"Event: " + dtf->Format(dateTime1) + L" " + eventInfo->Mileage + odoMeterUnit : "---";
    DetailContent3_1->Text = _DetailContent3_1;

    String^ _DetailContent3_2 = ref new String;
    _DetailContent3_2 = dateTime2.UniversalTime != 0 ? L"Service: " + dtf->Format(dateTime2) + L" " + serviceInfo->Mileage + odoMeterUnit : "---";
    DetailContent3_2->Text = _DetailContent3_2;

    String^ _DetailContent3_3 = ref new String;
    _DetailContent3_3 = dateTime3.UniversalTime != 0 ? L"Refueling: " + dtf->Format(dateTime3) + L" " + fuelingInfo->Mileage + odoMeterUnit : "---";
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

    mCurrentVehicle = vehicle;
}

/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.

Arguments:

e - Event data that can be examined by overriding code
TBD: Consider how to handle list view focus handling in different use cases.

--*/
void MasterDetailPage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);

    VehicleListItemCollector^ vehicleListItemCollector = dynamic_cast<VehicleListItemCollector^>(gEnginePool->GetEngineObject(L"VehicleListItemCollector"));
    Vector<VehicleListItem^>^ vehicleListItems = dynamic_cast<Vector<VehicleListItem^>^>(gEnginePool->GetEngineObject(L"VehicleList"));
    DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

    if (vehicleListItemCollector == nullptr ||
        vehicleListItems == nullptr ||
        database == nullptr)
    {
        DebugOut("MasterDetailPage::OnNavigatedTo: Engine objects not available");
        return;
    }

    PageNavigateArgs^ args = dynamic_cast<PageNavigateArgs^>(e->Parameter);

    if (args != nullptr)
    {
        int selectedIndex = MasterListView->SelectedIndex > 0 ? MasterListView->SelectedIndex : 0;

        if (args->PageArguments == PageArgs::PageArgsAppLaunched || args->PageArguments == PageArgs::PageArgsVehicleEntered || args->PageArguments == PageArgs::PageArgsVehicleModified)
        {
            // Clear previous vchile list view
            vehicleListItems->Clear();

            // Read all vehicles into list view.
            //TBD: What to do if list remains empty
            EventRegistrationToken token = database->VehicleInfoRead += ref new MasterDetailApp::VehicleInfoHandler(vehicleListItemCollector, &VehicleListItemCollector::VehicleInfoEventHandler);
            int vehicleCount = database->ReadShortVehicleInfo();

            database->VehicleInfoRead -= token;

            // Set the list model for the list view control
            MasterListView->ItemsSource = vehicleListItems;

            // Autoselect first vehicle if none is selected.
            if (vehicleCount)
            {
                MasterListView->SelectedIndex = MasterListView->SelectedIndex < 0 ? 0 : MasterListView->SelectedIndex;
            }
        }
        if (args->PageArguments == PageArgs::PageArgsVehicleEntered)
        {
            // If new vehicle was entered, select the latest vehicle
            int Size = dynamic_cast<Vector<VehicleListItem^>^>(MasterListView->ItemsSource)->Size;
            MasterListView->SelectedIndex = Size-1;
            // Read the detail data of selected vehicle
            selectedIndex = MasterListView->SelectedIndex > 0 ? MasterListView->SelectedIndex : 0;
        }
        else if (args->PageArguments == PageArgs::PageArgsVehicleModified)
        {
            MasterListView->SelectedIndex = selectedIndex;
        }
        auto vehicleToRead = vehicleListItems->GetAt(selectedIndex);

        // Subscribe vehicle data read events
        EventRegistrationToken token = database->VehicleInfoRead += ref new MasterDetailApp::VehicleInfoHandler(this, &MasterDetailPage::VehicleInfoEventHandler);

        if (database->ReadVehicleData(vehicleToRead->VehicleId) == false)
        {
            NotifyUser("ReadVehicleData failed [code: 1]", NotifyType::ErrorMessage);
        }

        database->VehicleInfoRead -= token;
    }
    if (mCurrentVehicle != nullptr)
    {
        // Ensure the current vechile data is uptodate
        EventRegistrationToken token = database->VehicleInfoRead += ref new MasterDetailApp::VehicleInfoHandler(this, &MasterDetailPage::VehicleInfoEventHandler);
        if (database->ReadVehicleData(mCurrentVehicle->VehicleId) == false)
        {
            NotifyUser("ReadVehicleData failed [code: 2]", NotifyType::ErrorMessage);
        }
        database->VehicleInfoRead -= token;
    }

    UpdateForVisualState(AdaptiveStates->CurrentState);

    // Don't play a content transition for first item load.
    // Sometimes, this content will be animated as part of the page transition.
    DisableContentTransitions();
}

/*++
Routine Description:

Handle list view item selection if page size or orientation is changed

Arguments:

newState - New visual state
oldState - Old visual state

--*/
void MasterDetailPage::UpdateForVisualState(VisualState ^ newState, VisualState ^ oldState)
{
    bool isWideState = newState == DefaultState;

    // If not wide state
    if (!isWideState && oldState == DefaultState && mCurrentVehicle != nullptr)
    {
        Frame->Navigate(TypeName(DetailPage::typeid), mCurrentVehicle, ref new SuppressNavigationTransitionInfo());
    }

    EntranceNavigationTransitionInfo::SetIsTargetElement(MasterListView, !isWideState);
    if (DetailContentPresenter != nullptr)
    {
        EntranceNavigationTransitionInfo::SetIsTargetElement(DetailContentPresenter, isWideState);
    }
}

/*++
Routine Description:

Play a refresh animation when the user switches detail items

--*/
void MasterDetailPage::EnableContentTransitions()
{
    DetailContentPresenter->ContentTransitions->Clear();
    DetailContentPresenter->ContentTransitions->Append(ref new EntranceThemeTransition());
}

/*++
Routine Description:

Don't play a content transition for first item load.
Sometimes, this content will be animated as part of the page transition.

--*/

void MasterDetailPage::DisableContentTransitions()
{
    if (DetailContentPresenter != nullptr)
    {
        DetailContentPresenter->ContentTransitions->Clear();
    }
}

/*++
Routine Description:

Called when list view grid layout root load. Set the selected item of list view

Arguments:

e - Event data that can be examined by overriding code

--*/
void MasterDetailPage::LayoutRoot_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    // TBD: Assure we are displaying the correct item. This is necessary in certain adaptive cases.
}

/*++
Routine Description:

Set the text into status pane. Text could be notification or error info.

Arguments:

strMessage - Message to be show
type - Notification type, ErrorMessage will be shown with red background

--*/
void MasterDetailPage::NotifyUser(String^ strMessage, NotifyType type)
{
    switch (type)
    {
    case NotifyType::StatusMessage:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::LightGreen);
        break;
    case NotifyType::ErrorMessage:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Red);
        break;
    default:
        break;
    }
    StatusBlock->Text = strMessage;

    // Collapse the StatusBlock if it has no text to conserve real estate.
    if (StatusBlock->Text != "")
    {
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
    else
    {
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
}

/*++
Routine Description:

Event handler for the list view. Occurs after a control transitions to a different state.

Arguments:

sender - sender object of this event
e - Event data that can be examined by overriding code

--*/
void MasterDetailPage::AdaptiveStates_CurrentStateChanged(Object^ sender, VisualStateChangedEventArgs^ e)
{
    UpdateForVisualState(e->NewState, e->OldState);
}

/*++
Routine Description:

Event handler for the list view. Occurs after a item is selected.

Arguments:

sender - sender object of this event
e - Event data that can be examined by overriding code

--*/
void MasterDetailPage::MasterListView_ItemClick(Object^ sender, ItemClickEventArgs^ e)
{
    VehicleListItem^ clickedItem = dynamic_cast<VehicleListItem^>(e->ClickedItem);

    if (clickedItem != nullptr)
    {
        // Read the clicked item vehicle data from database, if not already read
        if (mCurrentVehicle == nullptr || clickedItem->VehicleId != mCurrentVehicle->VehicleId)
        {
            mCurrentVehicle = nullptr;

            // Need to read vechile data into mCurrentVehicle
            DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

            EventRegistrationToken token = database->VehicleInfoRead += ref new MasterDetailApp::VehicleInfoHandler(this, &MasterDetailPage::VehicleInfoEventHandler);
            if (database->ReadVehicleData(clickedItem->VehicleId) == false)
            {
                NotifyUser("ReadVehicleData failed [code: 3]", NotifyType::ErrorMessage);
            }
            database->VehicleInfoRead -= token;
        }

        // Check if reading was OK
        if (mCurrentVehicle != nullptr)
        {
            if (AdaptiveStates->CurrentState != DefaultState)
            {
                // Narrow or mid state, Navigate to detail page to show detail information
                // Use "drill in" transition for navigating from master list to detail view
                Frame->Navigate(TypeName(DetailPage::typeid), mCurrentVehicle, ref new DrillInNavigationTransitionInfo());
            }
            else
            {
                // Detail information is already in this page
                // Play a refresh animation when the user switches detail items.
                EnableContentTransitions();
            }
        }
    }
}

/*++
Routine Description:

Get the rect of the event sender object. Used for the context menu.

Arguments:

element - sender object of the open context menu event

Return Value: Rect.

--*/
Rect MasterDetailPage::GetElementRect(FrameworkElement^ element)
{
    GeneralTransform^ buttonTransform = element->TransformToVisual(nullptr);
    const Point pointOrig(0, 0);
    const Point pointTransformed = buttonTransform->TransformPoint(pointOrig);
    const Rect rect(pointTransformed.X, pointTransformed.Y, safe_cast<float>(element->ActualWidth), safe_cast<float>(element->ActualHeight));
    return rect;
}

/*++
Routine Description:

Event handler to open context menu

Arguments:

sender - sender object of the open context menu event

--*/
void MasterDetailPage::ContextMenu_Open(Object^ sender)
{
    // Create a menu and add commands specifying a callback delegate for each.
    // Since command delegates are unique, no need to specify command Ids.

    auto menu = ref new PopupMenu();
    menu->Commands->Append(ref new UICommand("Menu 1", ref new UICommandInvokedHandler([this](IUICommand^ command)
    {
        ShowMessageDialogAsync(ref new Platform::String(L"Context 'Menu 1' selected"));
    })));

    menu->Commands->Append(ref new UICommand("Menu 2", ref new UICommandInvokedHandler([this](IUICommand^ command)
    {
        ShowMessageDialogAsync(ref new Platform::String(L"Context 'Menu 2' selected"));
    })));

    // We don't want to obscure content, so pass in a rectangle representing the sender of the context menu event.
    auto rect = GetElementRect(safe_cast<FrameworkElement^>(sender));
    create_task(menu->ShowForSelectionAsync(rect)).then([this](IUICommand^ command)
    {
        // Context menu dismissed
        ;
    });
    // Context menu shown

}

/*++
Routine Description:

Event handler for the context menu. Called when user have selected menu item from context menu.

Arguments:

sender - sender object of the open context menu event

--*/
void MasterDetailPage::CommandInvokedHandler(Windows::UI::Popups::IUICommand^ command)
{
    if (command->Label == "Pick a file")
    {
        //selectDatabaseFile();
    }
    else if (command->Label == "Select location")
    {
        FolderPicker^ folderPicker = ref new FolderPicker();
        folderPicker->SuggestedStartLocation = PickerLocationId::Desktop;

        // Users expect to have a filtered view of their folders depending on the scenario.
        // For example, when choosing a documents folder, restrict the filetypes to documents for your application.
        folderPicker->FileTypeFilter->Append(".db");
        folderPicker->FileTypeFilter->Append(".sqlite");

        create_task(folderPicker->PickSingleFolderAsync()).then([this](StorageFolder^ folder)
        {
            if (folder)
            {
                ;
            }
        });
    }
    else if (command->Label == "Create")
    {
        // Get working directory
        String^ workFilePath = ApplicationData::Current->LocalFolder->Path;

    }
}

/*++
Routine Description:

Event handler for the detail view tapping

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::DetailView_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    ShowMessageDialogAsync(ref new Platform::String(L"Detail view tapped"));
}

/*++
Routine Description:

Event handler for the list item right tapping

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::List_Item_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e)
{
    ContextMenu_Open(sender);
}

/*++
Routine Description:

Event handler for menu panel. Open new vehicle page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnNewVehicle_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsNone, nullptr);
    Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewVehiclePage::typeid), args);
}

/*++
Routine Description:

Event handler for menu panel. Open new fueling page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnFueling_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (mCurrentVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(mCurrentVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewFuelingPage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for menu panel. Open New Service page.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnService_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (mCurrentVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(mCurrentVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewServicePage::typeid), args);
    }
}

/*++
Routine Description:

Event handler for menu panel. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnEvent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (mCurrentVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(mCurrentVehicle->VehicleId, PageArgs::PageArgsNone, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewEventPage::typeid), args);
    }
}

/*++
Routine Description:

Only for testing purpose.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnTest_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}

/*++
Routine Description:

Event handler of the button. Read and open page for all events of selected vehicle.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnEditVehicle_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (mCurrentVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(mCurrentVehicle->VehicleId, PageArgs::PageArgsEditVehicle, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewVehiclePage::typeid), args);
    }
    /*
    else if (m_lastSelectedItem != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(m_lastSelectedItem->VehicleId, PageArgs::PageArgsEditVehicle, nullptr);
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewVehiclePage::typeid), args);
    }
    */
}

/*++
Routine Description:

Event handler of the 'LastEvent' button. Read and open page for all events of selected vehicle.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnLastEvent_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (mCurrentVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(mCurrentVehicle->VehicleId, PageArgs::PageArgsEventsList, safe_cast<Platform::Object^>(mCurrentVehicle));
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
    else
    {
        NotifyUser("Select vehicle to open event list, [code: 4]", NotifyType::ErrorMessage);
    }
}

/*++
Routine Description:

Event handler of the 'LastService' button. Read and open page for all services of selected vehicle.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnLastService_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (mCurrentVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(mCurrentVehicle->VehicleId, PageArgs::PageArgsServiceList, safe_cast<Platform::Object^>(mCurrentVehicle));
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
    else
    {
        NotifyUser("Select vehicle to open serice list", NotifyType::ErrorMessage);
    }
}

/*++
Routine Description:

Event handler of the 'LastFueling' button. Read and open page for all fuelings of selected vehicle.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnLastFueling_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (mCurrentVehicle != nullptr)
    {
        PageNavigateArgs^ args = ref new PageNavigateArgs(mCurrentVehicle->VehicleId, PageArgs::PageArgsFuelingList, safe_cast<Platform::Object^>(mCurrentVehicle));
        Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(EventListPage::typeid), args);
    }
    else
    {
        NotifyUser("Select vehicle to open refueling list, [code: 5]", NotifyType::ErrorMessage);
    }
}

/*++
Routine Description:

Event handler for menu panel. Import SQLite database file

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnImportDatabase_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::Thumbnail;
    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    openPicker->FileTypeFilter->Append(".db");
    openPicker->FileTypeFilter->Append(".sqlite");

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            // Close the existing database if already open, this is done because we overwrite the default database file
            // with the one user has select
            DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

            database->CloseDatabase();

            create_task(file->CopyAsync(ApplicationData::Current->LocalFolder, "vehicle_data.db", NameCollisionOption::ReplaceExisting)).then([this](StorageFile^ newFile)
            {
                NotifyUser("Import db clicked: Database file copied succesfully to documents folder\nThis feature is not yet implemented, this file can't be opened", NotifyType::StatusMessage);

                String^ dbFilePath = newFile->Path;
            });
        }
    });
}

/*++
Routine Description:

Event handler for the button. Not implemented.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::OnExportDatabase_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    NotifyUser("Export db clicked: Not implemented", NotifyType::StatusMessage);
}


/*++
Routine Description:

Event handler for list view control. Does nothing

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void MasterDetailPage::MasterListView_ItemSelected(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    DebugOut("MasterDetailPage: item selected");

    auto item = MasterListView->SelectedItem;
    auto index = MasterListView->SelectedIndex;
}

/*++
Routine Description:

Event handler for vehicle image tapping. Select a new image for the vehicle.

Arguments:

sender - sender object of this event
e - Event data that can be examined by overriding code

--*/
void MasterDetailPage::OnSelectVehicleImage(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::Thumbnail;
    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    openPicker->FileTypeFilter->Append(".jpg");
    openPicker->FileTypeFilter->Append(".png");

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            create_task(file->CopyAsync(ApplicationData::Current->LocalFolder, file->Name, NameCollisionOption::ReplaceExisting)).then([this](StorageFile^ newFile)
            {
                String^ path = newFile->Path;
                this->SetVehicleImage(newFile);
                DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));
                if (database != nullptr && mCurrentVehicle != nullptr)
                {
                    database->updateVehicleImagePath(mCurrentVehicle->VehicleId, newFile->Path);
                }

            });
        }
    });
}

/*++
Routine Description:

Open and set the given file into vehicle image of the page

Arguments:

file - The file to be load

--*/
void MasterDetailPage::SetVehicleImage(StorageFile^ file)
{
    Platform::String^ filePath = file->Path;

    // Ensure the stream is disposed once the image is loaded
    create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([this](IRandomAccessStream^ fileStream)
    {
        // Set the image source to the selected bitmap
        BitmapImage^ bitmapImage = ref new BitmapImage();
        bitmapImage->DecodePixelHeight = (int)300;
        //bitmapImage->DecodePixelHeight = this->detail_image_narrow->Height;
        bitmapImage->SetSource(fileStream);
        detail_image_narrow->Source = bitmapImage;
    });
}

