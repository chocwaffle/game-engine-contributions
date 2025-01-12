/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
@file       UndoRedo.hpp
@project    TeaEditor
@author(s)  Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (primary: 100%)

Contents:
- Command interface for encapsulating operations such as execute, undo, and redo.
- Concrete Command classes for entity creation, transformation, and material changes.
- CommandManager class for managing undo/redo stacks and Command execution flow.

====================================================================================*/
#pragma once

/*                                                                          includes
====================================================================================*/
#include <memory>     
#include <stack>      
#include <iostream>

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

/*                                                             function declarations
====================================================================================*/
namespace Cmd
{
    /**
     * @brief Manages the execution, undoing, and redoing of commands
     *
     * This class handles Command operations, including:
     * 1. Executing new commands
     * 2. Maintaining undo/redo stacks
     * 3. Managing Command history
    */
    class CommandManager
    {
    public:
        /**
        * @brief Executes a Command and adds it to the undo stack
        * @param cmd Unique pointer to the Command to execute
        */
        void executeCommand(std::unique_ptr<Command> cmd);
        void undo();
        void redo();

    private:
        std::stack<std::unique_ptr<Command>> undoStack;
        std::stack<std::unique_ptr<Command>> redoStack;
    };

    /**
     * @brief Base Command class for implementing undo/redo functionality
     *
     * This abstract class provides the interface for Command pattern implementation:
     * 1. Execute for performing the Command
     * 2. Undo for reverting the Command's effects
     * 3. Redo for reapplying the Command
    */
    class Command
    {
    public:
        virtual ~Command() = default;
        virtual void execute() = 0;
        virtual void undo() = 0;
        virtual void redo() = 0;
    };

    /**
     * @brief Command for creating new entities in the scene
     *
     * This Command handles:
     * 1. Entity creation in the scene
     * 2. UUID assignment
     * 3. Proper cleanup during undo operations
    */
    class CreateEntityCommand : public Command 
    {
    public:
        CreateEntityCommand(SceneManager::sceneManager* mgr);

        void execute() override;
        void undo() override;
        void redo() override;

    private:
        SceneManager::sceneManager* m_sceneManager;
        btEngine::Entity m_entity; // Created entity reference
		btEngine::UUID m_uuid;     // UUID of the created entity
    };

    /**
     * @brief Command for transforming entities in the scene
     *
     * This Command manages:
     * 1. Position changes
     * 2. Rotation changes
     * 3. Scale changes
     * 4. Transform history for undo/redo
     */
    class TransformEntityCommand : public Command
    {
    public:
         /**
          * @brief Constructs a transform entity Command
          * @param mgr Pointer to the scene manager
          * @param entity Entity to transform
          * @param oldPos Previous position
          * @param oldRot Previous rotation
          * @param oldScale Previous scale
          * @param newPos New position
          * @param newRot New rotation
          * @param newScale New scale
         */
        TransformEntityCommand(SceneManager::sceneManager* mgr, btEngine::Entity entity,
            const glm::vec3& oldPos, const glm::vec3& oldRot, const glm::vec3& oldScale,
            const glm::vec3& newPos, const glm::vec3& newRot, const glm::vec3& newScale);

        void execute() override;
        void undo() override;
        void redo() override;

    private:
        /**
         * @brief Applies the specified transform values to the entity
         * @param pos Position to apply
         * @param rot Rotation to apply
         * @param scale Scale to apply
        */
        void applyTransform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);

        SceneManager::sceneManager* m_sceneManager;
        btEngine::Entity m_entity;

        glm::vec3 m_oldPosition;
        glm::vec3 m_oldRotation;
        glm::vec3 m_oldScale;

        glm::vec3 m_newPosition;
        glm::vec3 m_newRotation;
        glm::vec3 m_newScale;
    };

    /**
     * @brief Command for changing material instances on entities
     *
     * This Command handles:
     * 1. Material instance changes
     * 2. Property value management
     * 3. RTTR reflection operations
     * 4. Material change history
    */
    class ChangeMaterialInstanceCommand : public Command 
    {
    public:
        /**
         * @brief Constructs a change material instance command
         * @param compInst Component instance to modify
         * @param prop Property to change
         * @param propValue Property value container
         * @param index Index in material array
         * @param oldValue Previous material UUID
         * @param newValue New material UUID
         */
        ChangeMaterialInstanceCommand(rttr::instance compInst,
            rttr::property prop,
            rttr::variant propValue,
            size_t index,
            btEngine::UUID oldValue,
            btEngine::UUID newValue);

        void execute() override;
        void undo() override;
        void redo() override;

    private:
        void applyValue(btEngine::UUID val);

        rttr::instance m_compInst;    // Component instance reference
        rttr::property m_prop;        // Property being modified
        rttr::variant m_propValue;    // Property value container
        size_t m_index;               // Index in material array
        btEngine::UUID m_oldValue;    // Previous material UUID
        btEngine::UUID m_newValue;    // New material UUID
    };
}