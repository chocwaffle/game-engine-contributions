/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.
@file        AnimatorController.hpp
@project     TeaEditor
@author(s)   Primary: Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (100%)

Contents:
This header file contains the Editor class definition which manages the animation
editor interface and functionality, including:
1. Animation sequencer and timeline management
2. Event system for animation clips
3. Recording and playback controls
4. Frame control and manipulation
5. Animation clip management (creation, deletion, modification)
6. UI rendering for all animation-related components
====================================================================================*/
#pragma once

/*                                                                          includes
====================================================================================*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../../TeaEngine/Src/Core/logging.hpp"
#include "../Src/Core/engine.hpp"
#include "../Core/filedialog.hpp"
#include "../Core/hierarchypanel.hpp"
#include "../Core/assetbrowser.hpp"
#include "../Core/editorLog.hpp"
#include "../Core/editorOptions.hpp"
#include "../Core/GraphicsPanel.hpp"
#include "../Core/PhysicsPanel.hpp"
#include "../../TeaEngine/Src/Core/scene.hpp"
#include "Asset/assetImporter.hpp"
#include "AnimationController.hpp"
#include "UndoRedo.hpp"

/*                                                             function declarations
====================================================================================*/
namespace TeaEditor
{ 
	class Editor
	{
	public:
		/**
		 * @brief Renders the animation event management section of the editor
		 *
		 * This function handles the UI for animation events, including:
		 * 1. Creating new animation events with name and frame number
		 * 2. Displaying a table of existing events
		 * 3. Allowing modification of event frame numbers
		 * 4. Providing deletion functionality for events
		 *
		 * @param clip Pointer to the animation clip asset being edited
		 */
		void renderEventSection(std::shared_ptr<TeaGraphics::AnimationClipAsset> clip);

		/**
		 * @brief Handles the recording controls UI and functionality
		 *
		 * This function manages:
		 * 1. Start/Stop recording buttons
		 * 2. Recording state management
		 * 3. Keyframe recording for the selected entity
		 * 4. Automatic playback stopping when recording starts
		 *
		 * @param animator Reference to the animator component being recorded
		 * @param selectedEntity Reference to the entity being animated
		 */
		void renderRecordingControls(TeaComponents::BjornAnimator& animator, btEngine::Entity& selectedEntity);

		/**
		 * @brief Manages the animation playback control interface
		 *
		 * This function handles:
		 * 1. Play/Stop animation controls
		 * 2. Clip saving functionality
		 * 3. State management between playing and stopped states
		 * 4. Coordination between recording and playback states
		 *
		 * @param animator Reference to the animator component being controlled
		 * @param clip Pointer to the animation clip being played
		 */
		void renderPlaybackControls(TeaComponents::BjornAnimator& animator, std::shared_ptr<TeaGraphics::AnimationClipAsset> clip);

		/**
		 * @brief Renders the frame control interface for animation editing
		 *
		 * This function manages:
		 * 1. Start frame controls
		 * 2. Current frame selection
		 * 3. End frame controls
		 * 4. Playback speed adjustment
		 * 5. Loop settings
		 * 6. Frame range validation and constraints
		 *
		 * @param clip Pointer to the animation clip being edited
		 */
		void renderFrameControls(std::shared_ptr<TeaGraphics::AnimationClipAsset> clip);

		/**
		 * @brief Renders the main sequencer interface
		 *
		 * This function handles:
		 * 1. Timeline visualization
		 * 2. Keyframe manipulation
		 * 3. Frame selection
		 * 4. Visual timeline controls
		 *
		 * @note This uses the ImSequencer component for timeline management
		 */
		void renderSequencerUI();

		/**
		 * @brief Handles the animation clip save dialog
		 *
		 * This function manages:
		 * 1. New clip name input
		 * 2. Clip saving validation
		 * 3. Error handling for duplicate names
		 * 4. Save confirmation
		 *
		 * @note This is typically triggered when creating a new animation clip
		 */
		void renderAnimationClipSaveDialog();

		/**
		 * @brief Main render function for the animation sequencer editor
		 *
		 * This function coordinates:
		 * 1. Overall sequencer window layout
		 * 2. Component selection and validation
		 * 3. Clip management interface
		 * 4. Integration of all sub-components (events, controls, timeline)
		 *
		 * @param deltaTime Time elapsed since last frame in seconds
		 */
		void renderSequencer(float deltaTime);

		/**
		 * @brief Updates the animation playback state
		 *
		 * This function manages:
		 * 1. Frame advancement based on deltaTime
		 * 2. Loop handling
		 * 3. Event triggering at appropriate frames
		 * 4. Transform updates for the animated entity
		 * 5. Time accumulation for frame-rate independent playback
		 *
		 * @param animator Reference to the animator being updated
		 * @param clip Pointer to the currently playing animation clip
		 * @param selectedEntity Reference to the entity being animated
		 * @param deltaTime Time elapsed since last frame in seconds
		 */
		void handlePlaybackUpdate(TeaComponents::BjornAnimator& animator,
			std::shared_ptr<TeaGraphics::AnimationClipAsset> clip,
			btEngine::Entity& selectedEntity,
			float deltaTime);
	private:
		// Engine reference for entity management
		btEngine::engine& m_engine;
		
		// HierarchyPanel object to get the selected entity
		HierarchyPanel m_HierarchyPanel;

		// AnimationSequencer object to manage the sequencer 
		AnimationSequencer m_sequencer;

		// Sequencer state and settings
		int m_currentFrame = 0;              // Current frame in the sequencer
		bool m_expanded = true;              // UI expansion state
		int m_selectedEntry = -1;            // Selected entry in the sequencer UI
		int m_firstFrame = 0;                // First visible frame in the sequencer

		// Animation clip management
		char animClipName[256] = "";         // Buffer for clip name input
		bool showClipNameError = false;      // Error flag for invalid clip names
		bool showClipNameDuplicate = false;  // Error flag for duplicate clip names
		bool showClipNameWriteError = false; // Error flag for write errors
		bool isClipPopUpActive = false;      // Indicates if a clip popup is active

		// Popup management flags
		bool m_OpenSaveClipPopup = false;    // Flag for save clip popup
		bool m_openDeleteClipPopup = false;  // Flag for delete clip confirmation popup
	};
}


