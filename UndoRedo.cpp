/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
@file       UndoRedo.cpp
@project    TeaEditor
@author(s)  Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (primary: 100%)

Contents:
- Command interface for encapsulating operations such as execute, undo, and redo.
- Concrete Command classes for entity creation, transformation, and material changes.
- CommandManager class for managing undo/redo stacks and Command execution flow.
====================================================================================*/


/*                                                                          includes
====================================================================================*/
#include "UndoRedo.hpp"

/*                                                              function definitions
====================================================================================*/
namespace Cmd
{
    void CommandManager::executeCommand(std::unique_ptr<Command> cmd) 
    {
        cmd->execute();

        // Add Command to undo stack
        undoStack.push(std::move(cmd));

        // Clear redo stack since we cant redo after a new Command
        while (!redoStack.empty())
        {
            redoStack.pop();
        }
    }

    void CommandManager::undo()
    {
        if (undoStack.empty()) return;

        // Get the most recent Command
        auto cmd = std::move(undoStack.top());
        undoStack.pop();

        // Revert the Commands effects
        cmd->undo();

        // Move Command to redo stack
        redoStack.push(std::move(cmd));
    }

    void CommandManager::redo()
    {
        if (redoStack.empty()) return;

        // Get the most recently undone Command
        auto cmd = std::move(redoStack.top());
        redoStack.pop();

        // Reapply the Commands effects
        cmd->redo();

        // Move Command back to undo stack
        undoStack.push(std::move(cmd));
    }

    CreateEntityCommand::CreateEntityCommand(SceneManager::sceneManager* mgr)
        : m_sceneManager(mgr) {}

    void CreateEntityCommand::execute()
    {
        // Create new entity and store its UUID
        m_entity = m_sceneManager->createEntity();
        m_uuid = m_entity.getUUID();

        TEA_INFO("[CreateEntityCommand] execute() called.\n"
            "Created entity with UUID: {}", m_uuid.toString());
    }

    void CreateEntityCommand::undo() 
    {
        // Remove the created entity
        m_sceneManager->destroyEntity(m_entity);

        TEA_INFO("[CreateEntityCommand] undo() called.\n" 
            "Destroyed entity with UUID: {}", m_uuid.toString());
    }

    void CreateEntityCommand::redo() 
    {
        // Recreate entity with same UUID
        m_entity = m_sceneManager->createEntityWithUUID(m_uuid);

        TEA_INFO("[CreateEntityCommand] redo() called.\n" 
            "Recreated entity with UUID: {}", m_uuid.toString());
    }

    TransformEntityCommand::TransformEntityCommand(SceneManager::sceneManager* mgr, btEngine::Entity entity,
        const glm::vec3& oldPos, const glm::vec3& oldRot, const glm::vec3& oldScale,
        const glm::vec3& newPos, const glm::vec3& newRot, const glm::vec3& newScale)
        : m_sceneManager(mgr), m_entity(entity),
        m_oldPosition(oldPos), m_oldRotation(oldRot), m_oldScale(oldScale),
        m_newPosition(newPos), m_newRotation(newRot), m_newScale(newScale) {}

    void TransformEntityCommand::execute() {
        // Apply the new transform
        applyTransform(m_newPosition, m_newRotation, m_newScale);

        TEA_INFO("[TransformEntityCommand] execute() called.\n"
            "Entity UUID: {}\n"
            "Applied New Transform:\n"
            "   Position: ({}, {}, {})\n"
            "   Rotation: ({}, {}, {})\n"
            "   Scale:    ({}, {}, {})",
            m_entity.getUUID().toString(),
            m_newPosition.x, m_newPosition.y, m_newPosition.z,
            m_newRotation.x, m_newRotation.y, m_newRotation.z,
            m_newScale.x, m_newScale.y, m_newScale.z);
    }

    void TransformEntityCommand::undo() {
        // Revert to old transform
        applyTransform(m_oldPosition, m_oldRotation, m_oldScale);

        TEA_INFO("[TransformEntityCommand] undo() called.\n"
            "Entity UUID: {}\n"
            "Reverted to Old Transform:\n"
            "   Position: ({}, {}, {})\n"
            "   Rotation: ({}, {}, {})\n"
            "   Scale:    ({}, {}, {})",
            m_entity.getUUID().toString(),
            m_oldPosition.x, m_oldPosition.y, m_oldPosition.z,
            m_oldRotation.x, m_oldRotation.y, m_oldRotation.z,
            m_oldScale.x, m_oldScale.y, m_oldScale.z);
    }

    void TransformEntityCommand::redo() {
        // Reapply the new transform
        applyTransform(m_newPosition, m_newRotation, m_newScale);

        TEA_INFO("[TransformEntityCommand] redo() called.\n"
            "Entity UUID: {}\n"
            "Reapplied New Transform:\n"
            "   Position: ({}, {}, {})\n"
            "   Rotation: ({}, {}, {})\n"
            "   Scale:    ({}, {}, {})",
            m_entity.getUUID().toString(),
            m_newPosition.x, m_newPosition.y, m_newPosition.z,
            m_newRotation.x, m_newRotation.y, m_newRotation.z,
            m_newScale.x, m_newScale.y, m_newScale.z);
    }

    void TransformEntityCommand::applyTransform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale) 
    {
        if (m_entity && m_entity.hasComponent<TeaComponents::Transform>()) 
        {
            auto& transform = m_entity.getComponent<TeaComponents::Transform>();
            transform.setPosition(pos);
            transform.setRotation(rot);
            transform.setScale(scale);
        }
    }

    ChangeMaterialInstanceCommand::ChangeMaterialInstanceCommand(rttr::instance compInst,
        rttr::property prop,
        rttr::variant propValue,
        size_t index,
        btEngine::UUID oldValue,
        btEngine::UUID newValue)
        : m_compInst(compInst), m_prop(prop), m_propValue(propValue),
        m_index(index), m_oldValue(oldValue), m_newValue(newValue) {}

    void ChangeMaterialInstanceCommand::execute() 
    {
        applyValue(m_newValue);
        TEA_INFO("[ChangeMaterialInstanceCommand] execute(): Applied new material UUID: {}", m_newValue.toString());
    }

    void ChangeMaterialInstanceCommand::undo()
    {
        applyValue(m_oldValue);
        TEA_INFO("[ChangeMaterialInstanceCommand] undo(): Reverted to old material UUID: {}", m_oldValue.toString());
    }

    void ChangeMaterialInstanceCommand::redo() 
    {
        applyValue(m_newValue);
        TEA_INFO("[ChangeMaterialInstanceCommand] redo(): Reapplied new material UUID: {}", m_newValue.toString());
    }


    void ChangeMaterialInstanceCommand::applyValue(btEngine::UUID val)
    {
        rttr::variant_sequential_view seqView = m_propValue.create_sequential_view();

        if (!seqView.is_valid()) 
        {
            TEA_ERROR("Failed to create sequential_view on propValue");
            return;
        }

        if (!seqView.set_value(m_index, val)) 
        {
            TEA_ERROR("Failed to set material handle in propValue's array at index {}", m_index);
            return;
        }

        if (!m_prop.set_value(m_compInst, m_propValue))
        {
            TEA_ERROR("Failed to set material property on component instance");
        }
    }
}