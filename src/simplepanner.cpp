#include "simplepanner.h"

#include <math.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

// clipping for float to integer style
//#define _SIGN(N) ((N)>>31L)
//#define _ABS(N) ((N)^_SIGN(N))
//#define _CLIPR(N,R) ((_ABS((N)+(R))-_ABS((N)-((R)+1L)))/2L)
//long out = (long)(myfloat * 32767.0f);
//short outs = (short)(_CLIPR(out, 32767L));

const float PAN_FADE_RATE = (400.0f);	// Rate to fade at (dB per second)

SimplePanner::SimplePanner(QObject *parent) :
    QObject(parent),
    sampleRate(48000),
    panCurrent(0.0f),
    panDesired(0.0f),
    panDesiredByManual(0.0f),
    panDesiredByPanAdjuster(0.0f),
    panAdjustment(false),
    apaAttackRate(20.0f),
    apaDecayRate(1.0f)
{
}

// Properties

//! Sets the current pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  Intended for internal use
  \param value the current pan (-1-+1)
*/
void SimplePanner::setPanCurrent(float value)
{
    if (panCurrent == value)
        return;

    panCurrent = value;
}

//! Gets the current pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  \return the current pan (-1-+1)
*/
float SimplePanner::getPanCurrent() const
{
    return panCurrent;
}

//! Sets the desired pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  Intended for internal use
  \param value the desired pan (-1-+1)
*/
void SimplePanner::setPanDesired(float value)
{
    if (panDesired == value)
        return;

    panDesired = value;
}

//! Gets the desired pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  \return the desired pan (-1-+1)
*/
float SimplePanner::getPanDesired() const
{
    return panDesired;
}

//! Sets the desired pan (-1-+1) of a user interaction
/*!
  Intended for internal use
  \param value the desired Pan of a user interaction (dB)
*/
void SimplePanner::setPanDesiredByManual(float value)
{
    if (panDesiredByManual == value)
        return;

    panDesiredByManual = value;
    setPanDesired(value);
}

//! Gets the desired pan (-1-+1) set by the last user interaction
/*!
  \return the desired pan (-1-+1) of the last user interaction
*/
float SimplePanner::getPanDesiredByManual() const
{
    return panDesiredByManual;
}

//! Sets the desired pan (-1-+1) of an automatic Pan adjustment
/*!
  Intended for internal use
  \param value the desired Pan of an automatic Pan adjustment (dB)
*/
void SimplePanner::setPanDesiredByPanAdjuster(float value)
{
    if (panDesiredByPanAdjuster == value)
        return;

    panDesiredByPanAdjuster = value;
    setPanDesired(value);
}

//! Gets the desired pan (-1-+1) set by the last automatic Pan adjustment
/*!
  \return the desired pan (-1-+1) of the automatic Pan adjustment
*/
float SimplePanner::getPanDesiredByPanAdjuster() const
{
    return panDesiredByPanAdjuster;
}

float SimplePanner::getApaAttackRate() const
{
    return apaAttackRate;
}

float SimplePanner::getApaDecayRate() const
{
    return apaDecayRate;
}

bool SimplePanner::getPanAdjustment() const
{
    return panAdjustment;
}

// Slots

void SimplePanner::setApaAttackRate(float value)
{
    if (apaAttackRate == value)
        return;

    apaAttackRate = value;
    emit ApaAttackRateChanged(value);
}

void SimplePanner::setApaDecayRate(float value)
{
    if (apaDecayRate != value) {
        apaDecayRate = value;
        emit ApaDecayRateChanged(value);
    }
}

void SimplePanner::setPanAdjustment(bool value)
{
    if (panAdjustment == value)
        return;

    panAdjustment = value;
}



// process

//void SimplePanner::process(int nSamples,
//                      float *pleft,
//                      float *pright,
//                      float balance // -1...+1
//                      )
//{
//    float p, gl, gr;

//    p=M_PI*(balance+1)/4;
//    gl=::cos(p);
//    gr=::sin(p);

//    while( nSamples-- )
//    {
//       *pleft =*pleft++ *gl;
//       *pright=*pright++*gr;
//    }
//}

void SimplePanner::process(int nSamples,
                      QList<float> *pleft,
                      QList<float> *pright,
                      float balance // -1...+1
                      )
{
    balance = balance < -1 ? -1 : balance;
    balance = balance > 1 ? 1 : balance;

    float p, gl, gr;

    p=M_PI*(balance+1)/4;
    gl=::cos(p);
    gr=::sin(p);

    for (int i = 0; i<nSamples;++i)
    {
        pleft->replace(i, (pleft->at(i) * gl));
        pright->replace(i, (pright->at(i) * gr));
    }
}

void SimplePanner::process(short *samples, int sampleCount, int channels,int leftChannelNr, int rightChannelNr)
{
    // Determine Pan for current buffer
    float currentPan = getPanCurrent();
    float desiredPan = getPanDesired();
    float desiredPanByManual = getPanDesiredByManual();
    float desiredPanByPanAdjuster = getPanDesiredByPanAdjuster();

    if (desiredPan != desiredPanByManual) {
        if (getPanAdjustment() == true) {
            // is attacking
            float fade_step_down = (apaAttackRate / sampleRate) * sampleCount;
            float fade_step_up = (apaDecayRate / sampleRate) * sampleCount;
            if (currentPan < desiredPanByPanAdjuster - fade_step_up) {
                currentPan += fade_step_up;
            }
            else if (currentPan > desiredPanByPanAdjuster + fade_step_down) {
                currentPan -= fade_step_down;
            }
            else {
                currentPan = desiredPanByPanAdjuster;
            }
        }
        else {
            // is releasing
            float fade_step_gainAdjust = (apaDecayRate / sampleRate) * sampleCount;
            if (currentPan < desiredPanByManual - fade_step_gainAdjust) {
                currentPan += fade_step_gainAdjust;
            }
            else if (currentPan > desiredPanByManual + fade_step_gainAdjust) {
                currentPan -= fade_step_gainAdjust;
            }
            else {
                currentPan = desiredPanByManual;
            }
        }
        emit ChannelPanAdjusted(currentPan);
    }
    else if (currentPan != desiredPanByManual) {
        float fade_step = (PAN_FADE_RATE / sampleRate) * sampleCount;
        if (currentPan < desiredPanByManual - fade_step) {
            currentPan += fade_step;
        }
        else if (currentPan > desiredPanByManual + fade_step) {
            currentPan -= fade_step;
        }
        else {
            currentPan = desiredPanByManual;
        }
    }
    setPanCurrent(currentPan);

    // End Determine Pan for current buffer

    // Extract from interleaved and convert to QList<float>
    QList<float> float_data_left;
    QList<float> float_data_right;
    for(int i=0; i < sampleCount; ++i)
    {
        float_data_left.insert(i,samples[leftChannelNr + (i*channels) ]);
        float_data_right.insert(i,samples[rightChannelNr + (i*channels) ]);
    }

    //don't need to adjust the range to -1 - +1 do we?

    process(sampleCount,&float_data_left,&float_data_right,currentPan);

    // put it back into interleaved and Clip
    for (int i = 0; i < sampleCount; ++i)
    {
        float left = float_data_left.at(i);
        if (left > 32767.f)
            samples[leftChannelNr + (i*channels) ] = 32767;
        else if (left < -32768.f)
            samples[leftChannelNr + (i*channels) ] = -32768;
        else
            samples[leftChannelNr + (i*channels) ] = (short)floor(.5+left);

        float right = float_data_right.at(i);
        if (right > 32767.f)
            samples[rightChannelNr + (i*channels) ] = 32767;
        else if (right < -32768.f)
            samples[rightChannelNr + (i*channels) ] = -32768;
        else
            samples[rightChannelNr + (i*channels) ] = (short)floor(.5+right);
    }
}
