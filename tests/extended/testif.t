turtle testif
//test if condition

var x = 100
var y = 100

fun ti(x, y)
{
  up
  moveto(x, y)
  down
  x = x + 100
  moveto(x, y)
  if(x < 500){
    ti(x, y)
  }
  up
}

{
  ti(100, 100)
}
