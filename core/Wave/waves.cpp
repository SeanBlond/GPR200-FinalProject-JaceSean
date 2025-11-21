#include "waves.h"
using namespace hiWave;

WaveSystem::~WaveSystem()
{

}

// Functions
void WaveSystem::AddRandomWave()
{

	glm::vec2 direction;
	float wavelength;
	float steepness;
	float speed;

	// Generate Random Values
	direction.x = ew::RandomRange(-1.0f, 1.0f);
	direction.y = ew::RandomRange(-1.0f, 1.0f);

	wavelength = ew::RandomRange(0.1f, 5.0f);
	steepness = ew::RandomRange(0.1f, 1.0f);
	speed = ew::RandomRange(-8.0f, 8.0f);

	// Adding WaveSettings
	AddWave(new WaveSettings(direction, wavelength, steepness));
}
void WaveSystem::AddWave(WaveSettings* wave)
{
	if (waveCount < MAX_WAVES - 1)
	{
		waves.push_back(wave);
		waveCount++;
	}
}
void WaveSystem::RemoveWave(int index)
{
	if (waveCount > 0)
	{
		waves.erase(waves.begin() + index);
		waveCount--;
	}
}
void WaveSystem::RemoveAllWaves() {
	int waveAmount = waveCount;
	for (int i = 0; i < waveAmount; i++) {
		RemoveWave(0);
	}
}
void WaveSystem::PassValues(shdr::Shader* shader)
{
	shader->setInt("numWaves", waveCount);

	for (int i = 0; i < waveCount; i++)
	{
		std::string waveSettingName = ("waves[" + std::to_string(i) + "]");
		shader->setVec2((waveSettingName + ".direction"), waves[i]->direction);
		shader->setFloat((waveSettingName + ".wavelength"), waves[i]->wavelength);

		// Modifying the steepness of the wave;
		float modifiedSteepness = waves[i]->steepness;
		if (decreaseWaves)
			modifiedSteepness *= pow(0.65f, i);

		shader->setFloat((waveSettingName + ".steepness"), modifiedSteepness);
	}
}