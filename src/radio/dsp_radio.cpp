#include "dsp_radio.h"

#include <QVarLengthArray>

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif
const double TWO_PI_OVER_SAMPLE_RATE = 2*M_PI/48000;

DspRadio::DspRadio(QObject *parent) :
    QObject(parent)
{
    f_m = new Dsp::SmoothedFilterDesign
            <Dsp::Butterworth::Design::BandPass <4>, 1, Dsp::DirectFormII> (1024);
    f_s = new Dsp::SmoothedFilterDesign
          <Dsp::Butterworth::Design::BandPass <4>, 2, Dsp::DirectFormII> (1024);
//    f_m = new Dsp::SmoothedFilterDesign
//            <Dsp::RBJ::Design::BandPass2, 1, Dsp::DirectFormII> (1024);
//    f_s = new Dsp::SmoothedFilterDesign
//          <Dsp::RBJ::Design::BandPass2, 2> (1024);
    f_m_o = new Dsp::SmoothedFilterDesign
            <Dsp::Butterworth::Design::BandPass <4>, 1, Dsp::DirectFormII> (1024);
    f_s_o = new Dsp::SmoothedFilterDesign
          <Dsp::Butterworth::Design::BandPass <4>, 2, Dsp::DirectFormII> (1024);

    Dsp::Params params;
    params[0] = 48000; // sample rate
    params[1] = 4; // order
    params[2] = 1600; // center frequency
    params[3] = 1300; // band width
    f_m->setParams (params);
    f_s->setParams (params);
    f_m_o->setParams (params);
    f_s_o->setParams (params);
}

void DspRadio::setEnabled(QString name, bool val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (m_Enabled == val)
            return;
        m_Enabled = val;
    }
}

void DspRadio::setFudge(QString name, double val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (m_Fudge == val)
            return;

        m_Fudge = val;
        emit fudgeChanged(m_Fudge);
    }
}

void DspRadio::setBandpassEqInCenterFrequency(QString name, double val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (f_m != NULL)
            f_m->setParam(2,val);
        if (f_s != NULL)
            f_s->setParam(2,val);

        emit bandpassEqInCenterFrequencyChanged(val);
    }
}

void DspRadio::setBandpassEqInBandWidth(QString name, double val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (f_m != NULL)
            f_m->setParam(3,val);
        if (f_s != NULL)
            f_s->setParam(3,val);

        emit bandpassEqInBandWidthChanged(val);
    }
}

void DspRadio::setRmModFreq(QString name, double val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (m_RM_modFreq == val)
            return;

        m_RM_modFreq = val;

        emit ringModFrequencyChanged(val);
    }
}

void DspRadio::setRmMix(QString name, double val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (m_RM_mix == val)
            return;

        m_RM_mix = val;

        emit ringModMixChanged(val);
    }
}

void DspRadio::setBandpassEqOutCenterFrequency(QString name, double val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (f_m_o != NULL)
            f_m_o->setParam(2,val);
        if (f_s_o != NULL)
            f_s_o->setParam(2,val);

        emit bandpassEqOutCenterFrequencyChanged(val);
    }
}

void DspRadio::setBandpassEqOutBandWidth(QString name, double val)
{
    if (name.isEmpty() || (name == m_ChannelType))
    {
        if (f_m_o != NULL)
            f_m_o->setParam(3,val);
        if (f_s_o != NULL)
            f_s_o->setParam(3,val);

        emit bandpassEqOutBandWidthChanged(val);
    }
}

void DspRadio::DoProcess(float *samples, int sampleCount, float &volFollow)
{
    // ALL INPUTS AND OUTPUTS IN THIS ARE -1.0f and +1.0f
    // Find volume of current block of frames...
    float vol = 0.0f;
 //   float min = 1.0f, max = -1.0f;
    for (int i = 0; i < sampleCount; i++)
    {
       vol += (samples[i] * samples[i]);
    }
    vol /= (float)sampleCount;

    // Fudge factor, inrease for more noise
    vol *= (float)getFudge();

    // Smooth follow from last frame, both multiplies add up to 1...
    volFollow = volFollow * 0.5f + vol * 0.5f;

    // Between -1.0f and 1.0f...
    float random = (((float)(rand()&32767)) / 16384.0f) - 1.0f;

    // Between 1 and 128...
    int count = (rand() & 127) + 1;
    float temp;
    for (int i = 0; i < sampleCount; i++)
    {
       if (!count--)
       {
//          // Between -1.0f and 1.0f...
          random = (((float)(rand()&32767)) / 16384.0f) - 1.0f;
//          // Between 1 and 128...
          count = (rand() & 127) + 1;
       }
        // Add random to inputs * by current volume;
       temp = samples[i] + random * volFollow;

       // Make it an integer between -60 and 60
       temp = (int)(temp * 40.0f);

       // Drop it back down but massively quantised and too high
       temp = (temp / 40.0f);
       temp *= 0.05 * (float)getFudge();
       temp += samples[i] * (1 - (0.05 * (float)getFudge()));
       samples[i] = qBound(-1.0f,temp,1.0f);
    }
}

void DspRadio::DoProcessRingMod(float *samples, int sampleCount, double &modAngle)
{
    if ((m_RM_modFreq != 0.0f) && (m_RM_mix != 0.0f))   // RingMod
    {
        for(int i=0; i<sampleCount; ++i)
        {
//            samples[i] *= sin(m_RM_modAngle);
            float sample = samples[i];
            sample = (sample * (1-m_RM_mix)) + (m_RM_mix * (sample * sin(modAngle)));
            samples[i] = qBound(-1.0f,sample,1.0f);
            modAngle += m_RM_modFreq * TWO_PI_OVER_SAMPLE_RATE;
        }
    }
}

void DspRadio::Process(short *samples, int sampleCount, int channels)
{
//    Dsp::Filter* f = (channels==1)?f_m:f_s;

    if (!getEnabled())
        return;

    if (channels == 1)
    {
        QVarLengthArray<float,480> data(sampleCount); //Test has been 480
        for(int i=0; i<sampleCount; ++i)
            data[i] = samples[i] / 32768.f;

        float* audioData[1];
        audioData[0] = data.data();
        f_m->process(sampleCount,audioData);

        DoProcessRingMod(data.data(), sampleCount, m_RM_modAngle);

        if (getFudge() > 0.0f)
            DoProcess(data.data(),sampleCount, m_volFollow);

        f_m_o->process(sampleCount,audioData);

        for(int i=0; i<sampleCount; ++i)
            samples[i] = (short)(data[i] * 32768.f);

//        delete[] *audioData;      // release memory
    }
    else if (channels == 2)
    {
        // Extract from interleaved and convert to QVarLengthArray<float>
        QVarLengthArray<float,480> c_data_left(sampleCount);
        QVarLengthArray<float,480> c_data_right(sampleCount);
        for(int i=0; i<sampleCount; ++i)
        {
            c_data_left[i] = samples[i*2] / 32768.f;
            c_data_right[i] = samples[1 + (i*2)] / 32768.f;
        }

        float* audioData[2];
        audioData[0] = c_data_left.data();
        audioData[1] = c_data_right.data();
        f_s->process(sampleCount,audioData);

        DoProcessRingMod(c_data_left.data(), sampleCount, m_RM_modAngle);
        DoProcessRingMod(c_data_right.data(), sampleCount, m_RM_modAngle_r);

        if (getFudge() > 0.0f)
        {
            DoProcess(c_data_left.data(),sampleCount, m_volFollow);
            DoProcess(c_data_right.data(),sampleCount, m_volFollow_r);
        }

        f_s_o->process(sampleCount,audioData);

        for(int i=0; i<sampleCount; ++i)
        {
            samples[i*2] = (short)(c_data_left[i] * 32768.f);
            samples[1 + (i*2)] = (short)(c_data_right[i] * 32768.f);
        }
//        delete[] *audioData;      // release memory
    }
}

void DspRadio::setChannelType(QString name)
{
    m_ChannelType = name;
}

float DspRadio::getFudge() const
{
    return m_Fudge;
}

double DspRadio::getBandpassEqInCenterFrequency() const
{
    if (f_m != NULL)
        return f_m->getParam(2);
    else if (f_s != NULL)
        return f_s->getParam(2);
    else
        return -1;
}

double DspRadio::getBandpassEqInBandWidth() const
{
    if (f_m != NULL)
        return f_m->getParam(3);
    else if (f_s != NULL)
        return f_s->getParam(3);
    else
        return -1;
}

double DspRadio::getRmModFreq() const
{
    return m_RM_modFreq;
}

double DspRadio::getBandpassEqOutCenterFrequency() const
{
    if (f_m_o != NULL)
        return f_m_o->getParam(2);
    else if (f_s_o != NULL)
        return f_s_o->getParam(2);
    else
        return -1;
}

double DspRadio::getBandpassEqOutBandWidth() const
{
    if (f_m_o != NULL)
        return f_m_o->getParam(3);
    else if (f_s_o != NULL)
        return f_s_o->getParam(3);
    else
        return -1;
}
