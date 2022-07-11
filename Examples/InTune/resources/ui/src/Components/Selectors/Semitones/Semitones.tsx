import React, { useState, useEffect } from 'react'
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
  value: Array<number>;
  onChange: (semitone: Array<number>) => void;
}

export const Semitones = ({
  disable,
  value,
  onChange
}: SemitonesProps) => {
  const [tmpState, onChangeTmpState] = useState(JSON.stringify(value))

  useEffect(() => {
    onChangeTmpState(JSON.stringify(value))
  }, [value])

  useEffect(() => {
    if (tmpState.charAt(tmpState.length - 2) === ",") {
      return
    }

    try {
      onChange(JSON.parse(tmpState.replace(",]", "]")))
    } catch (e) {
      console.error(e)
    }
  }, [tmpState])

  return (
    <Wrapper>
      <BoldLabel style={{ color: disable ? '#ccc' : 'black' }}>Semitones:</BoldLabel>
      <Input
        disabled={disable}
        value={tmpState}
        onChange={e => {
          onChangeTmpState((e.target as HTMLInputElement).value || '[]')
        }}
        placeholder='Semitones'
      />
    </Wrapper>
  )
}

