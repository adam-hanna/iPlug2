import React, { useEffect, useState, useReducer } from 'react'
import { Button, Icon, Modal } from 'semantic-ui-react'
import styled from 'styled-components'
import { ScaleType, Interval } from "@tonaljs/tonal"

import { KeysToRootNoteMap } from '../../Keys'
import { KeyAndScaleSelectors } from '../../Components/KeyAndScaleSelectors'
import { ScaleSelector } from '../../Components/Selectors/Scales'
import {
  SET_BARS,
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

const BarsWrapper = styled.div`
  position: relative;
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;

  max-width: 1200px;

  padding-bottom: 5px;
`

const ListIcon = styled(Icon)`
  position: static;

  color: red;
  font-size: 3em !important;

  padding-top: 10px;
  margin-top: 10px !important;
`

type ListItemProps = {
  isCurrentBar: boolean;
}

const ListItem = styled.li`
  display: flex;
  align-items: center;
  justify-content: center;

  background-color: ${(props: ListItemProps) => props.isCurrentBar ? 'rgba(255, 255, 0, 1)' : 'rgba(255, 255, 0, 0)' };

  cursor: pointer;

  padding-bottom: 5px;
  padding-right: 5px;
`

type BarState = {
  musicKey: string;
  scale: string;
}

const InitialKey = "C"
const InitialScale = "major"

const initialBars: BarState[] = [
  {
    musicKey: InitialKey,
    scale: InitialScale
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
      return state.length === 8 ?
        [...state] :
        [...state, { musicKey: InitialKey, scale: InitialScale }];

    case "REMOVE_BAR":
      return state.length === 1 ?
        [...state] :
        [...state].filter((bar, idx) => {
          return idx !== action.idx
        });

    default:
      return state;
  }
}

export const Manual = (_props: Props) => {
  const [bars, dispatch] = useReducer(
    barReducer,
    initialBars,
  );
  const [currentBar, _setCurrentBar] = useState(0)

  const [scaleModal, setScaleModal] = useState(false)
  const [scale, setScale] = useState('Major')

  const handleKeyChange = (idx: number, data: string) => {
    dispatch({ idx, type: 'CHANGE_KEY', data });
  };
  const handleScaleChange = (idx: number, data: string) => {
    dispatch({ idx, type: 'CHANGE_SCALE', data });
  };

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

  useEffect(() => {
    if (NODE_ENV === 'production') {
      let arr: Array<number> = []
      for (const { musicKey, scale } of bars) {
        const keyIDX = KeysToRootNoteMap[musicKey]
        const { intervals } = ScaleType.get(scale)
        let semitones: Array<number> = []
        intervals.forEach(interval => {
          const intrvl = Interval.semitones(interval)
          if (intrvl) {
            semitones.push(intrvl)
          }
        })

        arr = [...arr, keyIDX, ...semitones, -1]
      }

      SAMFUI(SET_BARS, -1, Buffer.from(arr).toString('base64'));
    }
  }, [bars])

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
      </HeaderWrapper>
      <BarsWrapper>
        <ol>
          {(bars as BarState[]).map((bar, idx) => (
            <ListItem
              key={idx}
              isCurrentBar={currentBar === idx}
            >
              {
                <ListIcon
                  title='Remove'
                  name='remove circle'
                  onClick={() => {
                    dispatch({ idx, type: 'REMOVE_BAR', data: '' })
                  }}
                />
              }
              <KeyAndScaleSelectors
                key={idx}
                disableSelectors={false}
                isActive={false}

                musicKey={bar.musicKey}
                onKeyChange={(key: string) => { handleKeyChange(idx, key) }}

                scale={bar.scale}
                onScaleChange={(scale: string) => { handleScaleChange(idx, scale) }}
              />
            </ListItem>
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