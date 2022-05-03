#include <vector>
#include <map>

#include "InTune.h"
#include "IPlug_include_in_plug_src.h"

using namespace std;

struct KeyScales
{
  int key;
  vector<int> scale;
};

struct KeyMapper
{
  int keyScaleIDX = 0;
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
    Trace("KeyMapper::clear_scale", __LINE__, "");
    scale.clear();
  }

  void set_scale(vector<int> new_scale)
  {
    std::string str;
    for (int i : new_scale)
    {
      str.push_back(i + '0');
    }
    Trace("KeyMapper::set_scale", __LINE__, "new scales : [%s]", str);

    scale = new_scale;
  }

  void set_key(int k)
  {
    Trace("KeyMapper::set_key", __LINE__, "key : %d", key);
    key = k;
  }

  void set_bars(vector<int> data)
  {
    keyScales.clear();

    if (data.size() < 3)
    {
      return;
    }

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
      }
      else {
        k.scale.push_back(i);

        idx++;
        continue;
      }
    }


    idx = 0;
    Trace("KeyMapper::set_bars", __LINE__, "keyScales.size() : %d", keyScales.size());
    for (KeyScales k : keyScales)
    {
      std::string str;
      for (int i : k.scale)
      {
        str.push_back(i + '0');
      }
      Trace("KeyMapper::set_bars", __LINE__, "#%d - key : %d; scales : [%s]", idx, k.key, str);
      idx++;
    }

    
    if (keyScaleIDX > keyScales.size() - 1)
    {
      keyScaleIDX = keyScales.size() - 1;
    }
    set_bar_at_idx(keyScaleIDX);
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
    }
    else {
      keyScaleIDX--;
    }

    Trace("KeyMapper::prev", __LINE__, "new keyScaleIDX : %d", keyScaleIDX);
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

    Trace("KeyMapper::next", __LINE__, "new keyScaleIDX : %d", keyScaleIDX);
    set_bar_at_idx(keyScaleIDX);
  }

  void set_bar_at_idx(int idx)
  {
    try
    {
      Trace("KeyMapper::set_bar_at_idx", __LINE__, "idx : %d", idx);
      auto newKeyScale = keyScales.at(idx);
      set_key(newKeyScale.key);
      set_scale(newKeyScale.scale);
    }
    catch (const std::out_of_range& ex)
    {
      DBGMSG("out_of_range Exception Caught ::  %s\n", ex.what());
    }
  }
};

KeyMapper km = KeyMapper();

InTune::InTune(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  //GetParam(kGain)->InitGain("Gain", -70., -70, 0.);

  // Hard-coded paths must be modified!
#ifdef OS_WIN
  SetWebViewPaths("C:\\cygwin64\\home\\ahann\\apps\\iplug\\iPlug2\\Examples\\InTune\\packages\\Microsoft.Web.WebView2.1.0.824-prerelease\\runtimes\\win-x64\\native\\WebView2Loader.dll", "C:\\cygwin64\\home\\ahann\\apps\\iplug\\iPlug2\\Examples\\InTune\\");
#endif


  mEditorInitFunc = [&]() {
#ifdef OS_WIN
    //LoadHTML(R"(<h1>Foo</h1>)");
    LoadFile("C:\\cygwin64\\home\\ahann\\apps\\iplug\\iPlug2\\Examples\\InTune\\resources\\ui\\build\\index.html", nullptr);
#else
    LoadFile("index.html", GetBundleID());
#endif

    EnableScroll(true);
  };
}

#if IPLUG_DSP
bool InTune::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
  DBGMSG("have mesage; tag %d\n", msgTag);
  switch (msgTag) {
  case kMsgTagStop:
  {
    km.set_key(0);
    km.clear_scale();

    break;
  }
  case kMsgTagSet:
  {
    auto int8Data = reinterpret_cast<const uint8_t*>(pData);
    DBGMSG("Data Size %i bytes\n", dataSize);
    if (dataSize < 3) {
      break;
    }

    
    for (int i = 0; i < dataSize; i++) {
      DBGMSG("int8Data[%i] - %i", i, int8Data[i]);
    }
    

    km.set_key(int8Data[0]);
    DBGMSG("new key %i\n", int8Data[0]);

    vector<int> new_scale(&int8Data[1], &int8Data[dataSize]);

    for (int i : new_scale)
      DBGMSG("scale %i\n", i);
    

    new_scale.insert(new_scale.begin(), 0);
    km.set_scale(new_scale);
    break;
  }
  case kMsgTagModeAutomatic:
  {
    // TODO

    break;
  }
  case kMsgTagModeManual:
  {
    // TODO

    break;
  }
  case kMsgTagSetBars:
  {
    if (dataSize < 3) {
      DBGMSG("size less than 3");
      break;
    }

    auto int8Data = reinterpret_cast<const uint8_t*>(pData);
    for (int i = 0; i < dataSize; i++) {
      DBGMSG("int8Data[%i] - %i", i, int8Data[i]);
    }

    
    auto int8Data2 = reinterpret_cast<const int8_t*>(pData);
    vector<int> new_data(&int8Data2[0], &int8Data2[dataSize]);

    for (int i = 0; i < dataSize; i++) {
      DBGMSG("new_data[%i] - %i", i, new_data[i]);
    }
    km.set_bars(new_data);
    
    break;
  }
  case kMsgPing:
  {
    DBGMSG("kMsgPing");
    //unsigned char buffer[p.length()];
    //copy(p.begin(), p.end(), buffer);
    //const uint8_t p = 7;
    //const uint8_t* pPtr = &p;

    //string ret = "pong";
    //InTune::SendArbitraryMsgFromDelegate(1, ret.length(), ret.c_str());
    break;
  }
  default:
  {
    DBGMSG("unknown mesage tag %d\n", msgTag);
    break;
  }
  }

  return false;
}

void InTune::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;

  int status = msg.StatusMsg();
  int ctrl = msg.ControlChangeIdx();

  Trace("InTune::ProcessMidiMsg", __LINE__, "status : %d; ctrl : %d", status, ctrl);
  DBGMSG("status %d\n", status);
  
  switch (status)
  {
  case IMidiMsg::kNoteOn:
  {
    int new_note_number = km.transpose_pressed_key(msg);
    if (new_note_number == -1) {
      return;
    }

    IMidiMsg new_msg;
    new_msg.MakeNoteOnMsg(new_note_number, msg.Velocity(), msg.mOffset, msg.Channel());

    SendMidiMsg(new_msg);
    return;
  }
  case IMidiMsg::kNoteOff:
  {
    int new_note_number = km.transpose_pressed_key(msg);
    if (new_note_number == -1) {
      return;
    }

    IMidiMsg new_msg;
    new_msg.MakeNoteOffMsg(new_note_number, msg.mOffset, msg.Channel());

    SendMidiMsg(new_msg);
    return;
  }
  default:
  {
    DBGMSG("ctrl %d\n", ctrl);

    switch (ctrl)
    {
    case IMidiMsg::kUndefined102:
    {
      // note: 102 is prev bar
      km.prev();

      break;
    }
    case IMidiMsg::kUndefined103:
    {
      // note: 103 is next bar
      km.next();

      break;
    }
    case IMidiMsg::kUndefined104:
      [[fallthrough]];
    case IMidiMsg::kUndefined105:
      [[fallthrough]];
    case IMidiMsg::kUndefined106:
      [[fallthrough]];
    case IMidiMsg::kUndefined107:
      [[fallthrough]];
    case IMidiMsg::kUndefined108:
      [[fallthrough]];
    case IMidiMsg::kUndefined109:
      [[fallthrough]];
    case IMidiMsg::kUndefined110:
      [[fallthrough]];
    case IMidiMsg::kUndefined111:
    {
      // note: change to a stored bar
      km.set_bar_at_idx(ctrl - 104);

      break;
    }
    default:
    {
      // note: do nothing...
      break;
    }
    }

    SendMidiMsg(msg);

    return;
  }
  }
}
#endif
