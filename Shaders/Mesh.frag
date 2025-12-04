// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;
uniform vec3 uKeyLightPos;     // Luz principal do mundo
uniform vec3 uFillLightPos;    // Luz lateral esquerda
uniform vec3 uRimLightPos;     // Luz lateral direita
uniform vec3 uCameraPos;       // Posição da câmera
uniform float uMetallic;       // 0.0 = fosco, 1.0 = metálico
uniform bool uUnlit;           // true = sem luz (tiles/cursor)
uniform float uWorldLightIntensity; // Intensidade das luzes do mundo (0.0 a 1.0)

// Pontos de luz
#define MAX_POINT_LIGHTS 8
uniform int uNumPointLights;
uniform vec3 uPointLightPositions[MAX_POINT_LIGHTS];
uniform vec3 uPointLightColors[MAX_POINT_LIGHTS];
uniform float uPointLightIntensities[MAX_POINT_LIGHTS];
uniform float uPointLightRadii[MAX_POINT_LIGHTS];

void main()
{
	// Sample color from texture
    vec4 texColor = texture(uTexture, fragTexCoord);
    
    // Se unlit, retorna apenas a cor da textura sem iluminação
    if (uUnlit) {
        outColor = texColor;
        return;
    }
    
    // Normaliza a normal
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(uCameraPos - fragWorldPos);
    
    // LUZ AMBIENTE (reduzida pela intensidade do mundo)
    vec3 ambient = vec3(0.1, 0.1, 0.12) * (0.2 + uWorldLightIntensity * 0.8);
    
    // LUZ PRINCIPAL (frontal)
    vec3 keyDir = normalize(uKeyLightPos - fragWorldPos);
    float keyDist = length(uKeyLightPos - fragWorldPos);
    float keyAtten = 1.0 / (1.0 + 0.0001 * keyDist);
    float keyDiff = max(dot(normal, keyDir), 0.0);
    vec3 keyLight = vec3(1.0, 1.0, 1.0) * keyDiff * keyAtten;
    
    // LUZ LATERAL ESQUERDA (quente - laranja)
    vec3 fillDir = normalize(uFillLightPos - fragWorldPos);
    float fillDiff = max(dot(normal, fillDir), 0.0);
    vec3 fillLight = vec3(1.0, 0.6, 0.3) * fillDiff * 0.4;
    
    // LUZ LATERAL DIREITA (fria - azul)
    vec3 rimDir = normalize(uRimLightPos - fragWorldPos);
    float rimDiff = max(dot(normal, rimDir), 0.0);
    vec3 rimLight = vec3(0.3, 0.6, 1.0) * rimDiff * 0.4;
    
    // Especular para os  metais
    vec3 reflectDir = reflect(-keyDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(1.0, 1.0, 1.0) * spec * keyAtten * uMetallic;
    
    // Luzes dinamicas robos
    vec3 dynamicLight = vec3(0.0);
    
    for (int i = 0; i < uNumPointLights && i < MAX_POINT_LIGHTS; i++)
    {
        vec3 toLight = uPointLightPositions[i] - fragWorldPos;
        float distance = length(toLight);
        float atten = 1.0 - clamp(distance / uPointLightRadii[i], 0.0, 1.0);
        
        if (atten > 0.01)
        {
            vec3 lightDir = normalize(toLight);
            float diff = max(dot(normal, lightDir), 0.0);
            dynamicLight += uPointLightColors[i] * diff * atten * uPointLightIntensities[i];
        }
    }
    

    // Aplica intensidade do mundo nas luzes fixas
    vec3 worldLights = (keyLight + fillLight + rimLight) * uWorldLightIntensity;
    vec3 litColor = texColor.rgb * (ambient + worldLights + dynamicLight) + specular * 0.5 * uWorldLightIntensity;
    
    // Saturação alta
    float luminance = dot(litColor, vec3(0.299, 0.587, 0.114));
    vec3 saturated = mix(vec3(luminance), litColor, 1.6);
    litColor = saturated * 1.2; // Boost de brilho
    
    // Dithering ps1
    mat4 ditherMatrix = mat4(
        0.0,  8.0,  2.0, 10.0,
        12.0, 4.0, 14.0,  6.0,
        3.0, 11.0,  1.0,  9.0,
        15.0, 7.0, 13.0,  5.0
    ) / 16.0;
    
    vec2 ditherCoord = mod(gl_FragCoord.xy, 3.0);
    int x = int(ditherCoord.x);
    int y = int(ditherCoord.y);
    float ditherValue = ditherMatrix[x][y];
    
    // Dithering sutil para suavizar gradientes
    litColor += (ditherValue - 0.5) * 0.03;
    
    outColor = vec4(litColor, texColor.a);
}