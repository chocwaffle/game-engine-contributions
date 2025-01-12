/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
@file       Prefab.cpp
@project    TeaEditor
@author(s)  Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (primary: 100%)

Contents:
- Prefab instance updating functionality
====================================================================================*/

/*                                                                          includes
====================================================================================*/
#include "pch.hpp"
#include "../../TeaEngine/Src/Graphics/camera.hpp"
#include "../../TeaEngine/Src/Core/Serializer.hpp"
#include "Asset/MetadataSerializer.hpp"
#include "Asset/Prefab.hpp"
#include <random>

/*                                                              function definitions
====================================================================================*/
namespace TeaEditor
{ 
    rapidjson::Document HierarchyPanel::loadPrefab(const std::string& prefabPath)
    {
        // Open the file
        std::ifstream prefabFile(prefabPath);
        if (!prefabFile.is_open())
        {
            TEA_INFO("Failed to open prefab file: {0}", prefabPath);
            return {};
        }

        // Parse the JSON file
        rapidjson::IStreamWrapper isw(prefabFile);
        rapidjson::Document prefabDoc;
        prefabDoc.ParseStream(isw);

        if (prefabDoc.HasParseError()) 
        {
            TEA_INFO("Failed to parse prefab file: {0}", prefabPath);
        }

        return prefabDoc;
    }
    void HierarchyPanel::updateAllPrefabInstance(TeaAsset::AssetHandle& handle, SceneManager::sceneManager* sceneManager)
    {
        // Get the path to the master prefab
        std::string prefabPath = TeaAsset::AssetManager::getSourceFilePath(handle).string();

        // Load the prefab JSON using the helper function
        rapidjson::Document prefabDoc = loadPrefab(prefabPath);
        if (prefabDoc.IsNull()) 
        {
            // Exit if loading failed
            return; 
        }

        // Getting the registry and all entities with UUIDComponent and OverrideComponent (basically prefab instances in the scene)
        auto* registry = sceneManager->getRegistry();
        auto view = registry->view<TeaComponents::UUIDComponent, TeaComponents::OverrideComponent>();

        // Iterate tthrough the entities with UUIDComponent and OverrideComponent
        for (auto entityHandle : view)
        {
            btEngine::Entity entity(entityHandle, registry);

            auto& overrideComp = entity.getComponent<TeaComponents::OverrideComponent>();
            if (overrideComp.masterPrefabHandle == handle) 
            {
                const auto& entityData = prefabDoc["Entity"];
                if (!entityData.IsObject()) 
                {
                    continue;
                }

                // Get all registered component types
                std::vector<rttr::type> componentTypes = TeaComponents::ComponentManager::getAllComponentTypes();

                // Iterate through each registered component type
                for (const auto& componentType : componentTypes)
                {
                    std::string typeName = componentType.get_name().to_string();
                    if (typeName == "Scene ID") continue;

                    // Check if component exists in prefab
                    bool existsInPrefabAsset = entityData.HasMember(typeName.c_str());
                    bool existsInPrefabInstance = entity.hasComponent(typeName);

                    // Check if the component was added locally via the OverrideComponent Vector that stores the component name
                    bool isLocallyAdded = false; 
                    for (size_t i = 0; i < overrideComp.components.size(); i++)
                    {
                        if (overrideComp.components[i].componentName == typeName)
                        {
                            // If we find the component in the overrideComp vector, it means it was added locally by the user
                            isLocallyAdded = true; 
                            break;
                        }
                    }

                    // Case 1: Component exists in prefab asset but not in prefab instance neighter was it added locally by the user, can go ahead and add it
                    if (existsInPrefabAsset && !existsInPrefabInstance && !isLocallyAdded)
                    {
                        if (typeName == "Child" || typeName == "Parent") continue;

                        // Add the component from prefab
                        entity.addComponent(typeName);
                    }
                    // Case 2: Component exists in prefab instance but not in prefab asset
                    else if (!existsInPrefabAsset && existsInPrefabInstance)
                    {
                        if (typeName == "UUIDComponent" || typeName == "OverrideComponent") continue;

                        // Only keep the component if it was locally added by the user. Meaning the user intentionally added it
                        if (!isLocallyAdded)
                        {
                            entity.removeComponent(typeName);
                        }
                    }

                    // Checking if the entity in the prefab has this component
                    if (entityData.HasMember(typeName.c_str()))
                    {
                        const auto& componentData = entityData[typeName.c_str()];

                        // Get component 
                        rttr::variant componentVar = entity.getComponent(typeName);
                        if (!componentVar.is_valid()) {
                            TEA_WARNING("Entity does not have component {0}. Skipping.", typeName);
                            continue; // Skip to the next component type
                        }
                        rttr::instance component = componentVar;

                        // Iterate through each property of the component
                        for (auto& prop : componentType.get_properties())
                        {
                            std::string propName = prop.get_name().to_string();
                            std::string propertyPath = typeName + "/" + propName;
                            if (propName == "Scene ID") continue;
                            if (propName == "Parent") continue;

                            // Check if this property has been overridden
                            auto overrideIt = std::find_if(overrideComp.properties.begin(), overrideComp.properties.end(),
                                [&propertyPath](const TeaComponents::OverrideComponent::Property& overrideProp) {
                                    return overrideProp.path == propertyPath;
                                });

                            // If the property is not overridden and exists in the prefab data, update it
                            if (overrideIt == overrideComp.properties.end() && componentData.HasMember(propName.c_str()))
                            {
                                const rapidjson::Value& propValue = componentData[propName.c_str()];
                                rttr::variant propObj = prop.get_value(component);
                                rttr::type propType = prop.get_type();
                                bool success = false;

                                // Update the property based on its type
                                Serialize::deserializeEachProperty(propValue, propObj, propType);
                                success = prop.set_value(component, propObj);
                            }
                        }
                    }
                }
            }
            else
            {
                TEA_INFO("Entity is not a prefab instance of {0}, so skip and check the next entity", prefabPath);
            }
        }
    }
}