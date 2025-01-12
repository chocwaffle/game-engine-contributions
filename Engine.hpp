/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
@file      Engine.cpp
@project   TeaEngine
@author(s) Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu>

Contents:
- Engine class definition
- System management (Window, Physics, Audio, Graphics, etc.)
- Getter functions for various systems
====================================================================================*/
#pragma once

/*                                                                          includes
====================================================================================*/
#include "../Core/Window.hpp"
#include "../Core/Systems.hpp"
#include "../Physics/Physics.hpp"
#include "../Core/Audio.hpp"
#include "../Core/Scenemanager.hpp"
#include "../Graphics/System.hpp"
#include "../Particles/ParticleSystem.hpp"
#include "../Core/profiler.hpp"
#include "../Core/Font.hpp"
#include "Scripting/Scripting.hpp"
#include "Scripting/ScriptRuntime.hpp"
#include "Components/ComponentManager.hpp"
#include "../Core/AnimationTransform.hpp"

/*                                                             function declarations
====================================================================================*/
namespace btEngine
{
    class Engine
    {
    public:
		Engine() = default;
        bool initialize();
        void shutdown(entt::registry* registry);

        window::windowSystem&                   getWindow()             const { return *mWindow;        }
		TeaPhysics::physicsSystem&              getPhysics()            const { return *physicsSystem;  }
		audio::audioSystem&                     getAudio()              const { return *audioSystem;    }
        TeaGraphics::graphicsSystem&            getGraphics()           const { return *graphicsSystem; }
        TeaParticles::particleSystem&           getParticles()          const { return *particleSystem; }
		profiler::ProfilerSystem&               getProfiler()           const { return *mProfiler;      }
        SceneManager::sceneManager&             getSceneManager()             { return *sceneManager;   }
		Scripting::ScriptCore&                  getScriptCore()         const { return *mScriptCore;    }
		Scripting::ScriptRuntime&               getScriptRuntime()      const { return *mScriptRuntime; }
		TeaAnimation::Animator&                 getAnimator()           const { return *mAnimator;      }

    private:

        std::unique_ptr<window::windowSystem>                   mWindow                 = std::make_unique<window::windowSystem>();
        std::unique_ptr<TeaPhysics::physicsSystem>              physicsSystem           = std::make_unique<TeaPhysics::physicsSystem>();
        std::unique_ptr<audio::audioSystem>                     audioSystem             = std::make_unique<audio::audioSystem>();
		std::unique_ptr<SceneManager::sceneManager>             sceneManager            = std::make_unique<SceneManager::sceneManager>();
        std::unique_ptr<TeaParticles::particleSystem>           particleSystem          = std::make_unique<TeaParticles::particleSystem>();
        std::unique_ptr<TeaGraphics::graphicsSystem>            graphicsSystem          = std::make_unique<TeaGraphics::graphicsSystem>();
		std::unique_ptr<profiler::ProfilerSystem>               mProfiler               = std::make_unique<profiler::ProfilerSystem>();
		std::unique_ptr<Scripting::ScriptCore>                  mScriptCore             = std::make_unique<Scripting::ScriptCore>();
		std::unique_ptr<Scripting::ScriptRuntime>               mScriptRuntime          = std::make_unique<Scripting::ScriptRuntime>();
		std::unique_ptr<TeaAnimation::Animator>                 mAnimator               = std::make_unique<TeaAnimation::Animator>();
    };
}