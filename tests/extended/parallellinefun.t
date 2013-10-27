turtle Parallelline
//Draw parallel lines


var x = 200
var y = 200
var scale = 100

fun pll(x, y, scale)
{
  up
  moveto (x, y)
  down
  moveto (x+scale, y+scale)
  up
  moveto (x+2*scale, y+scale)
  down
  moveto (x+scale, y)
  up
}

{
  pll(x, y, scale)
}
