#pragma once
#include "../utilities/Core.h"

struct AudioDevice
{
    AudioDeviceID mID;
    bool mIsInput;
    UInt32 mSafetyOffset;
    UInt32 mBufferSizeFrames;
    AudioStreamBasicDescription mFormat;

    AudioDevice() : mID(kAudioDeviceUnknown), 
                    mIsInput(false), 
                    mSafetyOffset(0), 
                    mBufferSizeFrames(0)
    {} 

    AudioDevice(AudioDeviceID devid, bool isInput) 
    {
        init(devid, isInput); 
    }

    void init(AudioDeviceID devid, bool isInput)
    {
        mID = devid;
        mIsInput = isInput;

        if (mID == kAudioDeviceUnknown) 
            return;

        UInt32 propSize = sizeof(Float32);

        AudioObjectPropertyAddress pA =
        {
            kAudioDevicePropertySafetyOffset,
            mIsInput ? kAudioDevicePropertyScopeInput 
                     : kAudioDevicePropertyScopeOutput,
            0
        };

        AudioObjectGetPropertyData(mID, &pA, 0, NULL, &propSize, &mSafetyOffset);
        propSize = sizeof(UInt32);
        pA.mSelector = kAudioDevicePropertyBufferFrameSize;
        AudioObjectGetPropertyData(mID, &pA, 0, NULL, &propSize, &mBufferSizeFrames);

        propSize = sizeof(AudioStreamBasicDescription);
        pA.mSelector = kAudioDevicePropertyStreamFormat;
        AudioObjectGetPropertyData(mID, &pA, 0, NULL, &propSize, &mFormat);
    }

    bool isValid() const
    {
        return mID != kAudioDeviceUnknown;
    }
    
    void setBufferSize(UInt32 size)
    {
        UInt32 propSize = sizeof(UInt32);

        AudioObjectPropertyAddress pA =
        {
            kAudioDevicePropertyBufferFrameSize,
            mIsInput ? kAudioDevicePropertyScopeInput 
                     : kAudioDevicePropertyScopeOutput,
            0
        };

        AudioObjectSetPropertyData(mID, &pA, 0, NULL, propSize, &size);
        AudioObjectGetPropertyData(mID, &pA, 0, NULL, &propSize, &mBufferSizeFrames);
    }

    int getChannelCount()
    {
        OSStatus err;
        UInt32 propSize;
        int result = 0;

        AudioObjectPropertyAddress pA = 
        {
            kAudioDevicePropertyStreamConfiguration,
            mIsInput ? kAudioDevicePropertyScopeInput 
                     : kAudioDevicePropertyScopeOutput,
            0
        };

        err = AudioObjectGetPropertyDataSize(mID, &pA, 0, NULL, &propSize);
        if (err) 
            return 0;

        auto bufList = std::make_unique<char[]>(propSize);
        AudioBufferList *buflist = reinterpret_cast<AudioBufferList *>(bufList.get());

        err = AudioObjectGetPropertyData(mID, &pA, 0, NULL, &propSize, buflist);
        if (!err)
        {
            for (UInt32 i = 0; i < buflist->mNumberBuffers; ++i) 
            {
                result += buflist->mBuffers[i].mNumberChannels;
            }
        }

        return result;
    }

    std::string getName()
    {
        char buf[256];
        UInt32 maxlen = sizeof(buf);

        AudioObjectPropertyAddress pA = 
        {
            kAudioDevicePropertyDeviceName,
            mIsInput ? kAudioDevicePropertyScopeInput 
                     : kAudioDevicePropertyScopeOutput,
            0
        };

        AudioObjectGetPropertyData(mID, &pA, 0, NULL, &maxlen, buf);
        return std::string(buf);
    }
};