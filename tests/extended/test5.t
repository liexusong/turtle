// Siyuan Zhang, u4761413
// Meitian Huang, u4700480
turtle test5

var x = 0
var y = 100
var s = 100

fun x (x, y, s)
{
  while (x < s){
    x = x + 10
    y (x, y)
    }
}

fun y (x, y)
{
  up
  moveto(x, y)
  down
  moveto(x, y - 10)
  up
}

{
  x (x, y, s)
}
