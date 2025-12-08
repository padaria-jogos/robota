//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "../../Game.h"
#include <algorithm>

namespace
{
    constexpr float kBaseTextureSize = 640.0f;
    constexpr float kTargetRobotSize = 110.0f;
    constexpr float kRobotScale = kTargetRobotSize / kBaseTextureSize;
    constexpr size_t kMaxMessages = 3;
    const Vector2 kPlayerAnchor(-260.0f, 80.0f);
    const Vector2 kEnemyAnchor(260.0f, 80.0f);
    const Vector2 kPlayerNameOffset(-60.0f, 220.0f);
    const Vector2 kEnemyNameOffset(-60.0f, 220.0f);
    const Vector2 kHeadOffset(0.0f, 0.0f);
    const Vector2 kTorsoOffset(0.0f, 0.0f);
    const Vector2 kArmOffset(0.0f, 0.0f);
    const Vector2 kLegOffset(0.0f, 0.0f);
    const Vector2 kMessageShift(0.0f, -70.0f);
}

HUD::HUD(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
    , mPlayerRobot(nullptr)
    , mEnemyRobot(nullptr)
{
    mPlayerWidget = CreateRobotWidget(kPlayerAnchor, "J Robota", kPlayerNameOffset);
    mEnemyWidget = CreateRobotWidget(kEnemyAnchor, "Bea Bee", kEnemyNameOffset);
}

void HUD::TrackRobots(const Robot* playerRobot, const Robot* enemyRobot)
{
    mPlayerRobot = playerRobot;
    mEnemyRobot = enemyRobot;

    UpdateRobotWidget(mPlayerRobot, mPlayerWidget);
    UpdateRobotWidget(mEnemyRobot, mEnemyWidget);
}

void HUD::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);

    UpdateRobotWidget(mPlayerRobot, mPlayerWidget);
    UpdateRobotWidget(mEnemyRobot, mEnemyWidget);
}

void HUD::AddPlayerMessage(const std::string& message)
{
    AppendMessage(mPlayerWidget, message);
}

void HUD::AddEnemyMessage(const std::string& message)
{
    AppendMessage(mEnemyWidget, message);
}

HUD::RobotWidget HUD::CreateRobotWidget(const Vector2& anchor, const std::string& displayName, const Vector2& nameOffset)
{
    RobotWidget widget{};

    widget.label = displayName;
    widget.nameLabel = AddText(displayName, anchor + nameOffset, 0.7f, 0.0f, 36);

    const Vector2 messagePos = anchor + nameOffset + kMessageShift;
    widget.messageLabel = AddText("", messagePos, 0.55f, 0.0f, 26, 420);
    widget.messageLabel->SetText("");

    widget.head = AddImage(BuildTexturePath("g", "head"), anchor + kHeadOffset, kRobotScale);
    widget.torso = AddImage(BuildTexturePath("g", "torso"), anchor + kTorsoOffset, kRobotScale);

    widget.leftArm = AddImage(BuildTexturePath("g", "larm"), anchor + kArmOffset, kRobotScale);
    widget.rightArm = AddImage(BuildTexturePath("g", "rarm"), anchor + kArmOffset, kRobotScale);

    widget.leftLeg = AddImage(BuildTexturePath("g", "lleg"), anchor + kLegOffset, kRobotScale);
    widget.rightLeg = AddImage(BuildTexturePath("g", "rleg"), anchor + kLegOffset, kRobotScale);

    return widget;
}

void HUD::UpdateRobotWidget(const Robot* robot, RobotWidget& widget)
{
    if (!widget.nameLabel)
    {
        return;
    }
    if (widget.nameLabel && !widget.label.empty())
    {
        widget.nameLabel->SetText(widget.label);
    }
    if (!robot)
    {
        RobotPart empty;
        UpdatePartVisual(empty, widget.head, "head");
        UpdatePartVisual(empty, widget.torso, "torso");
        UpdatePartVisual(empty, widget.leftArm, "larm");
        UpdatePartVisual(empty, widget.rightArm, "rarm");
        UpdatePartVisual(empty, widget.leftLeg, "lleg");
        UpdatePartVisual(empty, widget.rightLeg, "rleg");
        return;
    }

    UpdatePartVisual(robot->GetPart(PartSlot::Head), widget.head, "head");
    UpdatePartVisual(robot->GetPart(PartSlot::Torso), widget.torso, "torso");
    UpdatePartVisual(robot->GetPart(PartSlot::LeftArm), widget.leftArm, "larm");
    UpdatePartVisual(robot->GetPart(PartSlot::RightArm), widget.rightArm, "rarm");

    const RobotPart& legs = robot->GetPart(PartSlot::Legs);
    UpdatePartVisual(legs, widget.leftLeg, "lleg");
    UpdatePartVisual(legs, widget.rightLeg, "rleg");
}

void HUD::UpdatePartVisual(const RobotPart& part, UIImage* image, const std::string& suffix)
{
    if (!image)
    {
        return;
    }

    const std::string prefix = PickColorPrefix(part);
    image->SetTexture(BuildTexturePath(prefix, suffix));
}

void HUD::AppendMessage(RobotWidget& widget, const std::string& message)
{
    if (!widget.messageLabel || message.empty())
    {
        return;
    }

    widget.messages.emplace_front(message);
    while (widget.messages.size() > kMaxMessages)
    {
        widget.messages.pop_back();
    }

    std::string combined;
    for (size_t i = 0; i < widget.messages.size(); ++i)
    {
        combined += widget.messages[i];
        if (i + 1 < widget.messages.size())
        {
            combined += "\n";
        }
    }

    widget.messageLabel->SetText(combined);
}

std::string HUD::PickColorPrefix(const RobotPart& part) const
{
    if (part.maxHP <= 0)
    {
        return "p";
    }

    const float normalized = std::clamp(static_cast<float>(part.currentHP), 0.0f, static_cast<float>(part.maxHP)) /
                             static_cast<float>(part.maxHP);

    if (normalized <= 0.0f)
    {
        return "p";
    }
    else if (normalized <= 0.25f)
    {
        return "r";
    }
    else if (normalized <= 0.5f)
    {
        return "y";
    }

    return "g";
}

std::string HUD::BuildTexturePath(const std::string& prefix, const std::string& suffix) const
{
    return "../Assets/HUD/hudRobot/" + prefix + suffix + ".png";
}
