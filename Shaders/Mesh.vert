// Request GLSL 3.3
#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Attribute 0 is position, 1 is normal, 2 is tex coords.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
	// Convert position to homogeneous coordinates
	vec4 pos = vec4(inPosition, 1.0);

	// Transform to world space
	vec4 worldPos = uWorldTransform * pos;
	fragWorldPos = worldPos.xyz;
	
	// Transform to clip space
	vec4 clipPos = uViewProj * worldPos;
	
	// VERTEX SNAPPING/JITTERING PS1
	// Reduz a precisão dos vértices para simular a baixa precisão do PS1
	float snapValue = 200.0; // Quanto maior, mais pixelado
	clipPos.xyz = clipPos.xyz / clipPos.w; // Normaliza para NDC
	clipPos.xy = floor(clipPos.xy * snapValue) / snapValue; // Snapping
	clipPos.xyz *= clipPos.w; // Volta para clip space
	
	gl_Position = clipPos;

	// Transform normal to world space
	fragNormal = mat3(uWorldTransform) * inNormal;
	
	// Pass along the texture coordinate to frag shader
	fragTexCoord = inTexCoord;
}
