#include "AudioSystem.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "Math.h"
#include <filesystem>

SoundHandle SoundHandle::Invalid;

// Create the AudioSystem with specified number of channels
// (Defaults to 8 channels)
AudioSystem::AudioSystem(int numChannels)
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(numChannels);
    mChannels.resize(numChannels);
    mLastHandle++;
}

// Destroy the AudioSystem
AudioSystem::~AudioSystem()
{
    for (auto& sound : mSounds)
    {
        Mix_FreeChunk(sound.second);
    }
    mSounds.clear();

    Mix_CloseAudio();
}

// Updates the status of all the active sounds every frame
void AudioSystem::Update(float deltaTime)
{
    for(int i = 0; i < mChannels.size(); i++)
    {
        if(mChannels[i].IsValid())
        {
            if(!Mix_Playing(i)) {
                mHandleMap.erase(mChannels[i]);
                mChannels[i].Reset();
            }
        }
    }

    for(auto it = mDuckInfos.begin(); it != mDuckInfos.end();)
    {
        it->mTimer -= deltaTime;
        const bool timerExpired = it->mTimer <= 0.0f;
        auto handleIter = mHandleMap.find(it->mHandle);
        const bool handleValid = handleIter != mHandleMap.end();

        if(timerExpired || !handleValid)
        {
            if(handleValid)
            {
                int channel = handleIter->second.mChannel;
                Mix_Volume(channel, it->mOriginalVolume);
            }
            it = mDuckInfos.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// Plays the sound with the specified name and loops if looping is true
// Returns the SoundHandle which is used to perform any other actions on the
// sound when active
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping)
{
    Mix_Chunk* sound = GetSound(soundName);
    if (!sound)
    {
        SDL_Log("[AudioSystem] PlaySound couldn't find sound for %s", soundName.c_str());
        return SoundHandle::Invalid;
    }

    // Find an empty channel
    int channel = -1;
    for (int i = 0; i < mChannels.size(); i++)
    {
        if (!mChannels[i].IsValid())
        {
            channel = i;
            break;
        }
    }

    // If thers not, substitue a non loop
    if (channel == -1)
    {
        for (auto& [handle, info] : mHandleMap)
        {
            if (!info.mIsLooping)
            {
                channel = info.mChannel;
                StopSound(handle);
                break;
            }
        }
    }

    int loops = looping ? -1 : 0;
    Mix_PlayChannel(channel, sound, loops);


    // Create new handle and put in the map
    SoundHandle handle = mLastHandle++;
    mChannels[channel] = handle;

    HandleInfo info;
    info.mChannel = channel;
    info.mSoundName = soundName;
    info.mIsLooping = looping;
    info.mIsPaused = false;
    mHandleMap[handle] = info;

    return handle;
}

void AudioSystem::SetSoundVolume(SoundHandle sound, float volume)
{
    auto iter = mHandleMap.find(sound);
    if(iter == mHandleMap.end())
    {
        return;
    }

    int channel = iter->second.mChannel;
    int clamped = static_cast<int>(Math::Clamp(volume, 0.0f, 1.0f) * MIX_MAX_VOLUME);
    Mix_Volume(channel, clamped);
}

void AudioSystem::DuckSound(SoundHandle sound, float targetVolume, float durationSeconds)
{
    if(durationSeconds <= 0.0f)
    {
        return;
    }

    auto iter = mHandleMap.find(sound);
    if(iter == mHandleMap.end())
    {
        return;
    }

    int channel = iter->second.mChannel;
    int currentVolume = Mix_Volume(channel, -1);
    int target = static_cast<int>(Math::Clamp(targetVolume, 0.0f, 1.0f) * MIX_MAX_VOLUME);
    Mix_Volume(channel, target);

    DuckInfo* foundInfo = nullptr;
    for(auto& info : mDuckInfos)
    {
        if(info.mHandle == sound)
        {
            foundInfo = &info;
            break;
        }
    }

    if(foundInfo)
    {
        foundInfo->mOriginalVolume = currentVolume;
        foundInfo->mTimer = durationSeconds;
    }
    else
    {
        DuckInfo info;
        info.mHandle = sound;
        info.mOriginalVolume = currentVolume;
        info.mTimer = durationSeconds;
        mDuckInfos.emplace_back(info);
    }
}

// Stops the sound if it is currently playing
void AudioSystem::StopSound(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        SDL_Log("[AudioSystem] StopSound couldn't find handle %s", sound.GetDebugStr());
        return;
    }

    // Crash se deixar invertido
    Mix_HaltChannel(mHandleMap[sound].mChannel);
    mChannels[mHandleMap[sound].mChannel].Reset();
    mHandleMap.erase(sound);
}

// Pauses the sound if it is currently playing
void AudioSystem::PauseSound(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        SDL_Log("[AudioSystem] PauseSound couldn't find handle %s", sound.GetDebugStr());
        return;
    }

    if(!mHandleMap[sound].mIsPaused)
    {
        Mix_Pause(mHandleMap[sound].mChannel);
        mHandleMap[sound].mIsPaused = true;
    }
}

// Resumes the sound if it is currently paused
void AudioSystem::ResumeSound(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        SDL_Log("[AudioSystem] ResumeSound couldn't find handle %s", sound.GetDebugStr());
        return;
    }

    if(mHandleMap[sound].mIsPaused)
    {
        Mix_Resume(mHandleMap[sound].mChannel);
        mHandleMap[sound].mIsPaused = false;
    }
}

// Returns the current state of the sound
SoundState AudioSystem::GetSoundState(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        return SoundState::Stopped;
    }

    if(mHandleMap[sound].mIsPaused)
    {
        return SoundState::Paused;
    }

    return SoundState::Playing;
}

// Stops all sounds on all channels
void AudioSystem::StopAllSounds()
{
    Mix_HaltChannel(-1);

    for(auto & mChannel : mChannels)
    {
        mChannel.Reset();
    }

    mHandleMap.clear();
}

// Cache all sounds under Assets/Sounds
void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
    std::error_code ec{};
    for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec})
    {
        std::string extension = rootDirEntry.path().extension().string();
        if (extension == ".ogg" || extension == ".wav")
        {
            std::string fileName = rootDirEntry.path().stem().string();
            fileName += extension;
            CacheSound(fileName);
        }
    }
#endif
}

// Used to preload the sound data of a sound
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
void AudioSystem::CacheSound(const std::string& soundName)
{
    GetSound(soundName);
}

// If the sound is already loaded, returns Mix_Chunk from the map.
// Otherwise, will attempt to load the file and save it in the map.
// Returns nullptr if sound is not found.
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
Mix_Chunk* AudioSystem::GetSound(const std::string& soundName)
{
    std::string fileName = "../Assets/Sounds/";
    fileName += soundName;

    Mix_Chunk* chunk = nullptr;
    auto iter = mSounds.find(fileName);
    if (iter != mSounds.end())
    {
        chunk = iter->second;
    }
    else
    {
        chunk = Mix_LoadWAV(fileName.c_str());
        if (!chunk)
        {
            SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
            return nullptr;
        }

        mSounds.emplace(fileName, chunk);
    }
    return chunk;
}

// Input for debugging purposes
void AudioSystem::ProcessInput(const Uint8* keyState)
{
    // Debugging code that outputs all active sounds on leading edge of period key
    if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
    {
        SDL_Log("[AudioSystem] Active Sounds:");
        for (size_t i = 0; i < mChannels.size(); i++)
        {
            if (mChannels[i].IsValid())
            {
                auto iter = mHandleMap.find(mChannels[i]);
                if (iter != mHandleMap.end())
                {
                    HandleInfo& hi = iter->second;
                    SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
                            static_cast<unsigned>(i), mChannels[i].GetDebugStr(),
                            hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
                }
                else
                {
                    SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i),
                            mChannels[i].GetDebugStr());
                }
            }
        }
    }

    mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}
