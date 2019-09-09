/**
 * @file
 * @brief waveform generator and corresponding factory's
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

namespace ddgen
{

#define PI 3.1416    /**< value of pi in radians */


/**
 * @brief Abstract generator interface
 *
 * GeneratorEncoder interface
 * @see SingleToneGeneratorType()
 * @see SinusoidalGeneratorType()
 */
class GeneratorType
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    GeneratorType() {}

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~GeneratorType() {}

    /**
     * @brief Pure virtual interface for generating waveform
     *
     * Calling method should supply at least size of space in pcm_data_ptr.
     * Notice that sampling frequency may be found to be (size / duration ) kHz
     * @param pcm_data_ptr OUTPUT pointer to output pcm data that will be generated, should contain size of data
     * @param size INPUT size, in terms of sample, of generated waveform.
     * @param duration INPUT duration,in terms of ms of waveform.
     * @return indicates success of generation
     */
    virtual bool Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration = 0) = 0;
};

/**
 * @brief ZeroGenerator realization
 *
 * Zero generator, which will return all zeros
 * @see GeneratorType()
 * @see SingleToneGeneratorType()
 * @see SinusoidalGeneratorType()
 */
class ZeroGeneratorType : public GeneratorType
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    ZeroGeneratorType() {}

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~ZeroGeneratorType() {}

    /**
     * @brief Generate waveform
     *
     * Calling method should supply at least size of space in pcm_data_ptr.
     * Zero samples are filled at pcm_data_ptr
     * @param pcm_data_ptr OUTPUT pointer to output pcm data that will be generated, should contain size of data
     * @param size INPUT size, in terms of sample, of generated waveform.
     * @param duration INPUT duration,in terms of ms of waveform.
     * @return indicates success of generation
     */
    virtual bool Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration = 0);
};

/**
 * @brief SingleToneGenerator realization
 *
 * Single tone generator
 * @see GeneratorType()
 * @see ZeroGeneratorType()
 * @see SinusoidalGeneratorType()
 */
class SingleToneGeneratorType : public GeneratorType
{
private:
    float m_amplitude;    /** < amplitude of tone 0, < m_amplitude < 1 */
    float m_frequency;    /**< frequency in radians of tone */
    float m_phase;    /**< phase in radians of tone */

public:
    /**
     * @brief Default constructor, that does let  constructor determine tone parameters
     */
    SingleToneGeneratorType();

    /**
     * @brief Constructor that specify tone parameters explicitly.
     */
    SingleToneGeneratorType(float amplitude, float frequency, float phase);

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~SingleToneGeneratorType() {}

    /**
     * @brief Generate waveform
     *
     * Calling method should supply at least size of space in pcm_data_ptr.
     * Sinusoidal samples are generated with the following formula;
     * Notice that at the end of operation phase will be changed to be:
     * @param pcm_data_ptr OUTPUT pointer to output pcm data that will be generated, should contain size of data
     * @param size INPUT size, in terms of sample, of generated waveform.
     * @param duration INPUT duration,in terms of ms of waveform.
     * @return indicates success of generation
     */
    virtual bool Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration = 0);
};

/**
 * @brief SingleToneGenerator realization
 *
 * Sinusoidal generator
 * @see GeneratorType()
 * @see ZeroGeneratorType()
 * @see SingleToneGeneratorType()
 */
class SinusoidalGeneratorType : public GeneratorType
{
private:


public:
    /**
     * @brief Default constructor, that does let  constructor determine tone parameters
     */
    SinusoidalGeneratorType() {}

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~SinusoidalGeneratorType() {}

    /**
     * @brief Generate waveform
     *
     * Calling method should supply at least size of space in pcm_data_ptr.
     * Sinusoidal samples are generated with the following formula;
     * Notice that at the end of operation phase will be changed to be:
     * @param pcm_data_ptr OUTPUT pointer to output pcm data that will be generated, should contain size of data
     * @param size INPUT size, in terms of sample, of generated waveform.
     * @param duration INPUT duration,in terms of ms of waveform.
     * @return indicates success of generation
     */
    virtual bool Generate(short int* pcm_data_ptr, unsigned short int size, unsigned short int duration = 0);
};

/**
 * @brief Abstract generator factory interface
 *
 * Generator factory interface
 * @see ZeroGeneratorFactory()
 * @see SingleToneGeneratorFactory()
 * @see SinusoidalGeneratorFactory()
 */
class GeneratorFactory
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    GeneratorFactory() {}

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~GeneratorFactory() {}

    /**
     * @brief pure virtual interface for generator creating
     *
     * @return Created generator
     */
    virtual GeneratorType* CreateGenerator() const = 0;
};

class ZeroGeneratorFactory : public GeneratorFactory
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    ZeroGeneratorFactory() { }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~ZeroGeneratorFactory() {}

    /**
     * @brief Implementation of zero generator creation
     *
     * @return ZeroGeneratorType is created and returned to calling object
     * @see GeneratorType()
     * @see SingleToneGeneratorType()
     * @see SinusoidalGeneratorType()
     */
    virtual GeneratorType* CreateGenerator() const { return new ZeroGeneratorType(); }
};

class SingleToneGeneratorFactory : public GeneratorFactory
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    SingleToneGeneratorFactory() { }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~SingleToneGeneratorFactory() {}

    /**
     * @brief Implementation of single tone generator creation
     *
     * @return SingleToneGeneratorType is created and returned to calling object
     * @see GeneratorType()
     * @see ZeroGeneratorType()
     * @see SinusoidalGeneratorType()
     */
    virtual GeneratorType* CreateGenerator() const { return new SingleToneGeneratorType(); }
};

class SinusoidalGeneratorFactory : public GeneratorFactory
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    SinusoidalGeneratorFactory() { }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~SinusoidalGeneratorFactory() {}

    /**
     * @brief Implementation of sinusoidal generator creation
     *
     * @return SinusoidalGeneratorType is created and returned to calling object
     * @see GeneratorType()
     * @see ZeroGeneratorType()
     * @see SingleToneGeneratorType()
     */
    virtual GeneratorType* CreateGenerator() const { return new SinusoidalGeneratorType(); }
};
}
