import React from 'react'
import styled from 'styled-components'

import { ScaleSelector } from '../Selectors/Scales'
import { KeySelector } from '../Selectors/Keys'
import { Semitones } from '../Selectors/Semitones'

export type Props = {
  disableSelectors: boolean;
  isActive: boolean;

  musicKey: string;
  onKeyChange: (key: string) => void;

  scale: string;
  onScaleChange: (scale: string) => void;

  semitones: string;
  onSemitonesChange: (semitone: string) => void;
}

const Wrapper = styled.div`
  display: inline-flex;
  flex-direction: row;
  justify-content: center;
`

export const KeyAndScaleSelectors = ({
  isActive,
  disableSelectors,
  musicKey,
  onKeyChange,
  scale,
  onScaleChange,
  semitones,
  onSemitonesChange,
}: Props) => {
  return (
    <Wrapper style={{ backgroundColor: `rgba(255, 255, 0, ${isActive ? 1 : 0})` }}>
      <KeySelector
        disable={disableSelectors}
        value={musicKey}
        onChange={onKeyChange}
      />
      <ScaleSelector
        disable={disableSelectors}
        value={scale}
        onChange={onScaleChange}
      />
      <Semitones
        disable={scale.toLocaleLowerCase() !== "custom"}
        value={semitones}
        onChange={onSemitonesChange}
      />
    </Wrapper>
  )
}