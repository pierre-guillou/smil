/*
 * Copyright (c) 2011, Matthieu FAESSEL and ARMINES
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
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _DCORE_INSTANCE_H
#define _DCORE_INSTANCE_H

#include <iostream>
#include <algorithm>

// #include "DGui.h"
#include "Qt/QtApp.h"

#include "DCommon.h"
#include <qtimer.h>
#include "DTimer.h"

class baseObject;

struct stat;

class guiInstance
{
public:
  guiInstance()
    : _qapp(NULL)
  {
	if (!qApp)
	{
// 	    cout << "core qt created" << endl;
	    int ac = 1;
	    char **av = NULL;
	    _qapp = new QApplication(ac, av);
	}
	_timer = new timer();
	_timer->app = _qapp;
	_timer->start();
  }
  ~guiInstance()
  {
      delete _timer;
  }
  void exec() 
  { 
      if (_qapp)
	_qapp->exec(); 
  }
  void processEvents() 
  { 
      if (_qapp)
	_qapp->processEvents(); 
  }
protected:
  QApplication *_qapp;
  timer *_timer;
};

class coreInstance
{
private:
  coreInstance ()
    : _value (0), keepAlive(false)
    { 
// 	cout << "core created" << endl;
	guiInst = new guiInstance();
      
    }
  ~coreInstance () 
  {
// 	cout << "core destroyed" << endl;
  }

public:
  // Public interface
  bool keepAlive;
  void setValue (int val) { _value = val; }
  int getValue () { return _value; }
  
  void registerObject(baseObject *obj);

  void unregisterObject(baseObject *obj);
  
  vector<baseObject*> getRegisteredObjects() { return registeredObjects; }
  
  void exec() 
  { 
      guiInst->exec();
  }
  void processEvents() 
  { 
      guiInst->processEvents();
  }

protected:
  void deleteRegisteredObjects();
//   QApplication *_qapp;
  guiInstance *guiInst;

  
public:
  static coreInstance *getInstance ()
  {
    if (NULL == _singleton)
      {
//         std::cout << "creating singleton." << std::endl;
        _singleton =  new coreInstance;
      }
    else
      {
//         std::cout << "singleton already created!" << std::endl;
      }

    return _singleton;
  }

  static void kill ()
  {
    if (_singleton==NULL)
      return;
    
//       std::cout << "Bye" << std::endl;
      _singleton->deleteRegisteredObjects();
      delete _singleton;
      _singleton = NULL;
    
  }
  

private:
  // Variables membres
  int _value;
  vector<baseObject*> registeredObjects;
  static coreInstance *_singleton;
};



#endif // _DCORE_INSTANCE_H

