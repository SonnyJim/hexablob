const cellMap = [
  -1, -1,  4,  3,  2,  1,  0, -1, -1,
  -1,  5,  6,  7,  8,  9, 10, -1, -1,
  -1, 17, 16, 15, 14, 13, 12, 11, -1,
  18, 19, 20, 21, 22, 23, 24, 25, -1,
  34, 33, 32, 31, 30, 29, 28, 27, 26,
  35, 36, 37, 38, 39, 40, 41, 42, -1,
  -1, 49, 48, 47, 46, 45, 44, 43, -1,
  -1, 50, 51, 52, 53, 54, 55, -1, -1,
  -1, -1, 60, 59, 58, 57, 56, -1, -1,
]

let state = {
  palette: [],
  color: '#00FF00'
}

function setState(_state) {
  console.log('state', _state);
  state = _state
}

function getState() {
  return state
}

function resetCells(color) {
  const cells = Array.from($('li div'))
  cells.forEach((cell, i) => {
    if (cellMap[i] > -1) {
      $(cell).css('background-color', color)
    } else {
      $(cell).css('background-color', 'black')
    }
  });
}

function writeOut() {
  const rgb = Array.from($('li div'))
    .map(x => $(x).css('background-color'))
    .map(x => x.replace('rgb(', '['))
    .map(x => x.replace(')',']'))

  const out = Array(61)
  rgb.forEach((color, i) => {
    if (cellMap[i] < 0) return
    out[cellMap[i]] = color
  });

  $('#lights-output').text(out.join(', '))
}

$(document).ready(() => {

  // Initialise cells
  resetCells('#fdbf00')

  // Initialise palette
  $($('.palette')[8]).css('background-color', '#fdbf00')
  setState({
    ...getState(),
    color: '#fdbf00'
  })

  // Reset cells button
  $('#reset').click(e => {
    resetCells($($('.palette')[8]).css('background-color'))
    writeOut()
  })

  // Colour picker
  $('.palette').click(e => {
    const palette = $(e.target)
    const color = palette.css('background-color')
    setState({
      palette: Array.from($('.palette')).map(x => $(x).css('background-color')),
      color
    })

    palette.colpick({
      colorScheme:'dark',
      onChange:(hsb, hex, rgb, el, bySetColor) => {
        const color = '#'+hex
        $(el).css('background-color', color)
        setState({
          palette: Array.from($('.palette')).map(x => $(x).css('background-color')),
          color
        })
      },
      submit: false
    })
  })

  // Color cell event
  $('li div').on('click', (e) => {
    const cell = $(e.target)
    const key = cell.attr('data-key')
    const light = cellMap[key]
    if (light > -1) {
      const color = getState()['color']
      cell.css('background-color', color)
    }
    writeOut()
  })
})
