/*++

Module Name:

App.xaml.cpp

Abstract:

Implementation of the App class.

Environment:

Universal Windows Platform application

Copyright © 2016 Vesa Eskola. All rights reserved.

--*/


#include "pch.h"
//#include "MasterDetailPage.xaml.h"

using namespace MasterDetailApp;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Application template is documented at http://go.microsoft.com/fwlink/?LinkId=402347&clcid=0x409

EnginePool^ gEnginePool;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);

    DetailPage::RegisterDependencyProperties();
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        // Create engine pool here
        if (gEnginePool == nullptr)
        {
            gEnginePool = ref new MasterDetailApp::EnginePool;
        }

        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        rootFrame = ref new Frame();

        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        if (rootFrame->Content == nullptr)
        {
            // If necesary create and open database
            DatabaseEngine^ database = dynamic_cast<DatabaseEngine^>(gEnginePool->GetEngineObject(L"DatabaseEngine"));

            if (database == nullptr)
            {
                auto e = ref new Exception(-1, "Fatal SW error. Database engine object is not instantiated.");
                throw e;
            }

            int vehicleCount = database->ReadShortVehicleInfo();
            if (vehicleCount)
            {
                PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsAppLaunched, nullptr);
                rootFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(MasterDetailPage::typeid), args);
            }
            else
            {
                PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsAppLaunched, nullptr);
                rootFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(WellcomePage::typeid), args);
            }
        }
        // Place the frame in the current Window
        Window::Current->Content = rootFrame;
        // Ensure the current window is active
        Window::Current->Activate();
    }
    else
    {
        if (rootFrame->Content == nullptr)
        {
            // When the navigation stack isn't restored navigate to the first page,
            // configuring the new page by passing required information as a navigation
            // parameter
            PageNavigateArgs^ args = ref new PageNavigateArgs(-1, PageArgs::PageArgsAppLaunched, nullptr);
            rootFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(MasterDetailPage::typeid), args);
        }
        // Ensure the current window is active
        Window::Current->Activate();
    }
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    (void) sender;  // Unused parameter
    (void) e;   // Unused parameter

    //TODO: Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    ::Windows::UI::Xaml::Application::Exit();
}
