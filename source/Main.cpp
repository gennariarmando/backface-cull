#include "plugin.h"
#include "CRenderer.h"
#include <iostream>

using namespace plugin;

bool bInitialised;
std::vector<int> modelsCullModeList(16000, rwCULLMODECULLNONE);

class BackfaceCull {
public:
    static void SetCullMode(int index) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, reinterpret_cast<void*>(modelsCullModeList[index]));
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

        Events::initRwEvent += [] {
            if (!bInitialised) {
#ifdef GTA3
                std::ifstream file(PLUGIN_PATH("BackfaceCullingIII.dat"));
#else
                std::ifstream file(PLUGIN_PATH("BackfaceCullingVC.dat"));
#endif
                if (file.is_open()) {
                    for (std::string line; getline(file, line);) {
                        int index;
                        char modeLit[8];
                        char mode = rwCULLMODECULLNONE;

                        if (!line[0] || line[0] == '\t' || line[0] == ' ' || line[0] == '#' || line[0] == '[' || line[0] == ';')
                            continue;

                        sscanf(line.c_str(), "%d, %s", &index, &modeLit);

                        if (!strcmp(modeLit, "BACK")) {
                            mode = rwCULLMODECULLBACK;
                        }
                        else if (!strcmp(modeLit, "FRONT")) {
                            mode = rwCULLMODECULLFRONT;
                        }

                        modelsCullModeList[index] = mode;
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
