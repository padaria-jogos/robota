//
// Created by mateus on 10/12/2025.
//

#include "GarageScreen.h"

namespace
{
    constexpr float kButtonRowY = -335.0f;
    constexpr float kRectButtonScale = 0.30f;
    constexpr float kSquareButtonScale = 0.30f;
    constexpr int kButtonPointSize = 32;
    constexpr unsigned kRectWrapLength = 360u;
    constexpr unsigned kSquareWrapLength = 120u;
    const Vector2 kRectTextMargin(30.0f, 8.0f);
    const Vector2 kSquareTextMargin(16.0f, 8.0f);

    constexpr float kButtonColumnSpacing = 150.0f;

    const float newOffset = 60.0f;
    const Vector2 kCameraButtonOffset(-1.5f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kBackButtonOffset(-1.0f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kLeftArmButtonOffset(0.37f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kRightArmButtonOffset(1.34f * kButtonColumnSpacing - newOffset, kButtonRowY);
    const Vector2 kCancelArmButtonOffset(2.28f * kButtonColumnSpacing - newOffset, kButtonRowY);

}

GarageScreen::GarageScreen(class Game* game, Robot* robot)
        :UIScreen(game, "../Assets/Fonts/Arial.ttf"),
        mGame(game),
        mRobot(robot)
{
    switch (mGame->GetLastLevelCompleted())
    {
        case 0:
            part1 = "../Assets/Parts/Torsos/EvilTrashcanChest.json";
            part2 = "../Assets/Parts/Arms/EvilBroomArm.json";
            mNextGameScene = GameScene::Level1;
            break;
        case 1:
            part1 = "Arm_Lv2";
            part2 = "Arm_Lv2";
            break;
        case 2:
            part1 = "Arm_Lv3";
            part2 = "Arm_Lv3";
            break;
        default:
            part1 = "Arm_Lv1";
            part2 = "Arm_Lv1";
            break;
    }


    // add game logo
    // AddImage("../Assets/UIBackground.png", Vector2(0.0f, 0.0f), 0.7f, 0.0f, 1);

    AddImage("../Assets/HUD/Garage/boxBackground.png", Vector2(-230.0f, 0.0f), 0.9f, 0.0f, 1);


    // botão
    UIButton* option1 = AddButton("Braço Esquerdo", [this]() {
        if (mRobot)
            mRobot->EquipPartFromJson(part1);
    }, Vector2(-230.0f, 60.0f), 1.0f, 0.0f, kButtonPointSize, kRectWrapLength, 1000);

    option1->SetText("");
    option1->SetAlignment(UITextAlignment::Center);
    option1->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    option1->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    option1->SetScale(1.0f);
    option1->SetBackgroundScale(0.8f);
    option1->SetBackgroundTextures("../Assets/HUD/Garage/Level0/boxPart-Chest.png", "../Assets/HUD/Garage/Level0/boxPartEnable-Chest.png");
    option1->SetMargin(kRectTextMargin);

    // botão
    UIButton* option2 = AddButton("Braço Esquerdo", [this]() {
        if (mRobot)
            mRobot->EquipPartFromJson(part2);
    }, Vector2(-230.0f, -105.0f), 1.0f, 0.0f, kButtonPointSize, kRectWrapLength, 1000);

    option2->SetText("");
    option2->SetAlignment(UITextAlignment::Center);
    option2->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    option2->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    option2->SetScale(1.0f);
    option2->SetBackgroundScale(0.8f);
    option2->SetBackgroundTextures("../Assets/HUD/Garage/Level0/boxPart-Arm.png", "../Assets/HUD/Garage/Level0/boxPartEnable-Arm.png");
    option2->SetMargin(kRectTextMargin);

    // btn confirma
    UIButton* btnConfirm = AddButton("Braço Esquerdo", [this]() {
        if (mRobot)
            mRobot->SaveToJson("../Saves/PlayerBuild.json");
        this->Close();
        mGame->SetScene(mNextGameScene);
    }, Vector2(-227.0f, -230.0f), 1.0f, 0.0f, kButtonPointSize, kRectWrapLength, 1000);

    btnConfirm->SetText("");
    btnConfirm->SetAlignment(UITextAlignment::Center);
    btnConfirm->SetBackgroundColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    btnConfirm->SetTextColor(Vector3(1.0f, 1.0f, 1.0f));
    btnConfirm->SetScale(kRectButtonScale);
    btnConfirm->SetBackgroundScale(1.0f);
    btnConfirm->SetBackgroundTextures("../Assets/HUD/Buttons/Level/btnConfirma.png", "../Assets/HUD/Buttons/Level/btnConfirmaHold.png");
    btnConfirm->SetMargin(kRectTextMargin);



    // -----

    for (auto* button : mButtons)
    {
        button->SetHighlighted(false);
    }

    mSelectedButtonIndex = 0;
    for (int i = 0; i < static_cast<int>(mButtons.size()); ++i)
    {
        if (mButtons[i] == option1)
        {
            mSelectedButtonIndex = i;
            break;
        }
    }

    if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size()))
    {
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
}

void GarageScreen::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    // navega de acordo com o input
    switch (key)
    {
        case SDLK_w:
        case SDLK_a:
        {
            // remove highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            // move para esquerda (circular)
            mSelectedButtonIndex--;
            if (mSelectedButtonIndex < 0)
                mSelectedButtonIndex = (int)mButtons.size() - 1;

            // highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
            break;

        case SDLK_s:
        case SDLK_d:
        {
            // remove highlight
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            // move para direita (circular)
            mSelectedButtonIndex++;
            if (mSelectedButtonIndex >= (int)mButtons.size())
                mSelectedButtonIndex = 0;

            // novo destaque
            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
            break;

        case SDLK_SPACE:
        {
            // click no botão selecionado
            mButtons[mSelectedButtonIndex]->OnClick();
        }
            break;

        default:
            break;

    }
}