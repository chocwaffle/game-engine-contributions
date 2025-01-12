/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
@file      Engine.cpp
@project   TeaEngine
@author(s) Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (primary: 100%)

Contents:
- Engine class implementation
- System management (Window, Physics, Audio, Graphics, etc.)
- Getter functions for various systems
====================================================================================*/

/*                                                                          includes
====================================================================================*/
#include "pch.hpp"

/*                                                              function definitions
====================================================================================*/
namespace btEngine
{
    bool Engine::initialize() {

       
        TEA_INFO("-------------------- Engine Lib ---------------------");

        // Initialize system in order
        if (!mWindow->initialize("GAM 300", "config.json"))
        {
	     TEA_ERROR("Window System failed to initialize");          
        }
        if (!physicsSystem->initialize())
        {
             TEA_ERROR("Physics System fail to initialze");
        }
        if (!audioSystem->initialize())
        {
            TEA_ERROR("Audio System fail to initialze");         
        }

        if (!particleSystem->initialize())
        {
            TEA_ERROR("Particles System fail to initialze");
        }

        if (!graphicsSystem->initialize())
        {
            TEA_ERROR("Graphics System fail to initialze");
        }

	if (!mScriptCore->initialize())
        {
	    TEA_ERROR("Scripting System fail to initialze");
	}
    
	TEA_INFO("Engine and its systems initialized succesfully");

	TeaComponents::ComponentManager::registerComponentMap();
        TeaComponents::ComponentManager::printRegisteredComponents();
        return true;
    }
   
    void Engine::shutdown(entt::registry* registry) {	
	// Terminate system in reverse order relative to initializations
        graphicsSystem->shutdown(registry);
        particleSystem->shutdown(registry);
        audioSystem->shutdown(registry);
	physicsSystem->shutdown(registry);
        mScriptRuntime->shutdown(registry);
	mScriptCore->shutdown(registry);
    }
}
