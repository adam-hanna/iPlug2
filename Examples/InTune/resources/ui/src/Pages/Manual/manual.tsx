import React, { useState, useReducer } from 'react'
import { Button, Icon, Modal } from 'semantic-ui-react'
import styled from 'styled-components'
import { KeyAndScaleSelectors } from '../../Components/KeyAndScaleSelectors'

import { ScaleSelector } from '../../Components/Selectors/Scales'

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
}

const initialBars: BarState[] = [
  {
    musicKey: "C",
    scale: "major",
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
      return [...state, { musicKey: "C", scale: "major" }]

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

  const [icon, setIcon] = useState('play')

  const handleKeyChange = (idx: number, data: string) => {
    dispatch({ idx, type: 'CHANGE_KEY', data });
  };
  const handleScaleChange = (idx: number, data: string) => {
    dispatch({ idx, type: 'CHANGE_SCALE', data });
  };

  const LowAudio = new Audio("https://in-tune-media.s3.amazonaws.com/Low_Seiko_SQ50.wav")
  const HighAudio = new Audio("https://in-tune-media.s3.amazonaws.com/High_Seiko_SQ50.wav")

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
          <Icon
            // @ts-ignore
            name={icon}
            size='huge'
            onClick={() => {
              HighAudio.play()
              if (icon === 'play') {
                setIcon('stop')
              } else {
                setIcon('play')
              }
            }}
            style={{ cursor: 'pointer' }}
          />
        </InputsWrapper>
      </HeaderWrapper>
      <BarsWrapper>
        <ol>
          {(bars as BarState[]).map((bar, idx) => (
            <li>
              <KeyAndScaleSelectors
                key={idx}
                disableSelectors={icon === 'stop'}
                isActive={currentBar === idx}

                musicKey={bar.musicKey}
                onKeyChange={(key: string) => { handleKeyChange(idx, key) }}

                scale={bar.scale}
                onScaleChange={(scale: string) => { handleScaleChange(idx, scale) }}
              />
              {icon !== 'stop' && <Icon style={{ position: 'absolute', color: 'red', fontSize: '50px', left: '125px', top: '52.5px' }} name='remove circle' onClick={() => {
                  if (icon !== 'stop') {
                    dispatch({ idx, type: 'REMOVE_BAR', data: '' })
                  }
                }} />}
            </li>
          ))}
        </ol>
      </BarsWrapper>
        <Button
          primary
          disabled={icon === 'stop'}
          onClick={() => { dispatch({ idx: -1, type: 'ADD_BAR', data: '' }) }}
        >
          Add Bar
        </Button>
    </Wrapper>
  )
}