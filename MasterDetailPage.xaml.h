/*++

Module Name:

MasterDetailPage.xaml.h

Abstract:

This module declare MasterDetailPage class. MasterDetailPage implement list view user interface for
the Vehicle Database application.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

--*/

#pragma once

#include "MasterDetailPage.g.h"
#include "pch.h"


namespace MasterDetailApp
{
    public enum class NotifyType
    {
        StatusMessage,
        ErrorMessage
    };

    /*++
    Description:

    An empty page that can be used on its own or navigated to within a Frame.

    --*/
    public ref class MasterDetailPage sealed
    {
    public:
        /*++
        Routine Description:

        Constructor

        --*/
        MasterDetailPage();

        /*++
        Routine Description:

        Event handler for the DatabaseEngine object. Each row found from VECHILES table cause this event
        handler triggered.

        Arguments:

        vehicle - Vehicle info, one row of data from VECHILES table. 
        --*/
        void VehicleInfoEventHandler(VehicleInfo^ vehicle);

        /*++
        Routine Description:

        Load the given file into vehicle image of the page

        Arguments:

        file - The file to be load

        --*/
        void SetVehicleImage(Windows::Storage::StorageFile^ file);

    protected:
        /*++
        Routine Description:

        Invoked when the Page is loaded and becomes the current source of a parent Frame.

        Arguments:

        e - Event data that can be examined by overriding code

        --*/
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        //MasterDetailApp::ViewModels::VehicleListItem^ m_lastSelectedItem;

        MasterDetailApp::VehicleInfo^ mCurrentVehicle;

        /*++
        Routine Description:

        Handle list view item selection if page size is changed

        Arguments:

        newState - New visual state
        oldState - New visual state

        --*/
        void UpdateForVisualState(Windows::UI::Xaml::VisualState^ newState, Windows::UI::Xaml::VisualState^ oldState = nullptr);

        /*++
        Routine Description:

        Play a refresh animation when the user switches detail items

        --*/
        void EnableContentTransitions();

        /*++
        Routine Description:

        Don't play a content transition for first item load.
        Sometimes, this content will be animated as part of the page transition.

        --*/
        void DisableContentTransitions();

        /*++
        Routine Description:

        Called when list view grid layout root load. Set the selected item of list view 

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void LayoutRoot_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Set the text into status pane. Text could be notification or error info.

        Arguments:

        strMessage - Message to be show
        type - Notification type, ErrorMessage will be shown with red background

        --*/
        void NotifyUser(Platform::String^ strMessage, NotifyType type);

        /*++
        Routine Description:

        Event handler for the list view. Occurs after a control transitions to a different state.

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void AdaptiveStates_CurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::VisualStateChangedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for the list view. Occurs after a item is selected.

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void MasterListView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);

        /*++
        Routine Description:

        Get the rect of the event sender object. Used for the context menu.

        Arguments:

        element - sender object of the open context menu event

        Return Value: Rect. 

        --*/
        Windows::Foundation::Rect GetElementRect(Windows::UI::Xaml::FrameworkElement^ element);

        /*++
        Routine Description:

        Event handler to open context menu

        Arguments:

        sender - sender object of the open context menu event

        --*/
        void ContextMenu_Open(Object^ sender);

        /*++
        Routine Description:

        Event handler for the context menu. Valled when user have selected menu item from context menu.

        Arguments:

        sender - sender object of the open context menu event

        --*/
        void CommandInvokedHandler(Windows::UI::Popups::IUICommand^ command);

        /*++
        Routine Description:

        Event handler for the detail view tapping

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void DetailView_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for the list item right tapping

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void List_Item_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler of 'Vehicle' button. Navigate to New Vehicle page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnNewVehicle_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler of 'Fueling' button. Navigate to New Vehicle page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnFueling_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler of 'Service' button. Navigate to NewService page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnService_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for menu panel. Not implemented.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnEvent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for menu panel. Navigate back to MasterDetail page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnTest_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for menu panel. Navigate to NewVehiclePage using edit vehicle mode.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnEditVehicle_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for menu panel. Navigate back to MasterDetail page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnLastEvent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for menu panel. Navigate back to MasterDetail page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnLastService_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for menu panel. Navigate back to MasterDetail page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnLastFueling_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Import database file. Currently not used.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnImportDatabase_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Export database file. Currently not used.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnExportDatabase_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);


        /*++
        Routine Description:

        Event handler for vehicle image tapping. Select a new image for the vehicle.

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void OnSelectVehicleImage(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);


    private:
        void MasterListView_ItemSelected(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
};
}
