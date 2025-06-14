/*
 * Copyright (c) 2011-2016, Matthieu FAESSEL and ARMINES
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DSIGNAL_H
#define _DSIGNAL_H

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#include "DCommon.h"
#include "Core/include/DSlot.h"

namespace smil {
  class BaseObject;

  class Event {
  public:
    Event(BaseObject *_sender = nullptr) : sender(_sender) {
    }
    const BaseObject *sender;
  };

  class Signal {
    friend class BaseSlot;

  public:
    Signal(BaseObject *_sender = nullptr) : sender(_sender), enabled(true) {
    }
    virtual ~Signal() {
      disconnectAll();
    }

    virtual void connect(BaseSlot *slot, bool _register = true) {
      auto it = std::find(_slots.begin(), _slots.end(), slot);

      if(it != _slots.end())
        return;

      _slots.push_back(slot);
      if(_register)
        slot->registerSignal(this);
    }

    virtual void disconnect(BaseSlot *slot, bool _unregister = true) {
      auto it = std::find(_slots.begin(), _slots.end(), slot);

      if(it == _slots.end())
        return;

      _slots.erase(it);

      if(_unregister)
        slot->unregisterSignal(this, false);
    }

    virtual void disconnectAll() {
      auto it = _slots.begin();

      while(it != _slots.end()) {
        (*it)->unregisterSignal(this, false);
        it++;
      }
    }

    virtual void trigger(Event *e = nullptr) {
      if(!enabled)
        return;

      if(e && sender)
        e->sender = sender;

      auto it = _slots.begin();

      while(it != _slots.end()) {
        (*it)->_run(e);
        it++;
      }
    }

    const BaseObject *sender;
    bool enabled;

  protected:
    vector<BaseSlot *> _slots;
  };

} // namespace smil

#endif // _DSIGNAL_H
