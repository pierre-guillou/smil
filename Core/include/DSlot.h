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

#ifndef _DSLOT_H
#define _DSLOT_H

#include <vector>
#include <iostream>

using namespace std;

#include "DCommon.h"

namespace smil {
  class Event;
  class Signal;

  class BaseSlot {
    friend class Signal;

  public:
    BaseSlot() = default;
    virtual ~BaseSlot() {
      unregisterAll();
    }
    virtual void run(Event * /*e*/ = nullptr) {
    }

  protected:
#ifndef SWIG
    virtual void _run(Event *e = nullptr) {
      run(e);
    }
    virtual void registerSignal(Signal *signal);
    virtual void unregisterSignal(Signal *signal, bool _disconnect = true);
    virtual void unregisterAll();
    vector<Signal *> _signals;
#endif // SWIG
  };

  template <class eventT>
  class Slot : public BaseSlot {
  public:
    Slot() = default;
    ~Slot() override = default;
    void operator()(eventT * /*e*/) {
    }
#ifndef SWIG
  protected:
    void _run(Event *e) override {
      run(e);
    }
#endif // SWIG
  private:
    using BaseSlot::run;
  };

  template <class T, class eventT = Event>
  class MemberFunctionSlot : public Slot<eventT> {
  public:
    using memberFunc = void (T::*)(eventT *);
    using voidMemberFunc = void (T::*)();
    MemberFunctionSlot() {
      _instance = nullptr;
      _function = nullptr;
    }
    MemberFunctionSlot(T *inst, memberFunc func) {
      init(inst, func);
    }
    MemberFunctionSlot(T *inst, voidMemberFunc func) {
      init(inst, func);
    }
    void init(T *inst, memberFunc func) {
      _instance = inst;
      _function = func;
    }
    void init(T *inst, voidMemberFunc func) {
      _instance = inst;
      _void_function = func;
    }

  protected:
    T *_instance;
    memberFunc _function;
    voidMemberFunc _void_function;
    void run(eventT *e = NULL) override {
      if(!_instance)
        return;

      if(_function)
        (_instance->*_function)(e);
      if(_void_function)
        (_instance->*_void_function)();
    }
  };

  template <class eventT = Event>
  class FunctionSlot : public Slot<eventT> {
  public:
    using funcPtr = void (*)(eventT *);
    using voidFuncPtr = void (*)();
    FunctionSlot(funcPtr func) {
      _function = func;
    }

  protected:
    funcPtr _function;
    virtual void run(eventT *e) {
      (*_function)(e);
    }
  };

} // namespace smil

#endif // _DSLOT_H
