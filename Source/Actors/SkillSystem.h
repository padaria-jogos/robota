#pragma once

#include <vector>
#include <string>
#include "Math.h"

enum class SkillPattern
{
    SingleTile,      // Um único tile (padrão atual)
    Explosion,       // Explosão em raio (círculo)
    LineStraight,    // Linha reta na direção
    Cross,           // Padrão de cruz (+)
    Diamond,         // Padrão de diamante
};

struct AffectedTile
{
    int x;
    int y;
};

struct SkillData
{
    std::string name;
    int damage;
    int range;
    int minRange;
    SkillPattern pattern;

    int areaSize;           // Para Explosion, raio do círculo
    int areaWidth;            // Para Line, largura da linha
    std::string description;
    bool isBreakable;            // Se essa habilidade pode quebrar destrutíveis
};

class SkillCalculator
{
public:
    // Calcula quais tiles serão afetados baseado no padrão
    static std::vector<AffectedTile> GetAffectedTiles(
        int originX, int originY,
        int targetX, int targetY,
        const SkillData& skill
    );

private:
    // Padrões individuais
    static std::vector<AffectedTile> PatternSingleTile(int targetX, int targetY);
    static std::vector<AffectedTile> PatternExplosion(int centerX, int centerY, int radius);
    static std::vector<AffectedTile> PatternLineStraight(int originX, int originY, int targetX, int targetY, int length, int width);
    static std::vector<AffectedTile> PatternCross(int centerX, int centerY, int radius);
    static std::vector<AffectedTile> PatternDiamond(int centerX, int centerY, int radius);
    
    // Helpers
    static int ManhattanDistance(int x1, int y1, int x2, int y2);
    static bool IsInCircle(int x, int y, int centerX, int centerY, int radius);
};
