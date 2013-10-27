turtle scope
//test the scope handling
var x = 100
var y = 100

fun x (x, y)
{
  up
  moveto (x, y)
  down 
  moveto (x+100, y)
  up
}

fun y (x, y) 
{
  up
  moveto (x, y)
  down 
  moveto (x, y+100)
  up
}

{
   x (x, y)
   y (x, y)

}

