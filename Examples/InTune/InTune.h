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

struct KeyScales
{
  int key;
  vector<int> scale;
};

struct KeyMapper
{
  int keyScaleIDX;
  int key;
  vector<int> scale;
  vector<KeyScales> keyScales;
  unordered_map<int, int> keys_on_map; // note: map is pressed_key -> transposed_key

  int transpose_pressed_key(const IMidiMsg& msg)
  {
    int status = msg.StatusMsg();
    int pressed_key = msg.NoteNumber();

    if (scale.size() == 0) {
      return pressed_key;
    }

    switch (status) {
    case IMidiMsg::kNoteOn:
    {
      // 144 is NOTE_ON
      int dist_from_root = pressed_key % 12;
      if (dist_from_root >= scale.size()) {
        return -1;
      }

      int offset = scale.at(dist_from_root);

      int octave = pressed_key / 12;
      int delta_key = key - 12;

      int new_key = (12 * octave) + delta_key + offset;

      keys_on_map.insert({ pressed_key, new_key });

      return new_key;
    }
    case IMidiMsg::kNoteOff:
    {
      // 128 is NOTE_OFF
      unordered_map<int, int>::const_iterator mapped_key = keys_on_map.find(pressed_key);

      if (mapped_key == keys_on_map.end()) {
        return pressed_key;
      }

      int ret = mapped_key->second;
      keys_on_map.erase(mapped_key);

      return ret;
    }
    default:
    {
      DBGMSG("unhandled midi status %d\n", status);

      return -1;
    }
    }
  }

  void clear_scale()
  {
    scale.clear();
  }

  void set_scale(vector<int> new_scale)
  {
    scale = new_scale;
  }

  void set_key(int k)
  {
    key = k;
  }

  void set_bars(vector<int> data)
  {
    if (data.size() < 3) {
      return;
    }

    keyScales.clear();
    keyScaleIDX = 0;

    KeyScales k;

    int idx = 0;
    bool nextIsKey = false;
    for (int i : data) {
      if (idx == 0) {
        k.key = data.at(0);
        idx++;
        continue;
      }

      // note: -1 indicates the end of this key scale
      if (i == -1) {
        KeyScales k1;
        k1 = k;
        k.scale.clear();
        nextIsKey = true;

        keyScales.push_back(k1);

        idx++;
        continue;
      }

      // note: the first int in the following sequence is the key; what follows is the scale until -1
      if (nextIsKey) {
        k.key = i;
        nextIsKey = false;

        idx++;

        continue;
      } else {
        k.scale.push_back(i);

        idx++;
        continue;
      }
    }
  }

  void prev()
  {
    int size = keyScales.size();
    if (size == 0) {
      return;
    }

    if (keyScaleIDX >= size) {
      keyScaleIDX = size - 1;
    }
    else if (keyScaleIDX <= 0) {
      keyScaleIDX = size - 1;
    } else {
      keyScaleIDX--;
    }

    set_bar_at_idx(keyScaleIDX);
  }

  void next()
  {
    int size = keyScales.size();
    if (size == 0) {
      return;
    }

    if (keyScaleIDX >= size - 1) {
      keyScaleIDX = 0;
    }
    else if (keyScaleIDX < 0) {
      keyScaleIDX = 0;
    }
    else {
      keyScaleIDX++;
    }

    set_bar_at_idx(keyScaleIDX);
  }

  void set_bar_at_idx(int idx)
  {
    try
    {
      auto newKeyScale = keyScales.at(keyScaleIDX);
      set_key(newKeyScale.key);
      set_scale(newKeyScale.scale);
    }
    catch (const std::out_of_range& ex)
    {
      DBGMSG("out_of_range Exception Caught ::  %s\n", ex.what());
    }
  }
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
