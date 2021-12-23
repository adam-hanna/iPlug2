#include <vector>
#include <map>

#include "InTune.h"
#include "IPlug_include_in_plug_src.h"

using namespace std;

struct KeyMapper
{
  int key;
  vector<int> scale;
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

  void set_key(int k)
  {
    key = k;
  }

  void clear_scale()
  {
    scale.clear();
  }

  void set_scale(vector<int> new_scale)
  {
    scale = new_scale;
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
void InTune::ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames)
{
  /*
  const double gain = GetParam(kGain)->DBToAmp();
  
  sample maxVal = 0.;
  
  mOscillator.ProcessBlock(inputs[0], nFrames); // comment for audio in

  for (int s = 0; s < nFrames; s++)
  {
    outputs[0][s] = inputs[0][s] * mGainSmoother.Process(gain);
    outputs[1][s] = outputs[0][s]; // copy left
    
    maxVal += std::fabs(outputs[0][s]);
  }
  
  mLastPeak = static_cast<float>(maxVal / (sample) nFrames);
  */
}

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
  default: {
    return;
  }
  }
}
#endif
