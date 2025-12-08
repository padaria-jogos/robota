#pragma once
#include "../Component.h"
#include "../../Math.h"

class PointLightComponent : public Component
{
public:
    PointLightComponent(class Actor* owner, int updateOrder = 10);
    ~PointLightComponent();

    void Update(float deltaTime) override;

    // Getters
    Vector3 GetPosition() const;
    Vector3 GetColor() const { return mColor; }
    float GetIntensity() const { return mIntensity; }
    float GetRadius() const { return mRadius; }
    bool IsEnabled() const { return mEnabled; }

    // Setters
    void SetColor(const Vector3& color) { mColor = color; }
    void SetIntensity(float intensity) { mIntensity = intensity; }
    void SetRadius(float radius) { mRadius = radius; }
    void SetEnabled(bool enabled) { mEnabled = enabled; }
    void SetOffset(const Vector3& offset) { mOffset = offset; }

private:
    Vector3 mColor;       // Cor RGB
    float mIntensity;
    float mRadius;
    bool mEnabled;
    Vector3 mOffset;      // Offset da posição do owner
};
