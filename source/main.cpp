#include <iostream>
#include <vector>
#include <functional>
#include <string>

#include "shared/audio/AudioDeviceList.h"
#include "shared/audio/AudioDeviceListener.h"

/* 
    This is a simple example of how to use the AudioDeviceList and AudioDeviceListener classes.
    The AudioDeviceList class is used to get a list of all the audio devices on the system.
    The AudioDeviceListener class is used to get notifications when the default input or output
    device is changed, or when a device is added or removed from the system.

    I've tried to be conscious of keeping these classes as simple as possible, so that the user
    could for example, create their own custom DeviceManager class that uses these classes to
    manage the audio devices in their application.
*/

int main(int argc, char* argv[])
{
    AudioDeviceListener dListener;
    AudioDeviceList dList;

    for (const auto& device : dList.getList())
    {
        std::cout << "Device: " << device.mName << std::endl;
        std::cout << "ID: " << device.mID << std::endl;
    }

    using Notification = AudioDeviceListener::Notification;

    dListener.onDeviceChange = [&] (Notification dNotification)
    {
        switch(dNotification)
        {
            case Notification::DeviceAdded:
            {
                std::cout << "Device added" << std::endl;
                dList.updateDeviceList();
                break;
            }
            case Notification::DeviceRemoved:
            {
                std::cout << "Device removed" << std::endl;
                dList.updateDeviceList();
                break;
            }
            case Notification::InputChanged:
            {
                std::cout << "Default Input changed to: ";
                std::cout << dList.getDefaultInputDevice().mName << std::endl;
                break;
            }
            case Notification::OutputChanged:
            {
                std::cout << "Default Output changed to: ";
                std::cout << dList.getDefaultOutputDevice().mName << std::endl;
                break;
            }
        }
    };

    std::cin.get();
    return 0;
}