import React from 'react'
import styled from 'styled-components'
import { Dropdown } from 'semantic-ui-react'
import { Scale } from "@tonaljs/tonal"

const BoldLabel = styled.label`
  font-weight: bold;
`

const Wrapper = styled.div`
  display: inline-flex;
  flex-direction: column;
  align-items: baseline;
`

export type ScaleSelectorProps = {
  disable: boolean;
  value: string;
  onChange: (key: string) => void;
}

export const ScaleSelector = ({
  disable,
  value,
  onChange
}: ScaleSelectorProps) => {
  let scales = Scale.names().sort().map(name => {
    return {
      key: name,
      value: name,
      text: name
    }
  })
  scales.push({
    key: "custom",
    value: "custom",
    text: "custom"
  })

  return (
    <Wrapper>
      <BoldLabel style={{ color: disable ? '#ccc' : 'black' }}>Scale:</BoldLabel>
      <Dropdown
        search
        selection
        disabled={disable}
        value={value}
        onChange={e => {
          onChange((e.target as HTMLDivElement).textContent || '')
        }}
        placeholder='Select the scale'
        options={scales}
      />
    </Wrapper>
  )
}
