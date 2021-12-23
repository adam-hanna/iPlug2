import React from 'react';
import 'semantic-ui-css/semantic.min.css'
import styled from 'styled-components'
import { Tab } from 'semantic-ui-react'

import './App.css';
import { Automatic } from './Pages/Automatic';
import { Manual } from './Pages/Manual';

const Wrapper = styled.div`
  display: flex;
  flex-direction: column;
  align-items: center;
`

function App() {
  const panes = [
    { menuItem: 'Auto', render: () => <Automatic /> },
    { menuItem: 'Manual', render: () => <Manual /> },
  ]

  return (
    <Wrapper className="App">
      <Tab panes={panes} />
    </Wrapper>
  );
}

export default App;
