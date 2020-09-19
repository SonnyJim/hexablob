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

let color = '#00FF00';

$(document).ready(() => {

  // Colour picker
  $('#colorpicker').colpick({
    colorScheme:'dark',
    onChange:function(hsb,hex,rgb,el,bySetColor) {
        const c = '#'+hex
        color = c
    }
  })

  // Colour cells
  const cells = Array.from($('li div'))
  cells.forEach((cell, i) => {
    if (cellMap[i] > -1) {
      $(cell).css('background-color', '#fdbf00')
    } else {
      $(cell).css('background-color', 'black')
    }
  });


  // Mouse over event
  $('li div').on('mouseenter', (e) => {
    const cell = $(e.target)
    const key = cell.attr('data-key')
    const light = cellMap[key]
    if (e.shiftKey && light > -1) {
      cell.css('background-color', color)
    }
    console.log(light);
  })

  //
})
