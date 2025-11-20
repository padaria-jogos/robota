// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// Base solid color
uniform vec4 uBaseColor;

// This is used for texture blending
uniform float uTextureFactor;

void main()
{
    // Sample color from texture
    vec4 texColor = texture(uTexture, fragTexCoord);

    // Blend texture color with white based on factor
    outColor = mix(uBaseColor, texColor, uTextureFactor);
}
