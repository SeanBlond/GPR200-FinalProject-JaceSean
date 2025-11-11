#version 330 core
out vec4 FragColor;  
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

struct Material {
    float ambient;
    float diffuse;
    float specular;    
    float shininess;
}; 

struct Light
{
    vec3 lightPos;
    vec3 lightColor;

};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform int renderOption;

void main()
{
    // Creating the Texture Color 
    vec2 uv = TexCoord;
    vec3 color;
    vec4 face = texture(texture2, uv);
    color = mix(texture(texture1, uv), face, face.a).xyz;

    // Ambient
    vec3 ambient = material.ambient * color;

    // Diffuse
    vec3 lightDir = normalize(light.lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * material.diffuse * color;

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
    }
}