//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "../../Game.h"
#include <sstream>

namespace
{
    constexpr float kBaseTextureSize = 640.0f;
    constexpr float kTargetRobotSize = 110.0f;
    constexpr float kRobotScale = kTargetRobotSize / kBaseTextureSize;
    constexpr float kStatusFrameScale = kRobotScale;
    constexpr size_t kMaxMessages = 2;
    constexpr float kScreenHalfWidth = static_cast<float>(Game::WINDOW_WIDTH) * 0.5f;
    constexpr float kScreenHalfHeight = static_cast<float>(Game::WINDOW_HEIGHT) * 0.5f;
    constexpr float kScreenMarginX = 40.0f;
    constexpr float kScreenMarginY = 10.0f;
    constexpr float kStatusSpriteWidth = 200.0f;
    constexpr float kStatusSpriteHeight = 300.0f;
    constexpr float kStatusSpriteScale = 0.7f;
    constexpr float kStatusPanelWidth = kStatusSpriteWidth * kStatusSpriteScale;
    constexpr float kStatusPanelHeight = kStatusSpriteHeight * kStatusSpriteScale;
    const Vector2 kStatusPanelSize(kStatusPanelWidth, kStatusPanelHeight);

    constexpr float kNameSpriteWidth = 360.0f;
    constexpr float kNameSpriteHeight = 300.0f;
    constexpr float kNameSpriteScale = kStatusPanelHeight / kNameSpriteHeight;
    const Vector2 kNamePanelSize(kNameSpriteWidth * kNameSpriteScale,
                                 kNameSpriteHeight * kNameSpriteScale);
    const Vector2 kPlayerAnchor(-kScreenHalfWidth + kStatusPanelSize.x * 0.5f + kScreenMarginX,
                               kScreenHalfHeight - kStatusPanelSize.y * 0.5f - kScreenMarginY);
    const Vector2 kEnemyAnchor(kScreenHalfWidth - kStatusPanelSize.x * 0.5f - kScreenMarginX,
                              kScreenHalfHeight - kStatusPanelSize.y * 0.5f - kScreenMarginY);
    const Vector2 kHeadOffset(0.0f, 0.0f);
    const Vector2 kTorsoOffset(0.0f, 0.0f);
    const Vector2 kArmOffset(0.0f, 0.0f);
    const Vector2 kLegOffset(0.0f, 0.0f);
    constexpr float kHorizontalGap = 20.0f;
    constexpr float kNamePanelVerticalOffset = 30.0f;
    constexpr float kNameLabelOffsetY = 57.0f;
    constexpr float kLogLabelOffsetY = -5.0f;
    constexpr Vector2 kLogMargin(12.0f, 12.0f);
    constexpr float kStatusTitleInset = 18.0f;
    constexpr float kLogTextInset = 22.0f;
    constexpr unsigned kNameWrapLength = 220u;
    constexpr unsigned kLogWrapLength = 150u;
    constexpr int kPanelDrawOrder = 80;
    constexpr int kFrameDrawOrder = 85;
    constexpr int kRobotDrawOrder = 90;
    constexpr int kTextDrawOrder = 110;
    const Vector4 kPanelColor(0.07f, 0.07f, 0.07f, 0.9f);
}

HUD::HUD(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
    , mPlayerRobot(nullptr)
    , mEnemyRobot(nullptr)
    , mMenuVisible(true)
{
    mPlayerWidget = CreateRobotWidget(kPlayerAnchor, "J. Robota", PanelOrientation::TextToRight);
    mEnemyWidget = CreateRobotWidget(kEnemyAnchor, "", PanelOrientation::TextToLeft);
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

    if (!hasName)
    {
        setEnemyRobotWidgetName(mGame->GetLevel()->GetEnemyName());
        hasName = true;
    }


    UpdateRobotWidget(mPlayerRobot, mPlayerWidget);
    UpdateRobotWidget(mEnemyRobot, mEnemyWidget);
}

void HUD::SetVisible(bool visible)
{
    if (mMenuVisible == visible)
    {
        return;
    }

    mMenuVisible = visible;

    const auto applyVisibility = [visible](auto& collection)
    {
        for (auto element : collection)
        {
            if (element)
            {
                element->SetIsVisible(visible);
            }
        }
    };

    applyVisibility(mButtons);
    applyVisibility(mTexts);
    applyVisibility(mImages);
    applyVisibility(mRects);
}

void HUD::AddPlayerMessage(const std::string& message)
{
    AppendMessage(mPlayerWidget, message);
}

void HUD::AddEnemyMessage(const std::string& message)
{
    AppendMessage(mEnemyWidget, message);
}

HUD::RobotWidget HUD::CreateRobotWidget(const Vector2& anchor, const std::string& displayName, PanelOrientation orientation)
{
    RobotWidget widget{};

    widget.label = displayName;
    widget.orientation = orientation;

    const float direction = (orientation == PanelOrientation::TextToRight) ? 1.0f : -1.0f;
    const Vector2 statusHalf(kStatusPanelSize.x * 0.5f, kStatusPanelSize.y * 0.5f);

    const bool isPlayer = (orientation == PanelOrientation::TextToRight);
    const std::string robotPanelTexture = isPlayer ? "../Assets/HUD/boxPlayerRobot.png" : "../Assets/HUD/boxEnemyRobot.png";
    widget.statusPanel = AddImage(robotPanelTexture, anchor, kStatusSpriteScale, 0.0f, kPanelDrawOrder);

    widget.statusTitle = AddText("Status", anchor + Vector2(0.0f, statusHalf.y - kStatusTitleInset), 1.0f, 0.0f, 26, 256, kTextDrawOrder);
    widget.statusTitle->SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    widget.statusTitle->SetMargin(Vector2::Zero);

    widget.margin = AddImage(BuildTexturePath("", "margem"), anchor, kStatusFrameScale, 0.0f, kFrameDrawOrder);

    widget.head = AddImage(BuildTexturePath("g", "head"), anchor + kHeadOffset, kRobotScale, 0.0f, kRobotDrawOrder);
    widget.torso = AddImage(BuildTexturePath("g", "torso"), anchor + kTorsoOffset, kRobotScale, 0.0f, kRobotDrawOrder);

    widget.leftArm = AddImage(BuildTexturePath("g", "larm"), anchor + kArmOffset, kRobotScale, 0.0f, kRobotDrawOrder);
    widget.rightArm = AddImage(BuildTexturePath("g", "rarm"), anchor + kArmOffset, kRobotScale, 0.0f, kRobotDrawOrder);

    widget.leftLeg = AddImage(BuildTexturePath("g", "lleg"), anchor + kLegOffset, kRobotScale, 0.0f, kRobotDrawOrder);
    widget.rightLeg = AddImage(BuildTexturePath("g", "rleg"), anchor + kLegOffset, kRobotScale, 0.0f, kRobotDrawOrder);

    const float nameLateralOffset = statusHalf.x + kHorizontalGap + (kNamePanelSize.x * 0.5f);
    const Vector2 nameCenter = anchor + Vector2(direction * nameLateralOffset, kNamePanelVerticalOffset);
    const std::string namePanelTexture = isPlayer ? "../Assets/HUD/boxPlayerLog.png" : "../Assets/HUD/boxEnemyLog.png";
    widget.namePanel = AddImage(namePanelTexture, nameCenter, kNameSpriteScale, 0.0f, kPanelDrawOrder);

    widget.nameLabel = AddText("", nameCenter + Vector2(0.0f, kNameLabelOffsetY), 1.0f, 0.0f, 26, kNameWrapLength, kTextDrawOrder + 1);
    widget.nameLabel->SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    widget.nameLabel->SetMargin(Vector2::Zero);
    widget.nameLabel->SetText(widget.label);

    widget.logPanel = nullptr;

    // widget.messageLabel = AddText("", nameCenter + Vector2(7.0f, kLogLabelOffsetY), 1.0f, 0.0f, 16, kLogWrapLength, kTextDrawOrder + 1);
    // widget.messageLabel->SetText("");
    // widget.messageLabel->SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    // // widget.messageLabel->SetMargin(kLogMargin);
    // widget.messageLabel->SetMargin(Vector2::Zero);
    // widget.messageLabel->SetAlignment(UITextAlignment::Left);

    widget.messageLabel = AddText("", nameCenter + Vector2(0.0f, kLogLabelOffsetY), 0.95f, 0.0f, 16, kLogWrapLength, kTextDrawOrder + 1);
    widget.messageLabel->SetText("");
    widget.messageLabel->SetBackgroundColor(Vector4(0,0,0,0));
    widget.messageLabel->SetMargin(Vector2::Zero);

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
        return;

    const int MAX_CHAR_PER_LINE = 26;
    const int MAX_LINES = 4;

    // word wrap
    std::vector<std::string> newLines;
    std::string working = message;
    size_t pos = 0;

    while (!working.empty())
    {
        // quebra manual em \n
        size_t newlinePos = working.find('\n');
        std::string segment;

        if (newlinePos != std::string::npos)
        {
            segment = working.substr(0, newlinePos);
            working.erase(0, newlinePos + 1);
        }
        else
        {
            segment = working;
            working.clear();
        }

        // agora faz word-wrap no segmento
        while (!segment.empty())
        {
            if (segment.size() <= MAX_CHAR_PER_LINE)
            {
                newLines.push_back(segment);
                break;
            }

            // procura o último espaço antes do limite
            size_t breakPos = segment.rfind(' ', MAX_CHAR_PER_LINE);

            if (breakPos == std::string::npos)
            {
                // nenhuma quebra possível → força quebra
                newLines.push_back(segment.substr(0, MAX_CHAR_PER_LINE));
                segment.erase(0, MAX_CHAR_PER_LINE);
            }
            else
            {
                // quebra segura antes da palavra
                newLines.push_back(segment.substr(0, breakPos));
                segment.erase(0, breakPos + 1); // remove também o espaço
            }
        }

        // caso segmento fosse vazio após newline
        if (segment.empty() && newlinePos != std::string::npos)
            newLines.push_back("");
    }

    // empilha no buffer - FIFO
    for (const auto& ln : newLines)
    {
        widget.lineBuffer.push_back(ln);
        if (widget.lineBuffer.size() > MAX_LINES)
            widget.lineBuffer.pop_front();
    }

    // garante o máximo de linhas vazias
    while (widget.lineBuffer.size() < MAX_LINES)
        widget.lineBuffer.push_back("");

    // Remove linhas vazias do TOPO se houver linhas não-vazias
    // e adiciona as mesmas no fim (altura fixa + empurra texto)
    bool hasNonEmpty = false;
    for (const auto& ln : widget.lineBuffer)
        if (!ln.empty())
            hasNonEmpty = true;

    if (hasNonEmpty)
    {
        size_t removed = 0;
        while (!widget.lineBuffer.empty() && widget.lineBuffer.front().empty())
        {
            widget.lineBuffer.pop_front();
            removed++;
        }
        for (size_t i = 0; i < removed; i++)
            widget.lineBuffer.push_back("");
    }

    // segurança
    while (widget.lineBuffer.size() > MAX_LINES)
        widget.lineBuffer.pop_front();

    // monta texto final com \n
    std::string combined;
    for (size_t i = 0; i < widget.lineBuffer.size(); i++)
    {
        combined += widget.lineBuffer[i];
        if (i + 1 < widget.lineBuffer.size())
            combined += '\n';
    }

    widget.messageLabel->SetText(combined);
}


std::string HUD::PickColorPrefix(const RobotPart& part) const
{
    if (part.maxHP <= 0)
    {
        return "p";
    }

    const float maxHP = static_cast<float>(part.maxHP);
    const float clampedHP = Math::Clamp(static_cast<float>(part.currentHP), 0.0f, maxHP);
    const float normalized = clampedHP / maxHP;

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
    const std::string base = "../Assets/HUD/hudRobot/";
    if (prefix.empty())
    {
        return base + suffix + ".png";
    }
    return base + prefix + suffix + ".png";
}
