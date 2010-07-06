/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
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

#ifndef _DSP_TABLE_PLAY_H_
#define _DSP_TABLE_PLAY_H_

#include "TableReceiver.h"

/** [tabplay~] */
class DspTablePlay : public TableReceiver {
  
  public:
    DspTablePlay(PdMessage *initMessage, PdGraph *graph);
    ~DspTablePlay();
  
    const char *getObjectLabel();
  
    ConnectionType getConnectionType(int outletIndex);
  
    void sendMessage(int outletIndex, PdMessage *message);
    
  private:
    void processMessage(int inletIndex, PdMessage *message);
    void processDspToIndex(float blockIndex);
  
    /**
     * Sets up outgoing message message and other conditions to play the table from the given start
     * to end sample indicies. The entire sample is played if <code>endIndex</code> is -1.
     */
    void playTable(int startIndex, int duration);
  
    // the message which is scheduled to be issues when the sample finishes playing
    PdMessage *outgoingMessage;
    float *localDspBufferAtOutletReserved;
    int currentTableIndex; // the current index 
    int endTableIndex; // the index to play to
};

#endif // _DSP_TABLE_PLAY_H_
