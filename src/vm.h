/*
 * Hera VM: eWASM virtual machine conforming to the Ethereum VM C API
 *
 * Copyright (c) 2016 Alex Beregszaszi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __VM_H
#define __VM_H

#include "hera.h"
#include "eei.h" /* for ExecutionResult */

#include <evmc.h>

using namespace std;
using namespace HeraVM;

/* Base class for WASM Execution Engines */
class WasmEngine
{
public:
  WasmEngine(wasm_vm const _vm, 
    vector<uint8_t> const& _code, 
    evmc_message const& _msg,
    evmc_context *_context
  ):
    vm(_vm),
    code(_code),
    msg(_msg),
    context(_context),
    output(_msg.gas)
  { }
  
  /* Each WASM engine overrides this method */
  virtual int execute();

  ExecutionResult & getResult() { return output; }

protected:
  wasm_vm vm;
  vector<uint8_t> code;
  evm_message msg;
  evm_context *context;
  ExecutionResult output;
};

class BinaryenVM : public WasmEngine
{
public:
  BinaryenVM(vector<uint8_t> const& _code,
    evm_message const& _msg,
    evm_context *_context) : 
    WasmEngine(VM_BINARYEN, _code, _msg, _context)
    { }
  
  int execute();
};

#if WABT_SUPPORTED
class WabtVM : public WasmEngine
{
public:
  WabtVM(vector<uint8_t> const& _code,
    evm_message const& msg,
    evm_context *_context) :
    WasmEngine(VM_WABT, _code, _msg, _context)
    { }

  int execute();
};
#endif

#if WAVM_SUPPORTED
class WavmVM : public WasmEngine
{
public:
  WavmVM(vector<uint8_t> const& _code,
    evm_message const& msg,
    evm_context *_context) :
    WasmEngine(VM_WAVM, _code, _msg, _context)
    { }

  int execute();
};
#endif
#endif