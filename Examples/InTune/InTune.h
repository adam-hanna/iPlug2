#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "Oscillator.h"
#include "Smoothers.h"

using namespace iplug;

const int kNumPresets = 3;

enum EParams
{
  kNumParams
};

enum EMsgTags
{
  kMsgPing = 0,
  kMsgTagStop = 1,
  kMsgTagSet = 2,
  kMsgTagModeAutomatic = 3,
  kMsgTagModeManual = 4,
  kMsgTagSetBars = 5
};

class InTune final : public Plugin
{
public:
  InTune(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
#endif
};
