/*++

Module Name:

App.xaml.h

Abstract:

Declaration of the App class.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

--*/


#pragma once

#include "App.g.h"

namespace MasterDetailApp
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    ref class App sealed
    {
    protected:
        virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

   internal:
        App();

    private:
        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
        void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
    };
}
