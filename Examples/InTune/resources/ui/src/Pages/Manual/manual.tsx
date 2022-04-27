import React, { useEffect, useState, useReducer } from 'react'
import { Button, Icon, Modal } from 'semantic-ui-react'
import styled from 'styled-components'
import { KeyAndScaleSelectors } from '../../Components/KeyAndScaleSelectors'

import { ScaleSelector } from '../../Components/Selectors/Scales'
import {
  SET_MIDI,
  MODE_MANAUL,
  SAMFUI
} from '../../utils'

const { NODE_ENV } = process.env

export type Props = {

}

const Wrapper = styled.div`
  padding-top: 5px;

  width: 100%;
`

const HeaderWrapper = styled.div`
  width: 100%;

  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
`

const ControlsWrapper = styled.div`
  display: flex;
  flex-direction: row;

  margin-bottom: 10px;
`

const SaveLoadWrapper = styled.div`
  display: flex;
  flex-direction: column;
`

const InputsWrapper = styled.div`
  display: flex;
  flex-direction: row;
  align-items: center;

  margin: 0px 0px 15px 0px;
`

const BarsWrapper = styled.div`
  position: relative;
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;

  max-width: 1200px;

  padding-bottom: 5px;
`

type BarState = {
  musicKey: string;
  scale: string;
  midi: string;
}

const initialBars: BarState[] = [
  {
    musicKey: "C",
    scale: "major",
    midi: "not set"
  }
]

type Action = {
  idx: number;
  type: string;
  data: string;
}

type BarReducerFn = (state: BarState[], action: Action) => BarState[];

const barReducer: BarReducerFn = (state, action) => {
  switch (action.type) {
    case "CHANGE_MIDI":
      return state.map((bar, idx) => {
        if (idx === action.idx) {
          return { ...bar, midi: action.data }
        }

        return bar
      })

    case "CHANGE_KEY":
      return state.map((bar, idx) => {
        if (idx === action.idx) {
          return { ...bar, musicKey: action.data }
        }

        return bar
      })

    case "CHANGE_SCALE":
      return state.map((bar, idx) => {
        if (idx === action.idx) {
          return { ...bar, scale: action.data }
        }

        return bar
      })

    case "CHANGE_ALL_SCALES":
      return state.map((bar) => {
        return { ...bar, scale: action.data }
      })

    case "SET_BARS":
      try {
        const newBars = JSON.parse(action.data)
        return [ ...newBars ]
      } catch(e) {
        console.error('err processing data', action.data, e)
        return [ ...state ]
      }

    case "ADD_BAR":
      return [...state, { musicKey: "C", scale: "major", midi: "not set" }]

    case "REMOVE_BAR":
    return [...state].filter((bar, idx) => {
      return idx !== action.idx
    });

    default:
      return state;
  }
}

export const Manual = ({}: Props) => {
  const [bars, dispatch] = useReducer(
    barReducer,
    initialBars,
  );
  const [currentBar, setCurrentBar] = useState(0)

  const [scaleModal, setScaleModal] = useState(false)
  const [scale, setScale] = useState('Major')

  const [nextMidi, setNextMidi] = useState('not set')
  const [prevMidi, setPrevMidi] = useState('not set')

  const handleKeyChange = (idx: number, data: string) => {
    dispatch({ idx, type: 'CHANGE_KEY', data });
  };
  const handleScaleChange = (idx: number, data: string) => {
    dispatch({ idx, type: 'CHANGE_SCALE', data });
  };
  const handleMIDIChange = (idx: number, data: string) => {
    dispatch({ idx, type: 'CHANGE_MIDI', data });
  };

  const LowAudio = new Audio("https://in-tune-media.s3.amazonaws.com/Low_Seiko_SQ50.wav")
  const HighAudio = new Audio("https://in-tune-media.s3.amazonaws.com/High_Seiko_SQ50.wav")

  useEffect(() => {
    if (NODE_ENV === 'production') {
      SAMFUI(MODE_MANAUL, -1, '');


      // @ts-ignore
      window.SAMFD = (msgTag: number, dataSize: number, msg: Array<number>) => {
        var decodedData = window.atob(String(msg));
        console.log("SAMFD msgTag:" + msgTag + "; msg:" + msg + "; decoded:" + decodedData);
      }
    }
  }, [])

  return (
    <Wrapper>
      <Modal
        onClose={() => setScaleModal(false)}
        onOpen={() => setScaleModal(true)}
        open={scaleModal}
      >
        <Modal.Header>Select a Scale</Modal.Header>
        <Modal.Content>
          <Modal.Description>
            <ScaleSelector
              disable={false}
              value={scale}
              onChange={setScale}
            />
          </Modal.Description>
        </Modal.Content>
        <Modal.Actions>
          <Button color='black' onClick={() => setScaleModal(false)}>
            Cancel
          </Button>
          <Button
            content="Set Scale"
            labelPosition='right'
            icon='checkmark'
            onClick={() => {
              dispatch({ idx: -1, type: 'CHANGE_ALL_SCALES', data: scale })
              setScaleModal(false)
            }}
            positive
          />
        </Modal.Actions>
      </Modal>
      <HeaderWrapper>
        <ControlsWrapper>
          <SaveLoadWrapper>
            <Button
              icon='download'
              title="save"
              onClick={() => {
                const textToWrite = JSON.stringify(bars)
                const textFileAsBlob = new Blob([ textToWrite ], { type: 'text/plain' })
                const fileNameToSaveAs = "bars.json"

                const downloadLink = document.createElement("a")
                downloadLink.download = fileNameToSaveAs
                downloadLink.innerHTML = "Download File"
                if (window.webkitURL != null) {
                  // Chrome allows the link to be clicked without actually adding it to the DOM.
                  downloadLink.href = window.webkitURL.createObjectURL(textFileAsBlob)
                } else {
                  // Firefox requires the link to be added to the DOM before it can be clicked.
                  downloadLink.href = window.URL.createObjectURL(textFileAsBlob)
                  downloadLink.onclick = () => {
                    document.body.removeChild(downloadLink)
                  }
                  downloadLink.style.display = "none"
                  document.body.appendChild(downloadLink)
                }

                downloadLink.click()
              }}
            />
            {/*
            <input
              id="file-input"
              type="file"
              name="name"
              style={{ display: "none" }}
              onchange={(e: HTMLInputEvent) => {
                if (!e || !e.target || !e.target.files || e.target.files.length === 0) {
                  return
                }

                const selectedFile = e.target.files[0];
                const reader = new FileReader();

                reader.onload = function(event) {
                  if (!event || !event.target) {
                    return
                  }
                  dispatch({ idx: -1, type: 'SET_BARS', data: String(event.target.result) })
                };

                reader.readAsText(selectedFile);
              }}
            />
            */}
            <Button
              icon='upload'
              title="load"
              onClick={() => {
                //document.getElementById('file-input')?.click()
                const downloadLink = document.createElement("input")
                downloadLink.type = "file"
                // @ts-ignore
                downloadLink.onchange = (e: HTMLInputEvent) => {
                  if (!e || !e.target || !e.target.files || e.target.files.length === 0) {
                    return
                  }

                  const selectedFile = e.target.files[0];
                  const reader = new FileReader();

                  reader.onload = function(event) {
                    if (!event || !event.target) {
                      return
                    }
                    dispatch({ idx: -1, type: 'SET_BARS', data: String(event.target.result) })
                  };

                  reader.readAsText(selectedFile);
                }

                downloadLink.innerHTML = "Upload File"
                if (window.webkitURL != null) {
                  // Chrome allows the link to be clicked without actually adding it to the DOM.
                } else {
                  // Firefox requires the link to be added to the DOM before it can be clicked.
                  downloadLink.onclick = () => {
                    document.body.removeChild(downloadLink)
                  }
                  downloadLink.style.display = "none"
                  document.body.appendChild(downloadLink)
                }

                downloadLink.click()
              }}
            />
          </SaveLoadWrapper>
          <Button secondary onClick={() => { setScaleModal(true) }}>Set All Scales</Button>
        </ControlsWrapper>
        <InputsWrapper>
          <div
            className="ui labeled button"
            style={{ marginTop: '20px', marginLeft: '5px' }}
            onClick={() => {
              if (NODE_ENV === 'production') {
                SAMFUI(SET_MIDI, -1, Buffer.from(["next"]).toString('base64'));
              }
            }}
          >
            <div className="ui button">
              <i className="angle right icon"></i> Next
            </div>
            <a className="ui basic label">
              {nextMidi}
            </a>
          </div>
          <div
            className="ui labeled button"
            style={{ marginTop: '20px', marginLeft: '5px' }}
            onClick={() => {
              if (NODE_ENV === 'production') {
                let data = Buffer.from(["prev"]).toString('base64')
                SAMFUI(SET_MIDI, data.length, data);
              }
            }}
          >
            <div className="ui button">
              <i className="angle left icon"></i> Prev
            </div>
            <a className="ui basic label">
              {prevMidi}
            </a>
          </div>
          <Button
           onClick={() => {
              if (NODE_ENV === 'production') {
                SAMFUI(2, -1, '');
              }
           }}
          >
            Ping
          </Button>
        </InputsWrapper>
      </HeaderWrapper>
      <BarsWrapper>
        <ol>
          {(bars as BarState[]).map((bar, idx) => (
            <li style={{ display: 'flex', alignItems: 'center', justifyContent: 'center', backgroundColor: `rgba(255, 255, 0, ${currentBar === idx ? 1 : 0})`, cursor: 'pointer', paddingBottom: '5px' }}>
              {<Icon title='Remove' style={{ position: 'static', color: 'red', fontSize: '50px', marginTop: '20px', paddingTop: '10px' }} name='remove circle' onClick={() => {
                  dispatch({ idx, type: 'REMOVE_BAR', data: '' })
                }} />}
              <KeyAndScaleSelectors
                key={idx}
                disableSelectors={false}
                isActive={false}

                musicKey={bar.musicKey}
                onKeyChange={(key: string) => { handleKeyChange(idx, key) }}

                scale={bar.scale}
                onScaleChange={(scale: string) => { handleScaleChange(idx, scale) }}
              />
              <div
                className="ui labeled button"
                style={{ marginTop: '20px', marginLeft: '5px' }}
                onClick={() => {
                  handleMIDIChange(idx, "press midi button")
                }}
              >
                <div className="ui button">
                  <i className="edit icon"></i> MIDI Button
                </div>
                <a className="ui basic label">
                  {bar.midi}
                </a>
              </div>
            </li>
          ))}
        </ol>
      </BarsWrapper>
        <Button
          primary
          disabled={false}
          onClick={() => { dispatch({ idx: -1, type: 'ADD_BAR', data: '' }) }}
        >
          Add Bar
        </Button>
    </Wrapper>
  )
}