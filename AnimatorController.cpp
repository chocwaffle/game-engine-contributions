/*====================================================================================
All content (c) 2024 Digipen Institute of Technology Singapore. All rights reserved.

@file        AnimatorController.cpp
@project     TeaEditor
@author(s)   Primary: Bjorn Pokin Chinnaphongse <bjornpokin.c@digipen.edu> (100%)

Contents:     
Implementation of the TeaEditor animation system functions
This source file implements the animation editor functionality defined in Editor.hpp,
providing the core logic for animation creation, playback, and modification within
the TeaEditor environment.
====================================================================================*/

/*                                                                          includes
====================================================================================*/
#include <pch.hpp>
#include "Graphics/graphics.hpp"
#include "Graphics/AnimationClip.hpp"
#include "Asset/assetImporter.hpp"
#include "Asset/AssetManager.hpp"
#include "Asset/Prefab.hpp"
#include "Graphics/Picking.hpp"
#include "Assetbrowser.hpp"

// for ray cast
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>

#include "Graphics/Picking.hpp"
#include "Graphics/Camera.hpp"
#include "Core/LayerManager.hpp"

#include "Viewer/Viewer.hpp"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")
/*                                                              function definitions
====================================================================================*/
namespace TeaEditor {

	void Editor::renderAnimationClipSaveDialog()
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		
		// Add logging to check if BeginPopupModal is called
		if (ImGui::BeginPopupModal("Save Animation Clip", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			AssetBrowser::isPopUpActive = true;
			std::cout << "Save Animation Clip Popup is open" << std::endl;

			ImGui::Text("Enter Animation Clip Name:");
			ImGui::InputText("##clipname", animClipName, sizeof(animClipName));

			if (showClipNameError)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a name!");
			}
			if (showClipNameDuplicate)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "An animation clip with this name already exists!");
			}
			if (showClipNameWriteError)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to save animation clip!");
			}

			ImGui::Separator();
			bool saveAlready = false;
			if (ImGui::Button("Save", ImVec2(120, 0)))
			{
				std::cout << "Save button clicked" << std::endl;

				if (strlen(animClipName) > 0)
				{
					std::string const& clipName = animClipName;
					TeaGraphics::AnimationClipAsset newClip(clipName);

					if (newClip.existFile())
					{
						// File exists, show duplicate error
						showClipNameError = false;
						showClipNameDuplicate = true;
						showClipNameWriteError = false;
						std::cout << "Clip already exists" << std::endl;
					}
					else
					{
						if (newClip.saveToFile())
						{
							showClipNameError = false;
							showClipNameDuplicate = false;
							showClipNameWriteError = false;
							saveAlready = true;
							std::cout << "Clip saved successfully" << std::endl;
						}
						else
						{
							showClipNameError = false;
							showClipNameDuplicate = false;
							showClipNameWriteError = true;
							std::cout << "Failed to save clip" << std::endl;
						}
					}
				}
				else
				{
					showClipNameError = true;
					std::cout << "No name entered" << std::endl;
				}


				if (saveAlready) {
					const std::string directoryPath = "../Asset/Clips/";
					const std::string filePath = directoryPath + animClipName + ".clip";
					TeaAsset::AssetHandle clipHandle = TeaAsset::AssetManager::getAssetHandle(filePath);

					// Add to animator clip list compoennt
					auto& animator = m_HierarchyPanel.getSelectedEntity()
						.getComponent<TeaComponents::BjornAnimator>();
					animator.animationClips[filePath] = clipHandle;
					animator.currentClip = filePath;
					AssetBrowser::isPopUpActive = false;

					// Reset stuff
					showClipNameError = false;
					showClipNameDuplicate = false;
					showClipNameWriteError = false;
					memset(animClipName, 0, sizeof(animClipName));
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				showClipNameError = false;
				showClipNameDuplicate = false;
				showClipNameWriteError = false;
				memset(animClipName, 0, sizeof(animClipName));
				AssetBrowser::isPopUpActive = false;
				std::cout << "Popup canceled" << std::endl;
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
		else
		{
			AssetBrowser::isPopUpActive = false;
		}
	}
	void Editor::renderEventSection(std::shared_ptr<TeaGraphics::AnimationClipAsset> clip) 
	{
		if (!ImGui::CollapsingHeader("Events"))
			return;

		//  Static varibles for creating new event
		static char eventNameBuffer[256] = "";
		static int eventFrame = 0;

		ImGui::InputText("Event Name", eventNameBuffer, sizeof(eventNameBuffer));
		ImGui::InputInt("Event Frame", &eventFrame);

		// Button to add new event 
		if (ImGui::Button("Add Event"))
		{
			// If event name is not empty, add event to clip
			if (strlen(eventNameBuffer) > 0)
			{
				clip->addEvent(eventNameBuffer, eventFrame);
				memset(eventNameBuffer, 0, sizeof(eventNameBuffer));
			}
		}

		// List and manage existing events
		ImGui::Text("Current Events:");
		if (ImGui::BeginTable("Events", 3, ImGuiTableFlags_Borders)) 
		{
			ImGui::TableSetupColumn("Frame");
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Actions");
			ImGui::TableHeadersRow();

			// Iterate through the events in the clip
			for (size_t i = 0; i < clip->events.size(); i++) 
			{
				ImGui::TableNextRow();
				ImGui::PushID(static_cast<int>(i));

				ImGui::TableNextColumn();

				// Get the current frame of the event
				int frame = clip->events[i].keyFrame;
				if (ImGui::InputInt("##Frame", &frame)) // Allow users to edit the frame
				{
					clip->events[i].keyFrame = frame; // Update the events frame if edited
				}

				ImGui::TableNextColumn();
				ImGui::Text("%s", clip->events[i].eventName.c_str()); // Display the event name on the second column

				ImGui::TableNextColumn();
				if (ImGui::Button("X##Delete"))
				{
					// Remove the event from the list
					clip->events.erase(clip->events.begin() + i); 
					i--;
				}

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}

	void Editor::renderRecordingControls(TeaComponents::BjornAnimator& animator, btEngine::Entity& selectedEntity) 
	{
		// Display a button that toggles between "Start Recording" and "Stop Recording"
		if (ImGui::Button(m_sequencer.isRecording ? "Stop Recording" : "Start Recording"))
		{
			if (!m_sequencer.isRecording)
			{
				// Begin recording using the current animation clip and frame
				m_sequencer.StartRecording(animator, animator.currentClip, m_currentFrame);
				animator.isPlaying = false;  // Stop playback if playing
			}
			else 
			{
				// Stop recording when the button is clicked again
				m_sequencer.StopRecording();
			}
		}

		ImGui::SameLine();

		// Logic for recording a keyframe while recording is active
		if (m_sequencer.isRecording)
		{
			// Record a keyframe for the selected entity at the current frame
			m_sequencer.RecordKeyframe(selectedEntity, m_currentFrame);
		}
	}

	void Editor::renderPlaybackControls(TeaComponents::BjornAnimator& animator, std::shared_ptr<TeaGraphics::AnimationClipAsset> clip) {
		
		// Button to toggle between "Play" and "Stop" playback
		if (ImGui::Button(animator.isPlaying ? "Stop" : "Play"))
		{
			// If playback is active, stop it
			if (animator.isPlaying)
			{
				TeaAnimation::Animator::StopPlayback(animator);
			}
			else // If not playing, start playback
			{
				m_sequencer.StopRecording();  // Ensure recording stops before starting playback
				TeaAnimation::Animator::PlayClip(animator, animator.currentClip); // Play the current animation clip
				
				// Reset the current frame to the start of the clip
				m_currentFrame = clip->startFrame;
			}
		}

		ImGui::SameLine();

		// Button to save the current animation clip to a file
		if (ImGui::Button("Save Clip"))
		{
			clip->saveToFile();
		}
	}

	void Editor::renderFrameControls(std::shared_ptr<TeaGraphics::AnimationClipAsset> clip)
	{
		ImGui::PushItemWidth(130);

		// Input for the "Start Frame"
		if (ImGui::InputInt("Start Frame", &clip->startFrame)) 
		{
			clip->startFrame = std::max(0, clip->startFrame); // Ensure the start frame is not negative
			m_sequencer.SetFrameMin(clip->startFrame); // Update the minimum frame in the sequencer
		}

		ImGui::SameLine();

		// Input for the "Current Frame" (displayed on the same line as "Start Frame")
		if (ImGui::InputInt("Current Frame", &m_currentFrame))
		{
			m_currentFrame = std::max(clip->startFrame, std::min(m_currentFrame, clip->endFrame)); // Clamp the current frame between the start and end frame
		}

		ImGui::SameLine();

		if (ImGui::InputInt("End Frame", &clip->endFrame))
		{
			// Ensure the end frame is not less than the start frame
			clip->endFrame = std::max(clip->startFrame, clip->endFrame);

			// Update the maximum frame in the sequencer
			m_sequencer.SetFrameMax(clip->endFrame);
		}

		// Slider for controlling the playback speed
		ImGui::SliderFloat("Speed", &clip->speed, 0.1f, 2.0f, "%.1fx");
		// Checkbox for toggling looping of the animation clip
		ImGui::Checkbox("Loop", &clip->isLooping);
		// Restore the default item width
		ImGui::PopItemWidth();
	}

	void Editor::handlePlaybackUpdate(TeaComponents::BjornAnimator& animator, std::shared_ptr<TeaGraphics::AnimationClipAsset> clip,
		btEngine::Entity& selectedEntity, float deltaTime)
	{
		// Ensure playback only happens if the animator is playing and the editor is in EditMode
		if (!animator.isPlaying || m_engine.getSceneManager().getEditorState() != SceneManager::EditorState::EditMode)
			return;

		// Get the Transform component of the selected entity
		auto& transform = selectedEntity.getComponent<TeaComponents::Transform>();
		animator.timeAccumulator += deltaTime * clip->speed * animator.playbackSpeed;

		// Process frames based on accumulated time at 30fps
		while (animator.timeAccumulator >= (1.0f / 30.0f)) 
		{
			// Deduct one frame's worth of time from the accumulator
			animator.timeAccumulator -= (1.0f / 30.0f);
			m_currentFrame++; // Advance to the next frame

			// Handle end of clip
			if (m_currentFrame > clip->endFrame) 
			{
				if (clip->isLooping)
				{
					m_currentFrame = clip->startFrame;
				}
				else
				{
					m_currentFrame = clip->endFrame;
					animator.isPlaying = false;
					break;
				}
			}

			// Trigger any events associated with the current frame (for testing/debugging)
			auto currentEvents = animator.getCurrentFrameEvents();
			for (const auto& event : currentEvents)
			{
				TEA_INFO("Triggering event: {} at frame {}", event.eventName, event.keyFrame);
			}
		}

		// Apply keyframe data to the transform at the current frame
		TeaAnimation::Animator::ApplyKeyframeAtTime(animator, transform, m_currentFrame);
		// Update the animator's current frame
		animator.currentFrame = m_currentFrame;
	}

	void Editor::renderSequencerUI()
	{
		ImVec2 availableSpace = ImGui::GetContentRegionAvail();
		if (availableSpace.x > 0 && availableSpace.y > 0)
		{
			Sequencer(&m_sequencer,
				&m_currentFrame,
				&m_expanded,
				&m_selectedEntry,
				&m_firstFrame,
				ImSequencer::SEQUENCER_EDIT_STARTEND |
				ImSequencer::SEQUENCER_ADD |
				ImSequencer::SEQUENCER_DEL |
				ImSequencer::SEQUENCER_COPYPASTE |
				ImSequencer::SEQUENCER_CHANGE_FRAME);
		}
	}
	void Editor::renderSequencer(float deltaTime)
	{
		// Begin the Sequencer window
		ImGui::Begin("Sequencer", nullptr, ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::BeginChild("SequencerContent", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

		// Get the currently selected entity from the hierarchy panel
		btEngine::Entity selectedEntity = m_HierarchyPanel.getSelectedEntity();

		// Check if an entity is selected and if it has an animator component
		if (selectedEntity && selectedEntity.hasComponent<TeaComponents::BjornAnimator>())
		{
			m_sequencer.SetEntity(selectedEntity);
		}
		else // If dont have, print out warning to user and return early 
		{
			ImGui::Text("No Animator Component Selected");
			ImGui::EndChild();
			ImGui::End();
			return;
		}

		// If a valid entity with an animator component is selected
		if (selectedEntity && selectedEntity.hasComponent<TeaComponents::BjornAnimator>())
		{
			auto& animator = selectedEntity.getComponent<TeaComponents::BjornAnimator>();

			// Render the Animation Clip combo box for clip management
			if (ImGui::BeginCombo("Animation Clip", animator.currentClip.c_str()))
			{
				// Option to create a new clip
				if (ImGui::Selectable("+ New Clip"))
				{
					memset(animClipName, 0, sizeof(animClipName));
					m_OpenSaveClipPopup = true;
					std::cout << "Press new Clip" << std::endl;
				}

				// List available animation clips
				for (const auto& [name, clipHandle] : animator.animationClips)
				{
					const char* displayName = name.empty() ? "<Unnamed Clip>" : name.c_str();
					if (name.empty()) std::cout << "Empty name" << std::endl;
					if (ImGui::Selectable(displayName, name == animator.currentClip))
					{
						// If user select, then set the selected clip as the current clip
						animator.currentClip = name;
						std::cout << "Selected clip: " << name << std::endl;
					}
				}
				ImGui::EndCombo();
			}

			// Open the popup after the combo box has closed
			if (m_OpenSaveClipPopup)
			{
				ImGui::OpenPopup("Save Animation Clip");
				std::cout << "OpenPopup called" << std::endl;
				m_OpenSaveClipPopup = false; // Reset the flag
				
			}

			// Render the save animation clip popup
			renderAnimationClipSaveDialog();
		
			ImGui::SameLine();

			// Render the "Delete Clip" button if there is a current clip
			if (!animator.currentClip.empty())
			{
				if (ImGui::Button("Delete Clip"))
				{
					m_openDeleteClipPopup = true;
					ImGui::OpenPopup("Delete Clip?");
				}
			}

			// Render the delete confirmation popup
			if (ImGui::BeginPopupModal("Delete Clip?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Are you sure you want to delete this clip?");
				ImGui::Separator();

				if (ImGui::Button("Yes", ImVec2(120, 0)))
				{
					auto clipIt = animator.animationClips.find(animator.currentClip);
					if (clipIt != animator.animationClips.end())
					{
						animator.animationClips.erase(clipIt); // Remove the clip using the iterator
						animator.currentClip = ""; // Clear the current clip
					}
					m_openDeleteClipPopup = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("No", ImVec2(120, 0)))
				{
					m_openDeleteClipPopup = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// Render additional sequencer controls if a current clip is selected
			if (!animator.currentClip.empty())
			{
				if (!animator.currentClip.empty())
				{
					auto clipIt = animator.animationClips.find(animator.currentClip);
					if (clipIt != animator.animationClips.end())
					{
						auto clip = TeaAsset::AssetManager::getAsset<TeaGraphics::AnimationClipAsset>(clipIt->second);
						if (clip)
						{
							renderEventSection(clip);

							ImGui::Separator();

							// Render the various sequencer sections
							renderRecordingControls(animator, selectedEntity);
							renderPlaybackControls(animator, clip);
							renderFrameControls(clip);
							handlePlaybackUpdate(animator, clip, selectedEntity, deltaTime);
							renderSequencerUI();
						}
						else
						{
							std::cout << "Cannot find clip" << std::endl;
						}
					}
					else
					{
						std::cout << "Cannot find clip in animations clip" << std::endl;
					}
				}
			}
		}
		else if (selectedEntity)
		{
			// If an entity is selected but dont have an animator component, add a button for user to add the Animator component
			if (ImGui::Button("Add Animator Component"))
			{
				selectedEntity.addComponent<TeaComponents::BjornAnimator>();
			}
		}

		ImGui::EndChild();
		ImGui::End();
	}
}