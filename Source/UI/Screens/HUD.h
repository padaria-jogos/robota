//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <deque>
#include <string>
#include "UIScreen.h"
#include "../../Actors/Robot.h"

class HUD : public UIScreen
{
public:
    HUD(class Game* game, const std::string& fontName);

    void TrackRobots(const Robot* playerRobot, const Robot* enemyRobot);
    void Update(float deltaTime) override;

    void AddPlayerMessage(const std::string& message);
    void AddEnemyMessage(const std::string& message);

private:
    struct RobotWidget
    {
        UIText* nameLabel;
        UIText* messageLabel;
        UIImage* head;
        UIImage* torso;
        UIImage* leftArm;
        UIImage* rightArm;
        UIImage* leftLeg;
        UIImage* rightLeg;
        std::deque<std::string> messages;
        std::string label;
    };

    RobotWidget CreateRobotWidget(const Vector2& anchor, const std::string& displayName, const Vector2& nameOffset);
    void UpdateRobotWidget(const Robot* robot, RobotWidget& widget);
    void UpdatePartVisual(const RobotPart& part, UIImage* image, const std::string& suffix);
    void AppendMessage(RobotWidget& widget, const std::string& message);
    std::string PickColorPrefix(const RobotPart& part) const;
    std::string BuildTexturePath(const std::string& prefix, const std::string& suffix) const;

    const Robot* mPlayerRobot;
    const Robot* mEnemyRobot;

    RobotWidget mPlayerWidget;
    RobotWidget mEnemyWidget;
};
