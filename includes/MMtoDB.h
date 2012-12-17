#ifndef MMTODB_H
#define MMTODB_H


// Idee: Unter Einbeziehung der Bildschirmgröße, Auflösung usw. sollte sich bei Touchscreen Fader Verhalten quasi original abbilden lassen


//References : Posted by James McCartney
//
//Notes :
//These two functions reproduce a traditional professional
//mixer fader taper.
//
//MMtoDB converts millimeters of fader travel from the
//bottom of the fader for a 100 millimeter fader into
//decibels. DBtoMM is the inverse.
//
//The taper is as follows from the top:
//The top of the fader is +10 dB
//100 mm to 52 mm : -5 dB per 12 mm
//52 mm to 16 mm : -10 dB per 12 mm
//16 mm to 4 mm : -20 dB per 12 mm
//4 mm to 0 mm : fade to zero. (in these functions I go to -200dB
//which is effectively zero for up to 32 bit audio.)

static float MMtoDB(float mm)
{
        float db;

        mm = 100. - mm;

        if (mm <= 0.) {
                db = 10.;
        } else if (mm < 48.) {
                db = 10. - 5./12. * mm;
        } else if (mm < 84.) {
                db = -10. - 10./12. * (mm - 48.);
        } else if (mm < 96.) {
                db = -40. - 20./12. * (mm - 84.);
        } else if (mm < 100.) {
                db = -60. - 35. * (mm - 96.);
        } else db = -200.;
        return db;
}

static float DBtoMM(float db)
{
        float mm;
        if (db >= 10.) {
                mm = 0.;
        } else if (db > -10.) {
                mm = -12./5. * (db - 10.);
        } else if (db > -40.) {
                mm = 48. - 12./10. * (db + 10.);
        } else if (db > -60.) {
                mm = 84. - 12./20. * (db + 40.);
        } else if (db > -200.) {
                mm = 96. - 1./35. * (db + 60.);
        } else mm = 100.;

        mm = 100. - mm;

        return mm;
}

//linear fader
static float MMtoDBFineMode(float mm)
{
    float maxVal = 30;
    float minVal = -90;

    float db;
    db = minVal + (maxVal - minVal) * mm/100.;
    return db;
}

static float DBtoMMFineMode(float db)
{
    float maxVal = 30;
    float minVal = -90;

    float mm;
    mm = (db - minVal) * 100 / (maxVal - minVal);
    return mm;
}

//----------------------------------------------------------------------------
// IEC standard dB scaling -- as borrowed from meterbridge (c) Steve Harris

static inline float IEC_Scale ( float dB )
{
        float fScale = 1.0f;

        if (dB < -70.0f)
                fScale = 0.0f;
        else if (dB < -60.0f)
                fScale = (dB + 70.0f) * 0.0025f;
        else if (dB < -50.0f)
                fScale = (dB + 60.0f) * 0.005f + 0.025f;
        else if (dB < -40.0)
                fScale = (dB + 50.0f) * 0.0075f + 0.075f;
        else if (dB < -30.0f)
                fScale = (dB + 40.0f) * 0.015f + 0.15f;
        else if (dB < -20.0f)
                fScale = (dB + 30.0f) * 0.02f + 0.3f;
        else if (dB < -0.001f || dB > 0.001f)  /* if (dB < 0.0f) */
                fScale = (dB + 20.0f) * 0.025f + 0.5f;

        return fScale;
}

static inline float IEC_dB ( float fScale )
{
        float dB = 0.0f;

        if (fScale < 0.025f)	    // IEC_Scale(-60.0f)
                dB = (fScale / 0.0025f) - 70.0f;
        else if (fScale < 0.075f)	// IEC_Scale(-50.0f)
                dB = (fScale - 0.025f) / 0.005f - 60.0f;
        else if (fScale < 0.15f)	// IEC_Scale(-40.0f)
                dB = (fScale - 0.075f) / 0.0075f - 50.0f;
        else if (fScale < 0.3f)		// IEC_Scale(-30.0f)
                dB = (fScale - 0.15f) / 0.015f - 40.0f;
        else if (fScale < 0.5f)		// IEC_Scale(-20.0f)
                dB = (fScale - 0.3f) / 0.02f - 30.0f;
        else /* if (fScale < 1.0f)	// IED_Scale(0.0f)) */
                dB = (fScale - 0.5f) / 0.025f - 20.0f;

        return (dB > -0.001f && dB < 0.001f ? 0.0f : dB);
}

#endif // MMTODB_H
