/** 
 * Edited global finding from AddonSpawner
 * Credits
 * Find record global: zorg93 & Unknown Modder
 */

#include "menumemutils.hpp"
#include "inc/main.h"
#include "inc/types.h"
#include <Windows.h>
#include <Psapi.h>
#include <array>
#include <string>

namespace NativeMenu {
    uint16_t gRecordGlobal = 0;

    struct ScriptHeader;
    namespace MemoryAccess {
        void Init();
        uintptr_t FindPattern(const char* pattern, const char* mask, const char* startAddress, size_t size);
        uintptr_t FindPattern(const char* pattern, const char* mask);
        uint16_t findRecordGlobal(ScriptHeader* script);
        bool findScript(const std::string& scriptName, ScriptHeader** script);
    }

#pragma pack(push, 1)
    struct ScriptHeader {
        char padding1[16];                  //0x0
        unsigned char** codeBlocksOffset;   //0x10
        char padding2[4];                   //0x18
        int codeLength;                     //0x1C
        char padding3[4];                   //0x20
        int localCount;                     //0x24
        char padding4[4];                   //0x28
        int nativeCount;                    //0x2C
        __int64* localOffset;               //0x30
        char padding5[8];                   //0x38
        __int64* nativeOffset;              //0x40
        char padding6[16];                  //0x48
        int nameHash;                       //0x58
        char padding7[4];                   //0x5C
        char* name;                         //0x60
        char** stringsOffset;               //0x68
        int stringSize;                     //0x70
        char padding8[12];                  //0x74
                                            //END_OF_HEADER

        bool IsValid() const { return codeLength > 0; }
        int CodePageCount() const { return (codeLength + 0x3FFF) >> 14; }
        int GetCodePageSize(int page) const {
            return (page < 0 || page >= CodePageCount() ? 0 : (page == CodePageCount() - 1) ? codeLength & 0x3FFF : 0x4000);
        }
        unsigned char* GetCodePageAddress(int page) const { return codeBlocksOffset[page]; }
        unsigned char* GetCodePositionAddress(int codePosition) const {
            return codePosition < 0 || codePosition >= codeLength ? NULL : &codeBlocksOffset[codePosition >> 14][codePosition & 0x3FFF];
        }
        char* GetString(int stringPosition)const {
            return stringPosition < 0 || stringPosition >= stringSize ? NULL : &stringsOffset[stringPosition >> 14][stringPosition & 0x3FFF];
        }

    };
#pragma pack(pop)

    struct ScriptTableItem {
        ScriptHeader* Header;
        char padding[4];
        int hash;

        inline bool IsLoaded() const {
            return Header != NULL;
        }
    };

    struct ScriptTable {
        ScriptTableItem* TablePtr;
        char padding[16];
        int count;
        ScriptTableItem* FindScript(int hash) {
            if (TablePtr == NULL) {
                return NULL;//table initialisation hasnt happened yet
            }
            for (int i = 0; i < count; i++) {
                if (TablePtr[i].hash == hash) {
                    return &TablePtr[i];
                }
            }
            return NULL;
        }
    };

    struct GlobalTable {
        __int64** GlobalBasePtr;
        __int64* AddressOf(int index) const { return &GlobalBasePtr[index >> 18 & 0x3F][index & 0x3FFFF]; }
        bool IsInitialised()const { return *GlobalBasePtr != NULL; }
    };

    Hash joaat(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);

        Hash hash = 0;
        for (int i = 0; i < s.size(); i++) {
            hash += s[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
    }

    void MemoryAccess::Init() {
        ScriptHeader* selector = nullptr;

        // find enable MP cars patterns
        if (findScript("selector", &selector))
            gRecordGlobal = findRecordGlobal(selector);
    }

    uintptr_t MemoryAccess::FindPattern(const char* pattern, const char* mask, const char* startAddress, size_t size) {
        const char* address_end = startAddress + size;
        const auto mask_length = static_cast<size_t>(strlen(mask) - 1);

        for (size_t i = 0; startAddress < address_end; startAddress++) {
            if (*startAddress == pattern[i] || mask[i] == '?') {
                if (mask[i + 1] == '\0') {
                    return reinterpret_cast<uintptr_t>(startAddress) - mask_length;
                }
                i++;
            }
            else {
                i = 0;
            }
        }
        return 0;
    }

    uintptr_t MemoryAccess::FindPattern(const char* pattern, const char* mask) {
        MODULEINFO modInfo = { };
        GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &modInfo, sizeof(MODULEINFO));

        return FindPattern(pattern, mask, reinterpret_cast<const char*>(modInfo.lpBaseOfDll), modInfo.SizeOfImage);
    }

    GlobalTable globalTable;
    ScriptTable* scriptTable;

    // from EnableMPCars by drp4lyf
    bool MemoryAccess::findScript(const std::string & scriptName, ScriptHeader** script) {

        __int64 patternAddr = FindPattern("\x4C\x8D\x05\x00\x00\x00\x00\x4D\x8B\x08\x4D\x85\xC9\x74\x11", "xxx????xxxxxxxx");
        if (!patternAddr) {
            //logger.Write(ERROR, "ERROR: finding address 0");
            //logger.Write(ERROR, "Aborting...");
            return false;
        }
        globalTable.GlobalBasePtr = (__int64**)(patternAddr + *(int*)(patternAddr + 3) + 7);


        patternAddr = FindPattern("\x48\x03\x15\x00\x00\x00\x00\x4C\x23\xC2\x49\x8B\x08", "xxx????xxxxxx");
        if (!patternAddr) {
            //logger.Write(ERROR, "ERROR: finding address 1");
            //logger.Write(ERROR, "Aborting...");
            return false;
        }
        scriptTable = (ScriptTable*)(patternAddr + *(int*)(patternAddr + 3) + 7);

        DWORD startTime = GetTickCount();
        DWORD timeout = 10000; // in millis

        // FindScriptAddresses
        while (!globalTable.IsInitialised()) {
            scriptWait(100); //Wait for GlobalInitialisation before continuing
            if (GetTickCount() > startTime + timeout) {
                //logger.Write(ERROR, "ERROR: couldn't init global table");
                //logger.Write(ERROR, "Aborting...");
                return false;
            }
        }

        //logger.Write(INFO, "Found global base pointer 0x%lX", globalTable.GlobalBasePtr);

        ScriptTableItem* Item = scriptTable->FindScript(joaat(scriptName));
        if (Item == NULL) {
            //logger.Write(ERROR, "ERROR: finding address 2");
            //logger.Write(ERROR, "Aborting...");
            return false;
        }
        while (!Item->IsLoaded())
            Sleep(100);

        *script = Item->Header;
        //logger.Write(INFO, "Found script %s [0x%X]", scriptName.c_str(), joaat(scriptName));
        return true;
    }

    // Unknown Modder
    uint16_t MemoryAccess::findRecordGlobal(ScriptHeader* script) {
        const char* patt = "\x70\x25\x13\x2C\x09\x00\x00\x06\x2A\x56\x09\x00\x70\x25\x13\x2C\x09\x00\x00\x06\x1F\x56\x06\x00\x6E\x52\x00\x00\x42\x00\x70";
        const char* mask = "xxxxx??xxxxxxxxxx??xxxxxxx??x?x";

        for (int i = 0; i < script->CodePageCount(); i++)
        {
            int size = script->GetCodePageSize(i);
            if (size)
            {
                uintptr_t address = FindPattern(patt, mask, (const char*)script->GetCodePageAddress(i), size);
                if (address)
                {
                    uint16_t globalId = *(uint16_t*)(address + 26);
                    uint8_t structOff = *(uint8_t*)(address + 29);

                    //logger.Write(INFO, "global id: %u, struct offset: 0x%X\n", globalId, structOff);

                    return globalId + structOff;
                }
            }
        }

        //logger.Write(ERROR, "Global Variable not found");
        return 0;
    }

    uint16_t FindRecordGlobal() {
        if (!gRecordGlobal) {
            //logger.Write(INFO, "Finding Record global...");
            MemoryAccess::Init();
        }
        return gRecordGlobal;
    }
}
