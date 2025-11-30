#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// Particle color tint
uniform vec4 uParticleColor;

// Alpha multiplier for fade effects
uniform float uAlpha;

void main()
{
    // Sample color from texture
    vec4 texColor = texture(uTexture, fragTexCoord);

    // Multiply texture by particle color
    vec4 tintedColor = texColor * uParticleColor;

    // Apply alpha multiplier
    tintedColor.a *= uAlpha;

    outColor = tintedColor;
}