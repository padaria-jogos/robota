//
// Created by mateus on 20/11/2025.
//

#include "Level1.h"
// #include "Map/GridMap.h"

Level1::Level1(Game* game, HUD *hud) : Level(game, hud)
{
    // Setup do Player para Arena 1
    mPlayer->SetName("Rosevif");
    
    mPlayer->EquipPart(PartSlot::Torso,
                         RobotPart("Trashcan Chest", "../Assets/Robots/Rosevif/RosevifTorso.gpmesh",
                                   50, SkillType::None, 0, 0));
    mPlayer->EquipPart(PartSlot::RightArm,
                          RobotPart("Robota Dustpan", "../Assets/Robots/Rosevif/RosevifRightArm.gpmesh",
                                    20, SkillType::Missile, 1000, 3));
    mPlayer->EquipPart(PartSlot::LeftArm,
                          RobotPart("Robota Broom", "../Assets/Robots/Rosevif/RosevifLeftArm.gpmesh",
                                    10, SkillType::Punch, 50, 1));
    mPlayer->EquipPart(PartSlot::Legs,
                          RobotPart("Robota Legs", "../Assets/Robots/Rosevif/RosevifLeftLeg.gpmesh",
                                    20, SkillType::None, 0, 2));
    mPlayer->EquipPart(PartSlot::Head,
                          RobotPart("Robota Head", "../Assets/Robots/Rosevif/RosevifHead.gpmesh",
                                    30, SkillType::Repair, 0, 0));
    
    // Setup do Enemy para Arena 1
    mEnemy->SetName("EvilBee");
    
    mEnemy->EquipPart(PartSlot::Torso,
                     RobotPart("Honey Chest", "../Assets/Robots/BeaBee/BeaBeeTorso.gpmesh",
                               50, SkillType::None, 0, 0));
    mEnemy->EquipPart(PartSlot::RightArm,
                          RobotPart("Honey Blast", "../Assets/Robots/BeaBee/BeaBeeRightArm.gpmesh",
                                    10, SkillType::Missile, 20, 3));
    mEnemy->EquipPart(PartSlot::LeftArm,
                          RobotPart("Queen's Drill", "../Assets/Robots/BeaBee/BeaBeeLeftArm.gpmesh",
                                    10, SkillType::Punch, 50, 1));
    mEnemy->EquipPart(PartSlot::Legs,
                          RobotPart("Honey Boots", "../Assets/Robots/BeaBee/BeaBeeLeftLeg.gpmesh",
                                    30, SkillType::None, 0, 2));
    mEnemy->EquipPart(PartSlot::Head,
                          RobotPart("Queen's Crown", "../Assets/Robots/BeaBee/BeaBeeHead.gpmesh",
                                    30, SkillType::Repair, 0, 0));
    
    // Carrega a Arena 1
    LevelConfig config;
    if (LoadLevelConfig("../Assets/Arena/Arena1/Arena1.json", config)) {
        LoadLevel(config);
    } else {
        SDL_Log("ERRO: Falha ao carregar Arena1!");
    }
    
    // Posiciona cursor inicial
    if (mGrid) {
        MoveInGrid(mCursor, mPlayer->GetGridX(), mPlayer->GetGridY());
    }
}

void Level1::OnUpdate(float deltaTime)
{
    Level::OnUpdate(deltaTime);
}

void Level1::ProcessInput(SDL_Event &event)
{

}

