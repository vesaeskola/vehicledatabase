﻿/*++

Module Name:

NewServicePage.xaml.h

Abstract:

This module declare NewVehiclePage class. NewVehiclePage implement user interface to enter and edit vehicle information.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

*/

#pragma once

#include "NewVehiclePage.g.h"

namespace MasterDetailApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class NewVehiclePage sealed
    {
    public:
        /*++
        Routine Description:

        Constructor

        --*/
        NewVehiclePage();

        /*++
        Routine Description:

        Event handler for the DatabaseEngine object. Each row found from VECHILES table cause this event
        handler triggered.

        Arguments:

        vehicle - Vehicle info, one row of data from VECHILES table.
        Return Value: None.
        --*/
        void VehicleInfoEventHandler(VehicleInfo^ vehicle);

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
    private:

        /*++
        Routine Description:

        Event handler for menu panel. Navigate back to Wellcome page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnBack_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler of the Done button. Navigate back.to previous page.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnDone_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        /*++
        Routine Description:

        Event handler for the hardware and software back request from the system

        Arguments:

        e - Event arguments

        --*/
        void NewVehiclePage_BackRequested(Platform::Object ^sender, Windows::UI::Core::BackRequestedEventArgs ^args);

        /*++
        Routine Description:

        Populate data from UI to VehicleInfo object

        Return Value: VehicleInfo^. VehicleInfo object contain data from UI.

        --*/
        VehicleInfo^ PopulateVehicleInfoObject();

        int mVehicleID;

        /*++
        Event registration tokens.
        --*/
        Windows::Foundation::EventRegistrationToken m_backRequestedEventRegistrationToken;

    };
}
