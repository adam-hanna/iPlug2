import React from 'react'
import styled from 'styled-components'
import { Input } from 'semantic-ui-react'

const BoldLabel = styled.label`
  font-weight: bold;
`

const Wrapper = styled.div`
  display: inline-flex;
  flex-direction: column;
  align-items: baseline;
`

export type SemitonesProps = {
  disable: boolean;
  value: string;
  onChange: (semitone: string) => void;
}

export const Semitones = ({
  disable,
  value,
  onChange
}: SemitonesProps) => {

  return (
    <Wrapper>
      <BoldLabel style={{ color: disable ? '#ccc' : 'black' }}>Semitones:</BoldLabel>
      <Input
        disabled={disable}
        value={value}
        onChange={e => {
          onChange((e.target as HTMLInputElement).value || '[]')
        }}
        placeholder='Semitones'
      />
    </Wrapper>
  )
}

