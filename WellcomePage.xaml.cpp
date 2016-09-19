/*++

Module Name:

WellcomePage.xaml.cpp

Abstract:

This module implement WellcomePage class. WellcomePage show wellcome note for the user and is shown only if there is not entered yet any vehicles into database.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

*/
//
// WellcomePage.xaml.cpp
// Implementation of the WellcomePage class
//

#include "pch.h"
#include "WellcomePage.xaml.h"

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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

/*++
Routine Description:

Constructor

--*/
WellcomePage::WellcomePage()
{
    InitializeComponent();
}

/*++
Routine Description:

Invoked when the Page is loaded and becomes the current source of a parent Frame.
TBD: Not needed, eliminate....

Arguments:

e - Event data that can be examined by overriding code

--*/
void WellcomePage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    Page::OnNavigatedTo(e);
}

/*++
Routine Description:

Button event handler. Navigate to NewVehiclePage.

Arguments:

sender - sender object of the event
e - Event arguments

--*/
void WellcomePage::OnGoNewVehiclePage_Click(Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsNone, nullptr);
    Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(NewVehiclePage::typeid), args);
}
