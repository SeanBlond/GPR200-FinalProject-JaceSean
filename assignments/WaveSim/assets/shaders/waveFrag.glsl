#version 330 core
out vec4 FragColor;  
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
in float Depth;

struct Material {
    float ambient;
    float diffuse;
    float specular;    
    float shininess;
}; 

struct Light
{
    vec3 lightDirection;
    vec3 lightColor;

};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform vec3 colorWave;
uniform vec3 foamColor;
uniform float foamLimit;
uniform int renderOption;

void main()
{
    // Creating the Texture Color 
    vec2 uv = TexCoord;

    // Color Changing
    vec3 normal = normalize(Normal);
    vec3 waveDeepColor = mix(colorWave*vec3(0.25), colorWave, Depth);
    float foamAmount = 2.0 * max((Depth - foamLimit)/ (1.0 - foamLimit), 0.0);
    vec3 waveColor = mix(waveDeepColor, colorWave, Depth);
    waveColor = mix(waveColor, foamColor, foamAmount);

    // Ambient
    vec3 ambient = material.ambient * waveColor;

    // Diffuse
    vec3 lightDir = normalize(light.lightDirection);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * material.diffuse * waveColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.lightColor * spec * material.specular;

    if (renderOption == 0) // Rendering UVs
    {
        FragColor = vec4(uv, 0.0, 1.0);
    }
    else if (renderOption == 1) // Rendering Normals
    {
        FragColor = vec4(normal, 1.0);
    }
    else // Rendering Shaded Color
    {
        FragColor = vec4(ambient + diffuse + specular, 1.0);
        //FragColor = vec4(waveColor, 1.0);
    }
}