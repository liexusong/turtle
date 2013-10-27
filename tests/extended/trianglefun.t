turtle triangle
//Draw a triangle

var x = 100
var y = 100
var scale = 100

fun tri(x, y, scale)
{
  up
  moveto (x, y)
  down
  moveto (x+scale, y)
  moveto (x, y+100)
  moveto (x, y)
  up
}

//just draw a triangle
{
  tri(x, y, scale)
}
