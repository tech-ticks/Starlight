#pragma once

#include <nn/fs.h>
#include <hyperbeam/core.h>
#include <hyperbeam/logging.h>

namespace hb::fs {
    static const char* mountName = "hyperbeam_romfs";
    static void* romFsCache = nullptr;

    bool mountRomFs();
    bool unmountRomFs();
    char* readEntireFile(const char* path);

    bool mountRomFs() {
        u64 cacheSize;

        Result result = nn::fs::QueryMountRomCacheSize(&cacheSize);
        if (result != 0) {
            LOGF("Couldn't query rom cache size (result code %d)\n", result);
            return false;
        }
        romFsCache = operator new(cacheSize);

        result = nn::fs::MountRom(mountName, romFsCache, cacheSize);
        if (result != 0) {
            LOGF("Couldn't mount rom (result code %d)\n", result);
            return false;
        }

        return true;
    }

    bool unmountRomFs() {
        if (!romFsCache) {
            LOG("Unmounting failed, Rom not mounted.\n");
            return false;
        }

        Result result = nn::fs::Unmount(mountName);
        if (result != 0) {
            LOGF("Couldn't unmount romfs (result code %d)\n", result);
            return false;
        }
        
        operator delete(romFsCache);
        return true;
    }
  
    char* readEntireFile(const char* path) {
        if (!romFsCache) {
            LOG("Rom not mounted, call mountRomFs() first.");
            return nullptr;
        }

        nn::fs::FileHandle fileHandle;
        Result result = nn::fs::OpenFile(&fileHandle, path, nn::fs::OpenMode_Read);
        if (result != 0) {
            LOGF("Couldn't open file (result code %d)\n", result);
            return nullptr;
        }

        s64 fileSize;
        result = nn::fs::GetFileSize(&fileSize, fileHandle);
        if (result != 0) {
            LOGF("Couldn't get file size (result code %d)\n", result);
            return nullptr;
        }

        void* buffer = operator new(fileSize);
        result = nn::fs::ReadFile(fileHandle, 0, buffer, fileSize);
        if (result != 0) {
            LOGF("Couldn't read file (result code %d)\n", result);
            return nullptr;
        }

        nn::fs::CloseFile(fileHandle);
        return (char*) buffer;
    }

}
