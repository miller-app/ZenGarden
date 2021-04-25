/*
 *  Copyright 2009,2011 Reality Jockey, Ltd.
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

#include "MessageFloat.h"

MessageObject *MessageFloat::newObject(PdMessage *initMessage, PdGraph *graph) {
    return new MessageFloat(initMessage, graph);
}

MessageFloat::MessageFloat(PdMessage *initMessage, PdGraph *graph)
    : MessageObject(2, 1, graph) {
    constant = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
}

MessageFloat::~MessageFloat() {
    // nothing to do
}

void MessageFloat::processMessage(int inletIndex, PdMessage *message) {
    switch (inletIndex) {
    case 0: {
        switch (message->getType(0)) {
        case FLOAT: {
            constant = message->getFloat(0);
            // allow fallthrough
        }
        case BANG: {
            PdMessage *outgoingMessage = PD_MESSAGE_ON_STACK(1);
            outgoingMessage->initWithTimestampAndFloat(message->getTimestamp(),
                                                       constant);
            sendMessage(0, outgoingMessage);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 1: {
        if (message->isFloat(0)) {
            constant = message->getFloat(0);
        }
        break;
    }
    default:
        break;
    }
}
