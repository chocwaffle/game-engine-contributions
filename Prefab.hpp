/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.

@file       Prefab.hpp
@project    TeaEditor
@author(s)  Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (primary: 100%)

Contents:
- Prefab instance updating functionality
====================================================================================*/
#pragma once

/*                                                                          includes
====================================================================================*/
#include "imgui.h"
#include "../../TeaEngine/Src/Core/logging.hpp"
#include "../Src/Core/engine.hpp"
#include "../Core/filedialog.hpp"
#include "../Src/Core/entity.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Animator.hpp"
#include "../Src/Components/SoundComponent.hpp"
#include "Assetbrowser.hpp"
#include "Graphics/AnimationClip.hpp"
#include "UndoRedo.hpp"

/*                                                             function declarations
====================================================================================*/
namespace TeaEditor
{
    class HierarchyPanel
    {
    public:    
       /**
       * @brief Loads and parses a prefab file from the given path
       *
       * This function handles prefab file operations, including:
       * 1. Opening and reading the prefab file from the specified path
       * 2. Parsing the JSON content using RapidJSON
       * 3. Performing error checking for file operations
       * 4. Validating the parsed JSON document
       *
       * @param prefabPath String containing the path to the prefab file
       * @return rapidjson::Document Parsed JSON document containing prefab data,
       *         returns empty document if loading fails
       */
       static rapidjson::Document loadPrefab(const std::string& prefabPath);
       /**
        * @brief Updates all prefab instances in the scene based on their master prefab
        *
        * This function manages prefab instance synchronization, including:
        * 1. Loading the master prefab data from file
        * 2. Identifying all prefab instances in the scene
        * 3. Handling component additions and removals
        * 4. Managing property overrides and synchronization
        * 5. Preserving local modifications while updating from master
        *
        * @param handle Asset handle reference to the master prefab
        * @param sceneManager Pointer to the scene manager containing the prefab instances
        */
       static void updateAllPrefabInstance(TeaAsset::AssetHandle& handle, SceneManager::sceneManager* sceneManager);
   };
}