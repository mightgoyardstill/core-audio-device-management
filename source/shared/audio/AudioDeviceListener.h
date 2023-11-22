#pragma once
#include "../utilities/Core.h"

class AudioDeviceListener
{
private:
    std::vector<AudioObjectID> mDeviceCount;

public:
    enum Notification { DeviceAdded, DeviceRemoved, InputChanged, OutputChanged };

    AudioDeviceListener()
    {
        updateDeviceCount(); 
        registerForDeviceChangeNotifications(); 
    }

    ~AudioDeviceListener() 
    {
        removeDeviceChangeNotifications(); 
    }
    
    std::function <void (AudioDeviceListener::Notification)> onDeviceChange;

protected:
    void registerForDeviceChangeNotifications()
    {
        auto systemObjectAddress = coreaudio::createPropertyAddress(
            kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal);

        AudioObjectAddPropertyListener(kAudioObjectSystemObject, &systemObjectAddress, 
            devicesUpdatedCallback, this);

        auto defaultInputDeviceAddress = coreaudio::createPropertyAddress(
            kAudioHardwarePropertyDefaultInputDevice, kAudioObjectPropertyScopeGlobal);

        AudioObjectAddPropertyListener(kAudioObjectSystemObject, &defaultInputDeviceAddress, 
            defaultDeviceChangeCallback, this);

        auto defaultOutputDeviceAddress = coreaudio::createPropertyAddress(
            kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal);

        AudioObjectAddPropertyListener(kAudioObjectSystemObject, &defaultOutputDeviceAddress, 
            defaultDeviceChangeCallback, this);
    }

    void removeDeviceChangeNotifications()
    {
        auto systemObjectAddress = coreaudio::createPropertyAddress(
            kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal);

        AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &systemObjectAddress, 
            devicesUpdatedCallback, this);

        auto defaultInputDeviceAddress = coreaudio::createPropertyAddress(
            kAudioHardwarePropertyDefaultInputDevice, kAudioObjectPropertyScopeGlobal);

        AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &defaultInputDeviceAddress, 
            defaultDeviceChangeCallback, this);

        auto defaultOutputDeviceAddress = coreaudio::createPropertyAddress(
            kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal);
            
        AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &defaultOutputDeviceAddress, 
            defaultDeviceChangeCallback, this);
    }

    static OSStatus devicesUpdatedCallback(AudioObjectID id, 
                                           UInt32 numAddresses, 
                                           const AudioObjectPropertyAddress inAddresses[], 
                                           void* clientData)
    {
        auto listener = static_cast<AudioDeviceListener*>(clientData);

        // Store the number of devices before updating the list
        size_t prevDeviceCount = listener->getDeviceCount();

        for (UInt32 i = 0; i < numAddresses; ++i)
        {
            if (inAddresses[i].mSelector == kAudioHardwarePropertyDevices)
            {
                // The list of available devices has changed
                listener->updateDeviceCount();

                // Store the number of devices after updating the list
                size_t newDeviceCount = listener->getDeviceCount();

                if (listener->onDeviceChange)
                {
                    // Check whether a device was added or removed based on the count
                    if (newDeviceCount > prevDeviceCount) 
                        listener->onDeviceChange(AudioDeviceListener::Notification::DeviceAdded);

                    else if (newDeviceCount < prevDeviceCount)
                        listener->onDeviceChange(AudioDeviceListener::Notification::DeviceRemoved);
                }
            }
            // Add more conditions here if you register for more types of changes

        }
        return noErr;
    }

    static OSStatus defaultDeviceChangeCallback(AudioObjectID id, 
                                                UInt32 numAddresses, 
                                                const AudioObjectPropertyAddress inAddresses[], 
                                                void* clientData)
    {
        auto listener = static_cast<AudioDeviceListener*>(clientData);

        if (listener->onDeviceChange)
        {
            for (UInt32 i = 0; i < numAddresses; ++i)
            {
                // Default input device has changed
                if (inAddresses[i].mSelector == kAudioHardwarePropertyDefaultInputDevice)
                    listener->onDeviceChange(AudioDeviceListener::Notification::InputChanged);
                
                // Default output device has changed
                else if (inAddresses[i].mSelector == kAudioHardwarePropertyDefaultOutputDevice)
                    listener->onDeviceChange(AudioDeviceListener::Notification::OutputChanged);
            }
        }
        return noErr;
    }

    void updateDeviceCount()
    {
        // should this be wrapped in a mutex lock guard?
        mDeviceCount.clear();

        coreaudio::getPropertyData(kAudioObjectSystemObject, kAudioHardwarePropertyDevices, 
            kAudioObjectPropertyScopeGlobal, mDeviceCount);
    }

    size_t getDeviceCount() 
    {
        return this->mDeviceCount.size();
    }
};
