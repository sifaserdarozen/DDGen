#include "generator.h"

#include <chrono>
#include <climits>
#include <iostream>
#include <math.h>
#include <random>

namespace ddgen {

bool ZeroGeneratorType::Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration)
{
    for (; size; --size)
        *pcm_data_ptr++ = 0;

    return true;
}

std::vector<CallParameters::StreamParameters::ToneParameters> ZeroGeneratorType::GetParameters() const
{
    return { CallParameters::StreamParameters::ToneParameters{} };
}

SingleToneGeneratorType::SingleToneGeneratorType(float amplitude, float frequency, float phase)
{
    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);

    // check amplitude between 0 to 1.0
    if ((0 <= amplitude) && (1 >= amplitude))
        _generatorParams.amplitude = amplitude;
    else {
        // generate amplitude randomly between 0.2 to 0.8
        std::uniform_real_distribution<float> amplitude_distribution(0.2, 0.8);
        _generatorParams.amplitude = amplitude_distribution(generator);
    }

    // check frequency to be between 0.2PI to 0.2PI
    if ((((0.2) * PI) < frequency) && (frequency < ((0.8) * PI)))
        _generatorParams.frequency = frequency;
    else {
        // generate frequency between 0.2PI to 0.8PI
        std::uniform_real_distribution<float> frequency_distribution(0.2 * PI, 0.8 * PI);
        _generatorParams.frequency = frequency_distribution(generator);
    }

    // check phase to be between -PI to PI
    if ((-PI < phase) && (phase < PI))
        _generatorParams.phase = phase;
    else {
        // normalize PI
        if (phase > PI)
            while (phase > PI)
                phase -= 2 * PI;
        else
            while (-PI > phase)
                phase += 2 * PI;
        _generatorParams.phase = phase;
    }
}

SingleToneGeneratorType::SingleToneGeneratorType()
{
    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);

    // generate amplitude between 0.2 to 0.8
    std::uniform_real_distribution<float> amplitude_distribution(0.2, 0.8);
    _generatorParams.amplitude = amplitude_distribution(generator);

    // generate phase between -PI to PI
    std::uniform_real_distribution<float> phase_distribution(-PI, PI);
    _generatorParams.phase = phase_distribution(generator);

    // generate frequency between 0.2PI to 0.8PI
    std::uniform_real_distribution<float> frequency_distribution(0.2 * PI, 0.8 * PI);
    _generatorParams.frequency = frequency_distribution(generator);
}

bool SingleToneGeneratorType::Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration)
{
    if (!pcm_data_ptr) {
        std::cerr << __FILE__ << " " << __LINE__ << "pcm_data_ptr is null" << std::endl;
        return false;
    }

    for (; size; --size) {
        *pcm_data_ptr++ = (short int)(_generatorParams.amplitude * SHRT_MAX * sin(_generatorParams.phase));
        _generatorParams.phase += _generatorParams.frequency;
    }

    // normalize phase
    while (_generatorParams.phase > PI)
        _generatorParams.phase -= 2 * PI;

    return true;
}

std::vector<CallParameters::StreamParameters::ToneParameters> SingleToneGeneratorType::GetParameters() const
{
    return { _generatorParams };
}

bool SinusoidalGeneratorType::Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration)
{
    for (; size; --size)
        *pcm_data_ptr++ = 0;

    return true;
}

std::vector<CallParameters::StreamParameters::ToneParameters> SinusoidalGeneratorType::GetParameters() const
{
    return { CallParameters::StreamParameters::ToneParameters{} };
}
} // namespace ddgen
