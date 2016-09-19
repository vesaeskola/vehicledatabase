/*++

Module Name:

EventListPage.xaml.h

Abstract:

This module declare EventListPage class. EventListPage implement view with ListView control to show
vehicle actions like services, fuelings and other events.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola.

--*/

#pragma once

#include "EventListPage.g.h"

namespace MasterDetailApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class EventListPage sealed
    {
    public:
        EventListPage();

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
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) override;

    private:
        /*++
        Routine Description:

        Called when list view grid layout root load. Set the selected item of list view

        Arguments:

        e - Event data that can be examined by overriding code

        --*/
        void LayoutRoot_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

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

        Event handler for the list view item click. Navigate and open in edit mode NewEvent, NewFueling or NewService page to allow
        user edit the action item.

        Arguments:

        sender - sender object of this event
        e - Event data that can be examined by overriding code

        --*/
        void EventListView_EventItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);

        /*++
        Routine Description:

        Event handler back button. Navigate to previous page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnBack_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler new button. Navigate to enter new action page

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnNew_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for the hardware and software back request from the system

        Arguments:

        e - Event arguments

        --*/
        void EventListPage_BackRequested(Platform::Object ^sender, Windows::UI::Core::BackRequestedEventArgs ^args);

        /*++
        Event registration tokens.
        --*/
        Windows::Foundation::EventRegistrationToken m_backRequestedEventRegistrationToken;

        /*++
        Page arguments entered while navigated to this page.
        --*/
        PageNavigateArgs^ mPageArguments;

    };
}
