#include "plugin.h"
#include "CRenderer.h"
#include "CBaseModelInfo.h"
#include "CModelInfo.h"
#include <iostream>

using namespace plugin;

bool bInitialised;
std::vector<int> modelsCullModeList;

class BackfaceCull {
public:
    static void SetCullMode(int index) {
        if (modelsCullModeList.capacity() > index)
            RwRenderStateSet(rwRENDERSTATECULLMODE, reinterpret_cast<void*>(modelsCullModeList[index]));
    }

    static int GetIndexFromModelName(const char* name) {
        int index = -1;
        CBaseModelInfo* info = CModelInfo::GetModelInfo(name, &index);
        return index;
    }

    static RwCullMode GetCullModeFromName(const char* name) {
        RwCullMode mode = rwCULLMODECULLNONE;
        if (!strcmp(name, "BACK")) {
            mode = rwCULLMODECULLBACK;
        }
        else if (!strcmp(name, "FRONT")) {
            mode = rwCULLMODECULLFRONT;
        }

        return mode;
    }

    BackfaceCull() {
#ifdef DEBUG
        AllocConsole();
        freopen("conin$", "r", stdin);
        freopen("conout$", "w", stdout);
        freopen("conout$", "w", stderr);
        std::setvbuf(stdout, NULL, _IONBF, 0);

#endif
        bInitialised = false;

        Events::initGameEvent.after += [] {
            if (!bInitialised) {
#ifdef GTA3
                std::ifstream file(PLUGIN_PATH("BackfaceCullingIII.dat"));
#else
                std::ifstream file(PLUGIN_PATH("BackfaceCullingVC.dat"));
#endif
                if (file.is_open()) {
                    for (std::string line; getline(file, line);) {
                        int index = -1;
                        char name[32];
                        char modeLit[8];

                        if (!line[0] || line[0] == '\t' || line[0] == ' ' || line[0] == '#' || line[0] == '[' || line[0] == ';')
                            continue;

                        if (!strncmp(line.c_str(), "LIST", 4)) {
                            sscanf(line.c_str(), "%[^, ], %d, %[^, ]", &name, &index, &modeLit);
                            modelsCullModeList.resize(index, GetCullModeFromName(modeLit));
                            continue;
                        }

                        if (line[0] >= '0' && line[0] <= '9')
                            sscanf(line.c_str(), "%d, %s", &index, &modeLit);
                        else {
                            sscanf(line.c_str(), "%[^, ], %s", &name, &modeLit);
                            index = GetIndexFromModelName(name);
                        }

                        if (index != -1 && modelsCullModeList.capacity() > index) {
                            modelsCullModeList[index] = GetCullModeFromName(modeLit);
                        }
                    }
                    file.close();
                }
                bInitialised = true;
            }
        };

#ifdef GTA3
        CdeclEvent <AddressList<0x4A78EF, H_CALL, 0x4A7A7B, H_CALL>, PRIORITY_BEFORE, ArgPickN<CEntity*, 0>, void(CEntity*)> onRenderOnRoadNonRoad;
#endif
        onRenderOnRoadNonRoad.before += [](CEntity* e) {
            SetCullMode(e->m_nModelIndex);
        };
    }
} backfaceCull;
