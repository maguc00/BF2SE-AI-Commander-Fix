//config reading
#include <unordered_map>
#include <string>

constexpr int REQUIRED_FRAMEWORK_API_VERSION = 1;

struct FrameworkAPI
{
    int version;
    void (*Log)(const char* msg);
    void (*Patch)(int address, const char* patch, size_t size);
    std::unordered_map<std::string, std::string>(*LoadConfig)(const std::string& path);
};

struct PluginAPI
{
    int required_framework_version = REQUIRED_FRAMEWORK_API_VERSION;
    const char* name = "AI Commander Fix";
    const char* author = "maguc";
    const char* description = "Contains small patches to re-enable AI Commander logic.";
    const char* version = "1.0";
    bool unload_after_init = true;  // !! EXPERIMENTAL !!
                                    // DO NOT create threads, hooks, or timers in this plugin if unload_after_init = true!
                                    // FreeLibrary() is called immediately after PluginInit(), so the DLL must be fully idle (doing nothing).
                                    // It will most likely cause a crash or other unintended behaviours if code is still running!
};

static PluginAPI pluginAPI;

extern "C" __declspec(dllexport)
PluginAPI* PluginInit(FrameworkAPI* api)
{
    char patch[] = { 0x39, 0xC0 }; // eax, eax
    
    auto config = api->LoadConfig("plugins/AI_Commander_Fix.ini");

    if (config["enableUAV"] == "true")
        api->Patch(0x005401BF, patch, sizeof(patch));  // UAV
    if (config["enableSupplyDrop"] == "true")
        api->Patch(0x005423BE, patch, sizeof(patch));  // Supplies
    if (config["enableVehicleDrop"] == "true")
        api->Patch(0x0054024D, patch, sizeof(patch));  // Vehicle drop
    if (config["enableArtillery"] == "true")
        api->Patch(0x005404A8, patch, sizeof(patch));  // Artillery

    return &pluginAPI;
}