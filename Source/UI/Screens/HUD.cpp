//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "../../Game.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    // health bar images
    Vector2 pos = Vector2(-250.0f, 310.0f);
    mHealth1 = AddImage("../Assets/HUD/ShieldOrange.png", pos, 0.75f);
    mHealth2 = AddImage("../Assets/HUD/ShieldRed.png", pos, 0.75f);
    mHealth3 = AddImage("../Assets/HUD/ShieldBlue.png", pos, 0.75f);

    // shield bar
    UIImage* bar = AddImage("../Assets/HUD/ShieldBar.png", Vector2(-255.0f, 305.0f), 0.75f, 0.0f, 90);


    AddText("score:", Vector2(-400.0f, 240.0f), 1.0f);
    mScore = AddText("0", Vector2(-310.0f, 240.0f), 1.0f);
}

void HUD::SetHealth(int health)
{
    // hide all first
    mHealth1->SetIsVisible(false);
    mHealth2->SetIsVisible(false);
    mHealth3->SetIsVisible(false);

    if (health >= 1)
        mHealth2->SetIsVisible(true);

    if (health >= 2)
        mHealth1->SetIsVisible(true);

    if (health >= 3)
        mHealth3->SetIsVisible(true);
}

void HUD::SetScore(int score)
{
    mScore->SetText(std::to_string(score));
}
