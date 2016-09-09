/*++

Module Name:

WellcomePage.xaml.h

Abstract:

This module declare WellcomePage class. WellcomePage show wellcome note for the user and is shown only if there is not entered yet any vehicles into database.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

*/

#pragma once

#include "WellcomePage.g.h"

namespace MasterDetailApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class WellcomePage sealed
    {
    public:
        /*++
        Routine Description:

        Constructor

        --*/
        WellcomePage();

    protected:
        /*++
        Routine Description:

        Invoked when the Page is loaded and becomes the current source of a parent Frame.

        Arguments:

        e - Event data that can be examined by overriding code

        --*/
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        /*++
        Routine Description:

        Button event handler. Navigate to NewVehiclePage.

        Arguments:

        sender - sender object of the event
        e - Event arguments

        --*/
        void OnGoNewVehiclePage_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	};
}
