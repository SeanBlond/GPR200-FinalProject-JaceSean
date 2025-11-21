#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

struct Wave
{
    vec2 direction;
    float wavelength;
    float steepness;
};

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int numWaves;
uniform Wave waves[10];

const float PI = 3.14159265; 

void main()
{

    vec3 total = vec3(aPos.x, 0.0, aPos.z);
    vec3 tangent = vec3(1, 0, 0);
    vec3 binormal = vec3(0, 0, 1);
    for (int i = 0; i < numWaves; i++)
    {
        // Calculating Relative Variables for every iteration
        float a, w, s, k;
        w = waves[i].wavelength;
        k = (2 * PI) / w;
        s = sqrt(9.8f / k);
        a = (waves[i].steepness / k);
        vec2 d = normalize(waves[i].direction);
        
        // Calculating the Input that will go into the wave functions
        float waveInput = k * (dot(d, aPos.xz) - (s * time));

        // Calculating the position modification of a vertice for a certain iteration
        total.x += a * d.x * cos(waveInput);
        total.y += a * sin(waveInput);
        total.z += a * d.y * cos(waveInput);
        
        // Calculating the Tangent for each iteration
        tangent += vec3(
            -d.x * d.x * (waves[i].steepness * sin(waveInput)),
            d.x * (waves[i].steepness * cos(waveInput)),
            -d.x * d.y * (waves[i].steepness * sin(waveInput))
        );
        
        // Calculating the Binormal for each iteration
        binormal += vec3(
            -d.x * d.y * (waves[i].steepness * sin(waveInput)),
            d.y * (waves[i].steepness * cos(waveInput)),
            -d.y * d.y * (waves[i].steepness * sin(waveInput))
        );
    }

    // Calculating the normal
    vec3 waveNormal = normalize(cross(binormal, tangent));


    gl_Position = projection * view * model * vec4(total, 1);
    FragPos = vec3(model * vec4(total, 1.0));
    Normal = mat3(transpose(inverse(model))) * waveNormal;
    TexCoord = aTexCoord;

}