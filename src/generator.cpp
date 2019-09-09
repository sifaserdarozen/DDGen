#include "generator.h"

#include <chrono>
#include <climits>
#include <iostream>
#include <math.h>
#include <random>

namespace ddgen
{

bool ZeroGeneratorType::Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration)
{
    for ( ;size ; --size)
        *pcm_data_ptr++ = 0;

    return true;
}

SingleToneGeneratorType::SingleToneGeneratorType(float amplitude, float frequency, float phase)
{
    // form a seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // introduce generator
    std::minstd_rand generator(seed);

    // check amplitude between 0 to 1.0
    if (( 0 <= amplitude) && ( 1 >= amplitude))
        m_amplitude = amplitude;
    else
    {
        // generate amplitude randomly between 0.2 to 0.8
        std::uniform_real_distribution<float> amplitude_distribution(0.2, 0.8);
        m_amplitude = amplitude_distribution(generator);
    }

    // check frequency to be between 0.2PI to 0.2PI
    if ((((0.2)*PI) < frequency) && (frequency < ((0.8)*PI)))
        m_frequency = frequency;
    else
    {
        // generate frequency between 0.2PI to 0.8PI
        std::uniform_real_distribution<float> frequency_distribution(0.2*PI, 0.8*PI);
        m_frequency = frequency_distribution(generator);
    }

    // check phase to be between -PI to PI
    if ((-PI < phase) && (phase < PI))
        m_phase = phase;
    else
    {
        // normalize PI
        if (phase > PI)
            while (phase > PI)
                phase -= 2*PI;
        else
            while (-PI > phase)
                phase += 2*PI;
        m_phase = phase;
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
    m_amplitude = amplitude_distribution(generator);

    // generate phase between -PI to PI
    std::uniform_real_distribution<float> phase_distribution(-PI, PI);
    m_phase = phase_distribution(generator);

    // generate frequency between 0.2PI to 0.8PI
    std::uniform_real_distribution<float> frequency_distribution(0.2*PI, 0.8*PI);
    m_frequency = frequency_distribution(generator);

    std::cout << __FILE__ << " " << __LINE__ << " Single tone generator with A: " << m_amplitude << " F: " << m_frequency << " P: " << m_phase << std::endl;
}

bool SingleToneGeneratorType::Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration)
{
    if (!pcm_data_ptr)
    {
        std::cerr << __FILE__ << " " << __LINE__ << "pcm_data_ptr is null" << std::endl;
        return false;
    }

    for (; size; --size)
    {
        *pcm_data_ptr++ = (short int)(m_amplitude * SHRT_MAX * sin(m_phase));
        m_phase += m_frequency;
    }

    // normalize phase
    while (m_phase > PI)
        m_phase -= 2*PI;

    return true;
}

bool SinusoidalGeneratorType::Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration)
{
    for ( ;size ; --size)
        *pcm_data_ptr++ = 0;

    return true;
}
}
