// Credits
// Find record global: zorg93 & Unknown Modder
// Enhanced bits and bobs: Chiheb-Bacha

#include "menumemutils.hpp"
#include <inc/main.h>
#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <chrono>

namespace NativeMenu {

uint16_t gRecordGlobal = 0;

namespace Utils {
inline uint32_t Joaat(const std::string& s) {
    uint32_t hash = 0;
    for (char c : s) {
        hash += std::tolower(static_cast<unsigned char>(c));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

inline bool IsEnhanced() {
    return getGameVersion() >= 1000;
}
}

#pragma pack(push, 1)
struct ScriptHeader {
    char padding1[16];
    unsigned char** codeBlocksOffset;
    char padding2[4];
    int codeLength;
    char padding3[4];
    int localCount;
    char padding4[4];
    int nativeCount;
    int64_t* localOffset;
    char padding5[8];
    int64_t* nativeOffset;
    char padding6[16];
    int nameHash;
    char padding7[4];
    char* name;
    char** stringsOffset;
    int stringSize;

    bool IsValid() const {
        return codeLength > 0;
    }
    int PageCount() const {
        return (codeLength + 0x3FFF) >> 14;
    }
    int GetPageSize(int page) const {
        if (page < 0 || page >= PageCount()) return 0;
        return (page == PageCount() - 1) ? (codeLength & 0x3FFF) : 0x4000;
    }
    unsigned char* GetPageAddress(int page) const {
        return codeBlocksOffset[page];
    }
};

struct ScriptTableItem {
    ScriptHeader* Header;
    char padding[4];
    int hash;
    bool IsLoaded() const {
        return Header != nullptr;
    }
};

struct ScriptTable {
    ScriptTableItem* TablePtr;
    char padding[16];
    int count;

    ScriptTableItem* FindScript(uint32_t hash) {
        if (!TablePtr) return nullptr;
        for (int i = 0; i < count; i++) {
            if (TablePtr[i].hash == (int)hash) return &TablePtr[i];
        }
        return nullptr;
    }
};
#pragma pack(pop)

namespace MemoryAccess {
uintptr_t FindPattern(const std::string& pattern, const uint8_t* startAddress, size_t size) {
    std::vector<int> bytes;
    std::stringstream ss(pattern);
    std::string temp;

    while (ss >> temp) {
        if (temp == "?" || temp == "??") bytes.push_back(-1);
        else bytes.push_back(std::stoi(temp, nullptr, 16));
    }

    for (size_t i = 0; i <= size - bytes.size(); ++i) {
        bool found = true;
        for (size_t j = 0; j < bytes.size(); ++j) {
            if (bytes[j] != -1 && startAddress[i + j] != static_cast<uint8_t>(bytes[j])) {
                found = false;
                break;
            }
        }
        if (found) return reinterpret_cast<uintptr_t>(&startAddress[i]);
    }
    return 0;
}

uintptr_t FindModulePattern(const std::string& pattern) {
    MODULEINFO modInfo = {};
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &modInfo, sizeof(MODULEINFO));
    return FindPattern(pattern, reinterpret_cast<const uint8_t*>(modInfo.lpBaseOfDll), modInfo.SizeOfImage);
}

bool FindScript(const std::string& name, ScriptHeader** script) {
    uintptr_t patternAddr = Utils::IsEnhanced()
        ? FindModulePattern("48 03 05 ? ? ? ? 4c 85 c0 0f 84 ? ? ? ? e9")
        : FindModulePattern("48 03 15 ? ? ? ? 4C 23 C2 49 8B 08");

    if (!patternAddr) return false;

    auto* table = reinterpret_cast<ScriptTable*>(patternAddr + *reinterpret_cast<int*>(patternAddr + 3) + 7);
    uint32_t hash = Utils::Joaat(name);

    // Wait for script to register
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    ScriptTableItem* item = nullptr;

    while (!(item = table->FindScript(hash))) {
        scriptWait(100);
        if (std::chrono::steady_clock::now() > timeout) return false;
    }

    while (!item->IsLoaded()) {
        Sleep(100);
        if (std::chrono::steady_clock::now() > timeout) return false;
    }

    *script = item->Header;
    //LOG(Info, "Found script {} [0x{:X}]", name, hash);
    return true;
}

uint16_t FindRecordGlobal(ScriptHeader* script) {
    // script: selector(.c)
    // PAD::IS_CONTROL_PRESSED(2, 19), aka
    // PAD::IS_CONTROL_PRESSED(FRONTEND_CONTROL, INPUT_CHARACTER_WHEEL)
    const char* patt = "?? 25 13 2C 09 ?? ?? 06 2A 56 09 00 ?? 25 13 2C 09 ?? ?? 06 1F 56 06 00 ?? 52 ?? ?? 42 ?? ??";

    for (int i = 0; i < script->PageCount(); i++) {
        int size = script->GetPageSize(i);
        if (size <= 0) continue;

        uintptr_t addr = FindPattern(patt, script->GetPageAddress(i), size);
        if (addr) {
            uint16_t globalId = *reinterpret_cast<uint16_t*>(addr + 26);
            uint8_t structOff = *reinterpret_cast<uint8_t*>(addr + 29);
            //LOG(Info, "Global Match: ID {}, Offset 0x{:X}", globalId, structOff);
            return globalId + structOff;
        }
    }
    return 0;
}
}

uint16_t GetRecordGlobal() {
    if (gRecordGlobal) return gRecordGlobal;

    ScriptHeader* selector = nullptr;
    if (MemoryAccess::FindScript("selector", &selector)) {
        gRecordGlobal = MemoryAccess::FindRecordGlobal(selector);
    }

    //if (!gRecordGlobal) LOG(Error, "Failed to find Record Global");
    return gRecordGlobal;
}
}
