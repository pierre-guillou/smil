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

#ifndef _DERRORS_H
#define _DERRORS_H

#include <exception>
#include <iostream>
#include <sstream>

#include "DCommon.h"

using namespace std;

namespace smil {
  /**
   * @ingroup Core
   * @{
   */

  enum RES_T {
    RES_OK = 1,
    RES_ERR = -100,
    RES_ERR_BAD_ALLOCATION,
    RES_ERR_BAD_SIZE,
    RES_ERR_IO,
    RES_ERR_NOT_IMPLEMENTED,
    RES_ERR_UNKNOWN
  };

  inline const char *getErrorMessage(const RES_T &res) {
    switch(res) {
      case RES_OK:
        return "ok";
      case RES_ERR_BAD_ALLOCATION:
        return "Bad allocation";
      default:
        return "Unknown error";
    }
  }

  class Error
#ifndef SWIG
    : public exception
#endif // SWIG
  {
  public:
    Error() noexcept(true) : line(0) {
    }

    Error(const string &descr) noexcept(true) : description(cleanDescr(descr)) {
      buildMessage();
    }

    Error(const string &descr,
          const char *func,
          const char *_file,
          const int _line,
          const char *expr) noexcept(true)
      : description(cleanDescr(descr)), function(func), file(_file),
        line(_line), expression(expr) {
      buildMessage();
    }

    Error(const string &descr,
          const char *func,
          const char *_file,
          const int _line) noexcept(true)
      : description(cleanDescr(descr)), function(func), file(_file),
        line(_line) {
      buildMessage();
    }

    Error(const char *func,
          const char *_file,
          const int _line,
          const char *expr) noexcept(true)
      : function(func), file(_file), line(_line), expression(expr) {
      buildMessage();
    }

    Error(const string &descr,
          const char *func,
          const char *expr) noexcept(true)
      : description(cleanDescr(descr)), function(func), expression(expr) {
      buildMessage();
    }

    Error(const char *func, const char *expr) noexcept(true)
      : function(func), expression(expr) {
      buildMessage();
    }

#ifndef SWIG
    ~Error() noexcept(true) override = default;

    const char *what() const noexcept(true) override {
      return this->message.c_str();
    }
#else
    virtual ~Error() noexcept(true) = default;

    virtual const char *what() const noexcept(true) {
      return this->message.c_str();
    }
#endif // SWIG

    void show() {
      cout << "Error:" << this->what() << endl;
    }

    void buildMessage() {
      ostringstream buf;
      if(!function.empty())
        buf << "\n  in function: " << function;
      if(!description.empty())
        buf << "\n  error: " << description;
#ifndef NDEBUG
      if(!expression.empty()) {
        if(description.empty())
          buf << "\n  error: assert " << expression;
        else
          buf << " ( assert " << expression << " )";
      }
      if(!file.empty())
        buf << "\n  file: " << file << ":" << line;
#endif // NDEBUG
      this->message = buf.str();
    }

  private:
    inline string cleanDescr(const string descr) {
      if(descr[0] != '"')
        return descr;
      else
        return descr.substr(1, descr.length() - 2);
    }

    string description;
    string function;
    string file;
#if defined NDEBUG && defined __clang__
    SMIL_UNUSED
#endif // NDEBUG && __clang__
    int line;
    string expression;
    string message;
  };

#define ASSERT_1_ARG(func, file, line, expr) \
  if(!(expr)) {                              \
    Error(func, file, line, #expr).show();   \
    return RES_ERR;                          \
  }
#define ASSERT_2_ARGS(func, file, line, expr, errCode) \
  if(!(expr)) {                                        \
    Error(#errCode, func, file, line, #expr).show();   \
    return errCode;                                    \
  }
#define ASSERT_3_ARGS(func, file, line, expr, errCode, retVal) \
  if(!(expr)) {                                                \
    Error(#errCode, func, file, line, #expr).show();           \
    return retVal;                                             \
  }

#define ERR_MSG(msg) Error(msg, __FUNC__, __FILE__, __LINE__).show()

#define ASSERT_NARGS_CHOOSER(...) \
  GET_4TH_ARG(__VA_ARGS__, ASSERT_3_ARGS, ASSERT_2_ARGS, ASSERT_1_ARG, ...)

#ifdef _MSC_VER
#define ASSERT(...)                         \
  EXPAND(ASSERT_NARGS_CHOOSER(__VA_ARGS__)( \
    __FUNC__, __FILE__, __LINE__, __VA_ARGS__))
#else // _MSC_VER
#define ASSERT(...) \
  ASSERT_NARGS_CHOOSER(__VA_ARGS__)(__FUNC__, __FILE__, __LINE__, __VA_ARGS__)
#endif // _MSC_VER

  /** @} */

} // namespace smil

#endif // _DERRORS_H
