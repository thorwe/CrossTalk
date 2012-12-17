#ifndef DB_H
#define DB_H

#include <math.h>
/*
 *  db.h
 *
 *  Copyright (C) 2003,2005 Steve Harris, Nicholas Humfrey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Originally Stolen from JAMIN <http://jamin.sf.net/>
 */


static inline float
db2lin( float db )
{
        if (db <= -200.0f) return 0.0f;
        else {
                return powf(10.0f, db * 0.05f);
        }
}

static inline float
lin2db( float lin )
{
        if (lin == 0.0f) return -200.0f;
        else return (20.0f * log10f(lin));
}



static inline float
db2lin_alt(float db)
{
    if (db <= -200.0f) return 0.0f;
    else return powf(10, (5.000000e-02f * db));
}

static inline float
db2lin_alt2(float db)
{
    if (db <= -200.0f) return 0.0f;
    else return exp(db/20  * log(10.0f));   // went mad with ambigous call with 10 (identified as int)
}

#endif // DB_H
