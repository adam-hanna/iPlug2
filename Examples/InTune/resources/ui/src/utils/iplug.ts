// FROM DELEGATE

export const STOP: number = 0;
export const SET: number = 1;
export const SET_MIDI: number = 2;
export const MODE_AUTOMATIC: number = 3;
export const MODE_MANAUL: number = 4;

export const SPVFD = (paramIdx: number, val: number) => {
  // @ts-ignore
  OnParamChange(paramIdx, val);
}

export const SCVFD = (ctrlTag: number, val: number) => {
  // @ts-ignore
  OnControlChange(ctrlTag, val);
}

export const SCMFD = (ctrlTag: number, msgTag: number, msg: number) => {
//  var decodedData = window.atob(msg);
  console.log("SCMFD ctrlTag: " + ctrlTag + " msgTag:" + msgTag + "msg:" + msg);
}

export const SAMFD = (msgTag: number, dataSize: number, msg: Array<number>) => {
  //  var decodedData = window.atob(msg);
  console.log("SAMFD msgTag:" + msgTag + " msg:" + msg);
}

export const SMMFD = (statusByte: number, dataByte1: number, dataByte2: number) => {
  console.log("Got MIDI Message" + statusByte + ":" + dataByte1 + ":" + dataByte2);
}

export const SSMFD = (offset: number, size: number, msg: Array<number>) => {
  console.log("Got Sysex Message");
}

// FROM UI
// data should be a base64 encoded string
export const SAMFUI = (msgTag: number, ctrlTag = -1, data: string) => {
  var message = {
    "msg": "SAMFUI",
    "msgTag": msgTag,
    "ctrlTag": ctrlTag,
    "data": data
  };

  // @ts-ignore
  IPlugSendMsg(message);
}

export const SMMFUI = (statusByte: number, dataByte1:number, dataByte2: number) => {
  var message = {
    "msg": "SMMFUI",
    "statusByte": statusByte,
    "dataByte1": dataByte1,
    "dataByte2": dataByte2
  };

  // @ts-ignore
  IPlugSendMsg(message);
}

// data should be a base64 encoded string
export const SSMFUI = (data = 0) => {
  var message = {
    "msg": "SSMFUI",
    "data": data
  };

  // @ts-ignore
  IPlugSendMsg(message);
}

export const EPCFUI = (paramIdx: number) => {
  var message = {
    "msg": "EPCFUI",
    "paramIdx": paramIdx,
  };

  // @ts-ignore
  IPlugSendMsg(message);
}

export const BPCFUI = (paramIdx: number) => {
  var message = {
    "msg": "BPCFUI",
    "paramIdx": paramIdx,
  };

  // @ts-ignore
  IPlugSendMsg(message);
}

export const SPVFUI = (paramIdx: number, value: number) => {
  var message = {
    "msg": "SPVFUI",
    "paramIdx": paramIdx,
    "value": value
  };

  // @ts-ignore
  IPlugSendMsg(message);
}