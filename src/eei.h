/*
 * Copyright 2016-2018 Alex Beregszaszi et al.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <wasm.h>
#include <wasm-binary.h>
#include <evmc/evmc.h>

#include "shell-interface.h"
#include "exceptions.h"

using namespace wasm;

namespace hera {

struct ExecutionResult {
  int64_t gasLeft = 0;
  std::vector<uint8_t> returnValue;
  bool isRevert = false;
};

struct EthereumInterface : ShellExternalInterface {
  EthereumInterface(
    evmc_context* _context,
    std::vector<uint8_t> const& _code,
    evmc_message const& _msg,
    ExecutionResult & _result,
    bool _meterGas
  ):
    ShellExternalInterface(),
    m_context(_context),
    m_code(_code),
    m_msg(_msg),
    m_result(_result),
    m_meterGas(_meterGas)
  {
    // set starting gas
    m_result.gasLeft = m_msg.gas;
    // set sane defaults
    m_result.returnValue = std::vector<uint8_t>{};
    m_result.isRevert = false;
  }

  Literal callImport(Import *import, LiteralList& arguments) override;
#if HERA_DEBUGGING
  Literal callDebugImport(Import *import, LiteralList& arguments);
#endif

  void importGlobals(std::map<Name, Literal>& globals, Module& wasm) override;

  void trap(const char* why) override {
    ensureCondition(false, VMTrap, why);
  }

private:
  enum class EEICallKind {
    Call,
    CallCode,
    CallDelegate,
    CallStatic
  };

  // EEI methods

  void eeiRevertOrFinish(bool revert, uint32_t offset, uint32_t size);
  uint32_t eeiGetReturnDataSize();
  void eeiReturnDataCopy(uint32_t dataOffset, uint32_t offset, uint32_t size);
  uint32_t eeiCall(EEICallKind kind, int64_t gas, uint32_t addressOffset, uint32_t valueOffset, uint32_t dataOffset, uint32_t dataLength);
  uint32_t eeiCreate(uint32_t valueOffset, uint32_t dataOffset, uint32_t length, uint32_t resultOffset);
  void eeiSelfDestruct(uint32_t addressOffset);

  // Helpers methods

  void takeGas(int64_t gas);
  void takeInterfaceGas(int64_t gas);

  inline size_t memorySize() const { return memory.size(); }
  inline void memorySet(size_t offset, uint8_t value) { memory.set<uint8_t>(offset, value); }
  inline uint8_t memoryGet(size_t offset) { return memory.get<uint8_t>(offset); }

  void ensureSourceMemoryBounds(uint32_t offset, uint32_t length);
  void loadMemoryReverse(uint32_t srcOffset, uint8_t *dst, size_t length);
  void loadMemory(uint32_t srcOffset, uint8_t *dst, size_t length);
  void loadMemory(uint32_t srcOffset, std::vector<uint8_t> & dst, size_t length);
  void storeMemoryReverse(const uint8_t *src, uint32_t dstOffset, uint32_t length);
  void storeMemory(const uint8_t *src, uint32_t dstOffset, uint32_t length);
  void storeMemory(std::vector<uint8_t> const& src, uint32_t srcOffset, uint32_t dstOffset, uint32_t length);

  evmc_uint256be loadUint256(uint32_t srcOffset);
  void storeUint256(evmc_uint256be const& src, uint32_t dstOffset);
  evmc_address loadAddress(uint32_t srcOffset);
  void storeAddress(evmc_address const& src, uint32_t dstOffset);
  evmc_uint256be loadUint128(uint32_t srcOffset);
  void storeUint128(evmc_uint256be const& src, uint32_t dstOffset);

  inline int64_t maxCallGas(int64_t gas) { return gas - (gas / 64); }

  /* Checks for overflow and safely charges gas for variable length data copies */
  void safeChargeDataCopy(uint32_t length, unsigned baseCost);

  bool enoughSenderBalanceFor(evmc_uint256be const& value) const;

  static unsigned __int128 safeLoadUint128(evmc_uint256be const& value);

  /* Checks if host supplied 256 bit value exceeds UINT128_MAX */
  static bool exceedsUint128(evmc_uint256be const& value);

  /* Checks if a 128 bit value is all zeroes */
  static bool isZeroUint128(evmc_uint256be const& value);

  /* Checks if a 256 bit value is all zeroes */
  static bool isZeroUint256(evmc_uint256be const& value);

  evmc_context* m_context = nullptr;
  std::vector<uint8_t> const& m_code;
  evmc_message const& m_msg;
  std::vector<uint8_t> m_lastReturnData;
  ExecutionResult & m_result;
  bool m_meterGas = true;
};

struct GasSchedule {
  static constexpr unsigned storageLoad = 200;
  static constexpr unsigned storageStoreCreate = 20000;
  static constexpr unsigned storageStoreChange = 5000;
  static constexpr unsigned log = 375;
  static constexpr unsigned logData = 8;
  static constexpr unsigned logTopic = 375;
  static constexpr unsigned create = 32000;
  static constexpr unsigned call = 700;
  static constexpr unsigned copy = 3;
  static constexpr unsigned blockhash = 800;
  static constexpr unsigned balance = 400;
  static constexpr unsigned base = 2;
  static constexpr unsigned verylow = 3;
  static constexpr unsigned extcode = 700;
  static constexpr unsigned selfdestruct = 5000;
  static constexpr unsigned valuetransfer = 9000;
  static constexpr unsigned valueStipend = 2300;
  static constexpr unsigned callNewAccount = 25000;
};

}
