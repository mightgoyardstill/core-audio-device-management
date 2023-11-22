#pragma once
#include "AudioDevice.h"

class AudioDeviceList
{
public:
    struct Device { std::string mName;  AudioDeviceID mID; };
    using DeviceList = std::vector<Device>;

    AudioDeviceList() { updateDeviceList(); }
    ~AudioDeviceList() { eraseList();}

    Device getDefaultInputDevice() const { return getDefaultDevice(true); }
    Device getDefaultOutputDevice() const { return getDefaultDevice(false); }

    const DeviceList& getList() const { return mAllDevices; }
    const DeviceList& getInputList() const { return mInputDevices; }
    const DeviceList& getOutputList() const { return mOutputDevices; }

    void updateDeviceList() { buildList(); }

private:
    DeviceList mAllDevices;
    DeviceList mInputDevices;
    DeviceList mOutputDevices;

    Device getDefaultDevice(bool isInput) const
    {
        AudioDeviceID defaultId;
        UInt32 propSize = sizeof(AudioDeviceID);

        AudioObjectPropertyAddress pA = 
        {
            isInput ? kAudioHardwarePropertyDefaultInputDevice 
                    : kAudioHardwarePropertyDefaultOutputDevice,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };

        AudioObjectGetPropertyData(kAudioObjectSystemObject, &pA, 0, NULL, &propSize, &defaultId);

        // Choose the appropriate device list based on the isInput flag
        const DeviceList& relevantList = isInput ? mInputDevices : mOutputDevices;

        // Search for the default device in the appropriate list
        for (const auto& device : relevantList)
            if (device.mID == defaultId)
                return device;
        
        // Not found in our list (this should not normally happen)
        return Device{"", 0};
    }

    void buildList()
    {
        eraseList();
        
        UInt32 propSize;
        AudioObjectPropertyAddress pA =
        {
            kAudioHardwarePropertyDevices, 
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };
        
        AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &pA, 0, NULL, &propSize);
        int nDevices = propSize / sizeof(AudioDeviceID);
        auto devIds = std::make_unique<AudioDeviceID[]>(nDevices);
        AudioObjectGetPropertyData(kAudioObjectSystemObject, &pA, 0, NULL, &propSize, devIds.get());
            
        for (int i = 0; i < nDevices; ++i)
        {
            // Check for input devices
            AudioDevice inputDev(devIds[i], true);
            if (inputDev.getChannelCount() > 0)
            {
                Device d;
                d.mID = devIds[i];
                d.mName = inputDev.getName();
                mInputDevices.push_back(d);
            }

            // Check for output devices
            AudioDevice outputDev(devIds[i], false);
            if (outputDev.getChannelCount() > 0)
            {
                Device d;
                d.mID = devIds[i];
                d.mName = outputDev.getName();
                mOutputDevices.push_back(d);
            }

            // Add the device to mAllDevices if it has either input or output channels
            if (inputDev.getChannelCount() > 0 || outputDev.getChannelCount() > 0)
            {
                Device d;
                d.mID = devIds[i];
                d.mName = inputDev.getName();  // or output Dev.getName(), they should be the same
                mAllDevices.push_back(d);
            }
        }
    }

    void eraseList() 
    {
        mAllDevices.clear();
        mInputDevices.clear();
        mOutputDevices.clear();
    }
};
