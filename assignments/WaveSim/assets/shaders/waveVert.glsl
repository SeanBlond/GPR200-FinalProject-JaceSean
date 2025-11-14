#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

struct Wave
{

    float time;

    vec2 direction;

    float waveLength;
    float steepness;
    float amplitude;
    float speed;

    //int detail;

};

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Wave hiWave;

void main()
{

    vec3 normalnormal = aNormal;
    vec3 total = vec3(aPos.x, 0.0f, aPos.z);
    for (int i = 0; i < 1; i ++)
    {
        float a, w, s;
        a = hiWave.amplitude / (i*i);
        w = hiWave.waveLength * i;
        s = hiWave.speed * i;

        float WA = w*a;

        float waveBye = dot(w*hiWave.direction,total.xy + s*hiWave.time);
        float waveCya = dot(w*hiWave.direction,total.xy + s*hiWave.time);

        total.x += (hiWave.steepness * a * hiWave.direction.x) * cos(waveBye);
        total.y += a*sin(waveBye);
        total.z += (hiWave.steepness * a * hiWave.direction.y) * cos(waveBye);

        normalnormal.x += (hiWave.direction.x * WA) * cos(waveCya);
        normalnormal.y += (hiWave.steepness * WA) * sin(waveCya);
        normalnormal.z += (hiWave.direction.y * WA) * cos(waveCya);

    }

    normalnormal.x *= -1;
    normalnormal.y = 1 -normalnormal.y;
    normalnormal.z *= -1;

    gl_Position = projection * view * model * vec4(total, 1);
    FragPos = vec3(model * vec4(total, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

}