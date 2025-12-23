#include "SkillSystem.h"
#include <cmath>
#include <algorithm>

std::vector<AffectedTile> SkillCalculator::GetAffectedTiles(
    int originX, int originY,
    int targetX, int targetY,
    const SkillData& skill)
{
    switch (skill.pattern)
    {
        case SkillPattern::SingleTile:
            return PatternSingleTile(targetX, targetY);
            
        case SkillPattern::Explosion:
            return PatternExplosion(targetX, targetY, skill.areaSize);
            
        case SkillPattern::LineStraight:
            return PatternLineStraight(originX, originY, targetX, targetY, skill.areaSize, skill.areaWidth);
            
        case SkillPattern::Cross:
            return PatternCross(targetX, targetY, skill.areaSize);
            
        case SkillPattern::Diamond:
            return PatternDiamond(targetX, targetY, skill.areaSize);
            
        default:
            return PatternSingleTile(targetX, targetY);
    }
}

std::vector<AffectedTile> SkillCalculator::PatternSingleTile(int targetX, int targetY)
{
    return {{targetX, targetY}};
}

std::vector<AffectedTile> SkillCalculator::PatternExplosion(int centerX, int centerY, int radius)
{
    std::vector<AffectedTile> affected;
    
    for (int x = centerX - radius; x <= centerX + radius; x++)
    {
        for (int y = centerY - radius; y <= centerY + radius; y++)
        {
            if (IsInCircle(x, y, centerX, centerY, radius))
            {
                affected.push_back({x, y});
            }
        }
    }
    
    return affected;
}

std::vector<AffectedTile> SkillCalculator::PatternLineStraight(
    int originX, int originY, int targetX, int targetY, int length, int width)
{
    std::vector<AffectedTile> finalTiles; // Lista final
    std::vector<AffectedTile> centerLine; // Apenas a linha do meio

    // Direção
    int dx = targetX - originX;
    int dy = targetY - originY;

    // Normaliza
    if (dx != 0) dx = dx / std::abs(dx);
    if (dy != 0) dy = dy / std::abs(dy);

    if (dx == 0 && dy == 0) return finalTiles;

    // Calcular a Linha Central

    // MODO PROJETADO (Comprimento fixo)
    if (length > 0)
    {
        for (int i = 0; i < length; i++)
        {
            int x = targetX + (dx * i);
            int y = targetY + (dy * i);
            centerLine.push_back({x, y});
        }
    }
    // MODO CLÁSSICO (Conectar A -> B)
    else
    {
        int originalDx = targetX - originX;
        int originalDy = targetY - originY;
        int steps = std::max(std::abs(originalDx), std::abs(originalDy));

        for (int i = 1; i <= steps; i++)
        {
            int x = originX + (dx * i);
            int y = originY + (dy * i);
            centerLine.push_back({x, y});
        }
    }

    // Aplicar a Largura

    int perpX = -dy;
    int perpY = dx;

    for (const auto& tile : centerLine)
    {
        // Adiciona o tile central
        finalTiles.push_back(tile);

        // Expande para os lados se tiver largura
        // width aqui funciona como um "raio de espessura".
        // Se width = 1, adiciona 1 na esquerda e 1 na direita.
        for (int w = 1; w <= width; w++)
        {
            // Lado A
            finalTiles.push_back({
                tile.x + (perpX * w),
                tile.y + (perpY * w)
            });

            // Lado B
            finalTiles.push_back({
                tile.x - (perpX * w),
                tile.y - (perpY * w)
            });
        }
    }

    return finalTiles;
}


std::vector<AffectedTile> SkillCalculator::PatternCross(int centerX, int centerY, int radius)
{
    std::vector<AffectedTile> affected;
    
    // Centro
    affected.push_back({centerX, centerY});
    
    // Linhas horizontais e verticais
    for (int i = 1; i <= radius; i++)
    {
        affected.push_back({centerX + i, centerY});
        affected.push_back({centerX - i, centerY});
        affected.push_back({centerX, centerY + i});
        affected.push_back({centerX, centerY - i});
    }
    
    return affected;
}

std::vector<AffectedTile> SkillCalculator::PatternDiamond(int centerX, int centerY, int radius)
{
    std::vector<AffectedTile> affected;
    
    for (int x = centerX - radius; x <= centerX + radius; x++)
    {
        for (int y = centerY - radius; y <= centerY + radius; y++)
        {
            if (ManhattanDistance(x, y, centerX, centerY) <= radius)
            {
                affected.push_back({x, y});
            }
        }
    }
    
    return affected;
}

int SkillCalculator::ManhattanDistance(int x1, int y1, int x2, int y2)
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

bool SkillCalculator::IsInCircle(int x, int y, int centerX, int centerY, int radius)
{
    int dx = x - centerX;
    int dy = y - centerY;
    return (dx * dx + dy * dy) <= (radius * radius);
}
