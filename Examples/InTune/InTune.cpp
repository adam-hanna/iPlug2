#include <vector>
#include <map>

#include "InTune.h"
#include "IPlug_include_in_plug_src.h"

using namespace std;

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

    /*
    for (int i = 0; i < dataSize; i++) {
      DBGMSG("int8Data[%i] - %i", i, int8Data[i]);
    }
    */

    km.set_key(int8Data[0]);
    DBGMSG("new key %i\n", int8Data[0]);

    vector<int> new_scale(&int8Data[1], &int8Data[dataSize]);

    /*
    for (int i : new_scale)
      DBGMSG("scale %i\n", i);
    */

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
      break;
    }

    auto int8Data = reinterpret_cast<const int8_t*>(pData);
    vector<int> new_data(&int8Data[0], &int8Data[dataSize]);
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
    string ret = "pong";
    InTune::SendArbitraryMsgFromDelegate(1, ret.length(), ret.c_str());
    break;
  }
  default:
  {
    DBGMSG("unknown mesage tag %d\n", msgTag);
  }
  }

  return false;
}

void InTune::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;

  int status = msg.StatusMsg();
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
    int ctrl = msg.ControlChangeIdx();
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
    case IMidiMsg::kUndefined105:
    case IMidiMsg::kUndefined106:
    case IMidiMsg::kUndefined107:
    case IMidiMsg::kUndefined108:
    case IMidiMsg::kUndefined109:
    case IMidiMsg::kUndefined110:
    case IMidiMsg::kUndefined111:
    {
      // note: change to a stored bar
      km.set_bar_at_idx(ctrl - 104);

      break;
    }
    default:
    {
      // note: do nothing...
    }
    }

    SendMidiMsg(msg);

    return;
  }
  }
}
#endif
