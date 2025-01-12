# Individual Contributions to the BobaTime Custom 3D Game Engine

## Overview
As part of the **Software Engineering Project** at DigiPen Institute of Technology Singapore where students are required to built a **Custom 3D Game Engine** that includes tools and features designed to allow game designers to create polished and quality games, I contributed to building key systems that empower game designers with powerful tools for efficient workflows, including an Undo/Redo system, a Prefab system, and an Animation Timeline among other features. These contributions focused on enhancing usability, performance, and productivity within the game engine editor.
![Screenshot 2025-01-10 192658](https://github.com/user-attachments/assets/2b01072b-118e-43cd-a098-106674824d26)

---

## Core Engine Architecture
- Designed the engine architecture to easily and safely integrate multiple subsystems, including physics, graphics, audio, among others.
- Using safe coding practices to ensure efficient memory management and prevent system-wide memory leaks.

---

## Undo/Redo System
- Developed an **Undo/Redo system** using the **Command Pattern**, allowing game designers to reverse and reapply editor operations effortlessly.
- Enabled undo/redo functionality for:
  - Entity creation and deletion.
  - Position adjustments.
  - Material property changes.
  - Additional undo/redo features are currently in the works!

### Example Use Case:
1. A designer adjusts the position of a game object but decides the previous placement was better. With the Undo functionality, they can instantly revert to the previous position without manual rework.
2. Similarly, if material properties such as texture are modified and need to be reverted to their original state, the Undo functionality allows designers to restore the prior settings effortlessly.

### Why This Feature Matters:
- **Efficiency**: Reduces the time spent fixing errors or retracing steps during complex scene editing.
- **Reliability**: Ensures designers can experiment freely without fear of making irreversible changes.
- **Usability**: Provides an intuitive and familiar workflow similar to modern design tools.

![UndoRedo GIF](https://github.com/chocwaffle/game-engine-contributions/blob/main/UndoRedo.gif)

---

## Prefab System
- Developed a **Prefab System** that allows game designers to create reusable, scalable and more importantly consistent game objects across the game.
- Further improve by introducing **parent-child connections** similar to Unity's system, enabling hierarchical relationships between game objects.
  - For example: Adding a child object to a prefab asset and saving it will automatically update all instances of the prefab in the scene, maintaining consistency across designs.
- Collaborated with the Graphics Lead to build a **Prefab Viewer** tool to visualize and modify prefabs in real-time, ensuring instant updates to all instances when a prefab asset is edited.
- Streamlined game development workflows by reducing repetitive manual updates and ensuring design consistency across scenes.
### Example Use Case:
1. Add a child object (e.g., a weapon attachment) to a prefab asset.
2. Click the **Save** button in the editor.
3. All prefab instances in the scene automatically reflect the newly added child object, eliminating the need for manual updates.
### Why This Feature Matters:
- **Consistency**: Ensures that all prefab instances stay synchronized with the original asset.
- **Efficiency**: Saves game designers time by automating updates across the scene.
- **Flexibility**: Supports complex hierarchical relationships for advanced game object setups.

![Prefab System Gif](https://github.com/chocwaffle/game-engine-contributions/blob/main/PrefabSystem.gif)

---

## Animation Timeline
- Built an **Animation Timeline** tool that allows game designers to:
  - Record motion sequences.
  - Save and reuse animation assets across different game objects.
- Simplified the animation creation process, enabling reusability and faster iteration cycles.

### Example Use Case:
1. A designer animates a character's walking sequence using the timeline tool.
2. They save the animation asset and reuse it for multiple characters across different levels.
3. If adjustments are needed, the timeline allows them to edit the motion sequence, save it and re-apply to all character to ensure consistency. 

### Why This Feature Matters:
- **Reusability**: Promotes efficiency by allowing designers to share animations across assets, reducing redundant work.
- **Productivity**: Speeds up the iteration process by enabling quick edits and playback within the editor.
---
![Animator](https://github.com/chocwaffle/game-engine-contributions/blob/main/AnimatorController.gif)

## Additionals Features 
### Scene Manager
Implemeted a **Scene Manager** that is a core engine system responsible for handling scene creation, serialization, deserialization, and state management within the Game Engine. Below are the key features I implemented:

### Key Features:
- **Scene Serialization and Deserialization**:
  - Worked with Serialization Lead to save scenes to disk and reload them efficiently.
- **Editor and Game State Management**:
  - Designed `EditorState` and `GameState` enums to manage the states of the editor and game.
  - Added functionality to toggle between Edit, Pause and Play modes.

- **Entity Management**:
  - Implemented methods to create, destroy, and manage entities within scenes.
  - Designed UUID-based identification to ensure entities are uniquely tracked across scenes.

- **Prefab Support**:
  - Enabled the creation of prefab entities with parent-child relationships, allowing hierarchical scene composition.
  - Added functionality to update all prefab instances when changes are made to the base prefab.

- **Scene Clearing and Removal**:
  - Developed methods to clear all scenes or remove specific scenes without memory leaks.
  - Added logic to ensure entities are properly destroyed and removed from active registries.
 
- **Support for Multiple Scenes**: 
  - Enabled multiple scenes to coexist within a single level, allowing for modular and dynamic level design.
  - Designed the system to manage transitions and interactions between active scenes seamlessly.
    
 ![Scene](https://github.com/chocwaffle/game-engine-contributions/blob/main/MultipleScene.gif)
