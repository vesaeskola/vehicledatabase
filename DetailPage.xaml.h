/*++

Module Name:

DetailPage.xaml.h

Abstract:

This module declare DetailPage class. DetailPage implement detail view user interface for
the Vehicle Database application. Detailpage is shown in cases when tehre is narrow screen
available like e.g mobile phones:

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/


#pragma once

#include "DetailPage.g.h"
#include "MasterDetailPage.g.h"

namespace MasterDetailApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DetailPage sealed
    {
    public:
        /*++
        Routine Description:

        Constructor

        --*/
        DetailPage();

        /*++
        Routine Description:

        Load the given file into vehicle image of the page

        Arguments:

        file - The file to be load

        --*/
        void SetVehicleImage(Windows::Storage::StorageFile^ file);

        /*++
        Property Description:

        SelectedVehicle. Shared property from MasterDetail view ListView control. Note. this is not
        fully utilized and should be eliminated.

        --*/
        property MasterDetailApp::VehicleInfo^ SelectedVehicle
        {
            MasterDetailApp::VehicleInfo^ get()
            {
                return safe_cast<MasterDetailApp::VehicleInfo^>(GetValue(s_itemProperty));
            }
            void set(MasterDetailApp::VehicleInfo^ value)
            {
                SetValue(s_itemProperty, value);
            }
        }
    protected:
        /*++
        Routine Description:

        Invoked when the Page is loaded and becomes the current source of a parent Frame.

        Arguments:

        e - Event data that can be examined by overriding code

        --*/
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        /*++
        Routine Description:

        Invoked when the Page is unloaded.

        Arguments:

        e - Event data that can be examined by overriding code

        --*/
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    internal:
        /*++
        Routine Description:

        Called from App.xaml.cpp. Register dependency property from ItemViewModel object to DetailPage object.
        This ItemViewModel object is set in the OnNavigateTo method to point to selected item of ListView

        --*/
        static void RegisterDependencyProperties();

    private:
        static Windows::UI::Xaml::DependencyProperty^ s_itemProperty;

    private:
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

        Handle list view item selection if page size is changed

        Arguments:

        newState - New visual state
        oldState - New visual state

        --*/
        void UpdateForVisualState(Windows::UI::Xaml::VisualState^ newState, Windows::UI::Xaml::VisualState^ oldState = nullptr);

        /*++
        Routine Description:

        Event handler of 'Back' button. Navigate back to Wellcome page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnBack_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler of 'Vehicle' button. Navigate to New Vehicle page.

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

        Event handler of 'Event' button.. Not implemented.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnEvent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler of 'Test' button.. Navigate back to MasterDetail page.

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

        Event handler for last event text/button. Navigate back to MasterDetail page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnLastEvent_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for last service text/button. Navigate back to MasterDetail page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnLastService_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for fueling event text/button. Navigate back to MasterDetail page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnLastFueling_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        User can resize the page view which could case demand to navigate back to detail view, calling
        do the trick.

        Arguments:

        useTransition - true if transition should be used

        --*/
        void NavigateBackForWideState(bool useTransition);

        /*++
        Routine Description:

        Check if current page is wide (> 720.0 pixels)

        Return Value: bool. true if current page is < 720 pixels wide

        --*/
        bool ShouldGoToWideState();

        /*++
        Routine Description:

        Handler for page loading

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void PageRoot_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Handler for page unloading

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void PageRoot_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Handler for window size change

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void Window_SizeChanged(Platform::Object ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^e);

        /*++
        Routine Description:

        Event handler for the hardware and software back request from the system

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void DetailPage_BackRequested(Platform::Object ^sender, Windows::UI::Core::BackRequestedEventArgs ^args);

        /*++
        Event registration tokens. Used to monitor windows size change and hardware back button clicks. 
        --*/
        Windows::Foundation::EventRegistrationToken m_sizeChangedEventRegistrationToken;
        Windows::Foundation::EventRegistrationToken m_backRequestedEventRegistrationToken;

        /*++
        Routine Description:

        Event handler for vehicle image tapping. Select a new image for the vehicle.

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void OnSelectVehicleImage(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
};
}
