/*
 *  Copyright 2009,2010,2011,2012 Reality Jockey, Ltd.
 *                 info@rjdj.me
 *                 http://rjdj.me/
 * 
 *  This file is part of ZenGarden.
 *
 *  ZenGarden is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ZenGarden is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ZenGarden.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "DspOsc.h"
#include "PdGraph.h"

/*
 * This code makes use of selections from Pure Data source.
 * See https://github.com/pure-data/pure-data/blob/master/LICENSE.txt.
 *
 * Copyright (c) 1997-1999 Miller Puckette.
 */

#define COSTABSIZE 32768
#define UNITBIT32 1572864.  /* 3*2^19; bit 32 has place value 1 */

#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__FreeBSD_kernel__) \
    || defined(__OpenBSD__)
#include <machine/endian.h>
#endif

#if defined(__linux__) || defined(__CYGWIN__) || defined(__GNU__) || \
    defined(ANDROID)
#include <endian.h>
#endif

#ifdef __MINGW32__
#include <sys/param.h>
#endif

#ifdef _MSC_VER
/* _MSVC lacks BYTE_ORDER and LITTLE_ENDIAN */
#define LITTLE_ENDIAN 0x0001
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#if !defined(BYTE_ORDER) || !defined(LITTLE_ENDIAN)
#error No byte order defined
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
# define HIOFFSET 1
# define LOWOFFSET 0
#else
# define HIOFFSET 0    /* word offset to find MSB */
# define LOWOFFSET 1    /* word offset to find LSB */
#endif

union tabfudge
{
    double tf_d;
    int32_t tf_i[2];
};

// initialise the static class variables
float *DspOsc::cos_table = NULL;
int DspOsc::refCount = 0;

MessageObject *DspOsc::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new DspOsc(initMessage, graph);
}

DspOsc::DspOsc(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  frequency = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
  phase = 0.0;
  refCount++;

  if (cos_table == NULL) {
    cos_table = ALLOC_ALIGNED_BUFFER(COSTABSIZE * sizeof(float));
    for (int i = 0; i < COSTABSIZE; i++) {
      cos_table[i] = cosf(2.0f * M_PI * ((float) i) / COSTABSIZE);
    }
  }
  
  processFunction = &processScalar;
}

DspOsc::~DspOsc() {
  if (--refCount == 0) {
    FREE_ALIGNED_BUFFER(cos_table);
    cos_table = NULL;
  }
}

void DspOsc::onInletConnectionUpdate(unsigned int inletIndex) {
  processFunction = !incomingDspConnections[0].empty() ? &processSignal : &processScalar;
}

string DspOsc::toString() {
  char str[snprintf(NULL, 0, "%s %g", getObjectLabel(), frequency)+1];
  snprintf(str, sizeof(str), "%s %g", getObjectLabel(), frequency);
  return string(str);
}

void DspOsc::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: { // update the frequency
      if (message->isFloat(0)) {
        frequency = fabsf(message->getFloat(0));
      }
      break;
    }
    case 1: { // update the phase
      // TODO
      break;
    }
    default: break;
  }
}

void DspOsc::processSignal(DspObject *dspObject, int fromIndex, int toIndex) {
    DspOsc *d = reinterpret_cast<DspOsc *>(dspObject);
    float multiplier = (float)COSTABSIZE / d->graph->getSampleRate();
    float *input = d->dspBufferAtInlet[0];
    float *output = d->dspBufferAtOutlet[0];
    double phase = (double)d->phase + UNITBIT32;
    int normhipart;
    union tabfudge tf;

    tf.tf_d = UNITBIT32;
    normhipart = tf.tf_i[HIOFFSET];

    for (int i = fromIndex; i < toIndex; i++) {
        tf.tf_d = phase;
        phase += input[i] * multiplier;
        float *addr = DspOsc::cos_table + (tf.tf_i[HIOFFSET] & (COSTABSIZE - 1));
        tf.tf_i[HIOFFSET] = normhipart;
        float frac = tf.tf_d - UNITBIT32;
        float f1 = addr[0];
        float f2 = addr[1];
        output[i] = f1 + frac * (f2 - f1);
    }

    tf.tf_d = UNITBIT32 * COSTABSIZE;
    normhipart = tf.tf_i[HIOFFSET];
    tf.tf_d = phase + (UNITBIT32 * COSTABSIZE - UNITBIT32);
    tf.tf_i[HIOFFSET] = normhipart;
    d->phase = tf.tf_d - UNITBIT32 * COSTABSIZE;
}

void DspOsc::processScalar(DspObject *dspObject, int fromIndex, int toIndex) {
  DspOsc *d = reinterpret_cast<DspOsc *>(dspObject);
  float multiplier = (float)COSTABSIZE / d->graph->getSampleRate();
  float *output = d->dspBufferAtOutlet[0]+fromIndex;
  double frequency = d->frequency;
  double phase = (double)d->phase + UNITBIT32;
  int normhipart;
  union tabfudge tf;

  tf.tf_d = UNITBIT32;
  normhipart = tf.tf_i[HIOFFSET];

  for (int i = fromIndex; i < toIndex; i++) {
      tf.tf_d = phase;
      phase += frequency * multiplier;
      float *addr = DspOsc::cos_table + (tf.tf_i[HIOFFSET] & (COSTABSIZE - 1));
      tf.tf_i[HIOFFSET] = normhipart;
      float frac = tf.tf_d - UNITBIT32;
      float f1 = addr[0];
      float f2 = addr[1];
      output[i] = f1 + frac * (f2 - f1);
  }

  tf.tf_d = UNITBIT32 * COSTABSIZE;
  normhipart = tf.tf_i[HIOFFSET];
  tf.tf_d = phase + (UNITBIT32 * COSTABSIZE - UNITBIT32);
  tf.tf_i[HIOFFSET] = normhipart;
  d->phase = tf.tf_d - UNITBIT32 * COSTABSIZE;
}
