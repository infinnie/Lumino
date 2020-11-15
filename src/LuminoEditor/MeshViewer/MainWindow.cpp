﻿
#include "ViewModel.hpp"
#include "Application.hpp"
#include "MainWindow.hpp"
#include "../../../build/BuildCache/imgui/imgui.h"
#include "../../LuminoEngine/src/Mesh/MeshModelInstance.hpp"


MainWindow::MainWindow()
{
}

void MainWindow::onLoaded()
{
	setImGuiLayerEnabled(true);
	m_onImGuiLayer.connect(ln::makeDelegate(this, &MainWindow::handleImGUI));
}

void MainWindow::handleImGUI(ln::UIEventArgs* e)
{
    ImGui::Begin("Statistics");

    const auto& mesh = ViewModel::instance()->mesh();

    if (mesh) {
        const auto& modelInstance = mesh->skinnedMeshComponent()->modelInstance();

        const auto& skeletons = modelInstance->skeletons();
        for (int iSkeleton = 0; iSkeleton < modelInstance->skeletons().size(); iSkeleton++) {
            const auto& skeleton = skeletons[iSkeleton];
            if (ImGui::TreeNode(skeleton, "Skeleton[%d]", iSkeleton)) {

                const auto& bones = skeleton->bones();
                for (int iBone = 0; iBone < bones.size(); iBone++) {
                    const auto& bone = bones[iBone];

                    ImGui::BulletText("Bone[%d] %s", iBone, bone->node()->name().toStdString().c_str());
                }

                ImGui::TreePop();
            }
        }

    }

    ImGui::End();
}
