#pragma once

#include <nn/fs.h>
#include <hyperbeam/core.h>
#include <hyperbeam/logging.h>

namespace hb::fs {
    static const char* mountName = "hyperbeam_romfs";
    static void* romFsCache = nullptr;
    static bool romFsMounted = false;

    bool mountRomFs();
    bool unmountRomFs();
    char* readEntireFile(const char* path);

    bool mountRomFs() {
        if (romFsMounted) {
            LOG("Warning: Tried to mount RomFS, but it's already mounted.");
            return true;
        }

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

        romFsMounted = true;

        return true;
    }

    bool unmountRomFs() {
        if (!romFsMounted) {
            LOG("Unmounting failed, Rom not mounted.\n");
            return false;
        }

        Result result = nn::fs::Unmount(mountName);
        if (result != 0) {
            LOGF("Couldn't unmount romfs (result code %d)\n", result);
            return false;
        }
        romFsMounted = false;
        
        operator delete(romFsCache);

        return true;
    }
  
    char* readEntireFile(const char* path) {
        if (!romFsMounted) {
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

    /// Write a file, overwriting an existing one with the same name.
    bool writeFile(const char* path, void* contents, uint64_t size) {
        nn::fs::DirectoryEntryType type;
        if (nn::fs::GetEntryType(&type, path) == 0 && type == nn::fs::DirectoryEntryType_File) {
            auto result = nn::fs::DeleteFile(path);
            if (result != 0) {
                LOGF("Couldn't delete previous file (result code %d)\n", result);
                return false;
            }
        }

        auto result = nn::fs::CreateFile(path, size);
        if (result != 0) {
            LOGF("Couldn't create file (result code %d)\n", result);
            return false;
        }

        nn::fs::FileHandle fileHandle;
        result = nn::fs::OpenFile(&fileHandle, path, nn::fs::OpenMode_Write);
        if (result != 0) {
            LOGF("Couldn't open file (result code %d)\n", result);
            return false;
        }

        nn::fs::WriteOption options = nn::fs::WriteOption::CreateOption(
        nn::fs::WriteOptionFlag::WriteOptionFlag_Flush);
        result = nn::fs::WriteFile(fileHandle, 0, contents, size, options);
        if (result != 0) {
            LOGF("Couldn't write file (result code %d)\n", result);
            return false;
        }
        nn::fs::CloseFile(fileHandle);

        return true;
    }

    bool commitSaveData() {
        return nn::fs::Commit("PEGASUS") == 0;
    }

}
