/*
 *  Copyright 2009,2010,2011 Reality Jockey, Ltd.
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

#include "MessageAbsoluteValue.h"

MessageObject *MessageAbsoluteValue::newObject(PdMessage *initMessage, PdGraph *graph) {
  return new MessageAbsoluteValue(initMessage, graph);
}

MessageAbsoluteValue::MessageAbsoluteValue(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 1, graph) {
  // nothing to do
}

MessageAbsoluteValue::~MessageAbsoluteValue() {
  // nothing to do
}

void MessageAbsoluteValue::processMessage(int inletIndex, PdMessage *message) {
  if (message->isFloat(0)) {
    PdMessage outgoingMessage(1);
    outgoingMessage.initWithTimestampAndFloat(message->getTimestamp(), fabsf(message->getFloat(0)));
    sendMessage(0, &outgoingMessage);
  }
}
