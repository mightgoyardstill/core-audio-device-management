#pragma once
#include <CoreFoundation/CoreFoundation.h>
#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/CoreAudio.h>
#include <CoreMidi/CoreMidi.h>

namespace corefoundation
{

//==================================================================================
class String
{
public:
    String() = default;
    String(const String&) = delete;
    String& operator=(const String&) = delete;
    ~String() = default; // Destructor logic moved to the deleter of the smart pointer
    
    std::shared_ptr<__CFString> ref { nullptr, [] (CFStringRef ref)
    {
        if (ref) CFRelease(ref);
    }};

    std::string toString() const
    {
        if (!ref) return std::string();

        CFIndex length = CFStringGetLength(ref.get());
        CFIndex max = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);

        auto buffer = std::vector<char>(max);
        if (!CFStringGetCString(ref.get(), buffer.data(), max, kCFStringEncodingUTF8)) {
            return std::string();
        }

        return std::string(buffer.data());
    }
};

} // namespace corefoundation


namespace coreaudio
{

//==================================================================================
template <typename T> // Generic version for simple data types
OSStatus getPropertyData(AudioObjectID deviceId, 
                         AudioObjectPropertySelector selector,
                         AudioObjectPropertyScope scope, 
                         T* data)
{
    AudioObjectPropertyAddress propAddress = {selector, scope, kAudioObjectPropertyElementMaster};
    UInt32 size = static_cast<UInt32>(sizeof(T));
    return AudioObjectGetPropertyData(deviceId, &propAddress, 0, NULL, &size, data);
}

template <typename T> // Overloaded version for complex data types stored in std::vector
OSStatus getPropertyData(AudioObjectID deviceId, 
                         AudioObjectPropertySelector selector,
                         AudioObjectPropertyScope scope, 
                         std::vector<T>& data)
{
    AudioObjectPropertyAddress propAddress = {selector, scope, kAudioObjectPropertyElementMaster};
    // Get the data size
    UInt32 size{};
    AudioObjectGetPropertyDataSize(deviceId, &propAddress, 0, NULL, &size);
    // Resize the vector to fit the data
    data.resize(size / sizeof(T));
    // Get the data
    return AudioObjectGetPropertyData(deviceId, &propAddress, 0, NULL, &size, data.data());
}

// Overload for std::shared_ptr<__CFString>
OSStatus getPropertyData(AudioObjectID deviceId, 
                         AudioObjectPropertySelector selector,
                         AudioObjectPropertyScope scope, 
                         std::shared_ptr<__CFString>& data)
{
    AudioObjectPropertyAddress propAddress = {selector, scope, kAudioObjectPropertyElementMaster};
    UInt32 size = sizeof(CFStringRef);
    return AudioObjectGetPropertyData(deviceId, &propAddress, 0, NULL, &size, &data);
}

//==================================================================================
template <typename T> // Generic version for simple data types
OSStatus setPropertyData(AudioObjectID deviceId, 
                         AudioObjectPropertySelector selector,
                         AudioObjectPropertyScope scope, 
                         const T* data)
{
    AudioObjectPropertyAddress propAddress = {selector, scope, kAudioObjectPropertyElementMaster};
    UInt32 size = static_cast<UInt32>(sizeof(T));
    return AudioObjectSetPropertyData(deviceId, &propAddress, 0, NULL, size, data);
}

//==================================================================================
AudioObjectPropertyAddress createPropertyAddress(AudioObjectPropertySelector selector, 
                                                 AudioObjectPropertyScope scope)
{
    AudioObjectPropertyAddress propAddress;
    propAddress.mSelector = selector;
    propAddress.mScope = scope;
    propAddress.mElement = kAudioObjectPropertyElementMaster;
    return propAddress;
}

} // namespace coreaudio