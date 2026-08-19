
import scala.util.control.Breaks._

case class Rectangle(uX: Int, uY: Int, bX: Int, bY: Int) {
  // as per 2D matrix coordinate, not cartesian plane
  def area: Int = (bX - uX + 1) * (bY - uY + 1)
  def valid: Boolean = this.area > 0 && (uX >= 0 && uY >= 0 && bX >= 0 && bY >= 0)
}

object AggClusterInfo {

  import scala.collection.mutable

  private case class Height(value: Int, index: Int)

  var rectangles: mutable.ListBuffer[Rectangle] = new mutable.ListBuffer[Rectangle]()

  def maximalRectangle(matrix: Array[Array[Int]]): Long = {
    var histogram: Array[Int] = Array.fill(matrix(0).length){0}
    var max = 0L

    for (i <- matrix.indices) {
      // update maximum height above and including cell
      for(j <- matrix(i).indices) {
        if (matrix(i)(j) > 0)
          histogram(j) += 1
        else
          histogram(j) = 0
      }
      max = Math.max(max, largestRectangle(histogram, i))
    }

    max
  }

  private def largestRectangle(heights: Array[Int], x: Int): Long = {
    def area(height: Height, currentIndex: Int): Int = (currentIndex - height.index) * height.value
    val stack = new mutable.ArrayStack[Height]
    var max   = 0L

    for (i <- heights.indices) {
      val h     = heights(i)
      var index = i

      while (stack.nonEmpty && h < stack.head.value) {
        val building = stack.pop()
        max   = Math.max(max, area(building, i))
        rectangles :+= Rectangle(x - building.value + 1, building.index, x, i - 1)
        index = building.index
      }

      if (stack.isEmpty || h > stack.head.value) stack.push(Height(h, index))
    }

    while (stack.nonEmpty) {
      val building = stack.pop()
      max = Math.max(max, area(building, heights.length))
      rectangles :+= Rectangle(x - building.value + 1, building.index, x, heights.length - 1)
    }
    max
  }


  def getRectangleCoordinates(mat: Array[Array[Int]]): Unit = {
    var flag = true
    while(flag) {
      val maxRectangleArea = maximalRectangle(mat)

      rectangles = rectangles.filter(_.valid).sortBy(-1 * _.area)
      var itr = 2
      rectangles = rectangles.filter( rectangle => {
        var overlap = (
          mat(rectangle.uX)(rectangle.uY) > 1 || mat(rectangle.uX)(rectangle.bY) > 1 || mat(rectangle.bX)(rectangle.bY) > 1
            || mat(rectangle.bX)(rectangle.uY) > 1
          )

        if(!overlap) {
          // marking rectangle as used
          for (i <- rectangle.uX until (rectangle.bX + 1); j <- rectangle.uY until (rectangle.bY + 1)) {
            mat(i)(j) = itr
          }
          itr += 1
        }
        !overlap
      })

      // removing all found rectangles with non overlapping boundaries
      for (i <- mat.indices; j <- mat(0).indices; if mat(i)(j) > 1) mat(i)(j) = 0

      //  checking all 1 s are covered or not
      flag = false
      for (i <- mat.indices; j <- mat(0).indices; if mat(i)(j) == 1) flag = true
    }
  }

  def solve(mat: Array[Array[Int]]): Array[Rectangle] = {
    rectangles.clear()

    println("Original Matrix:")
    mat.foreach(arr => println(arr.toSeq))

    getRectangleCoordinates(mat)

    // Mark rectangles
    var itr = 2
    rectangles.foreach( rectangle => {
      for (i <- rectangle.uX until (rectangle.bX + 1); j <- rectangle.uY until (rectangle.bY + 1)) {
        mat(i)(j) = itr
      }
      itr += 1
    })

    println("Final Matrix:")
    mat.foreach(arr => println(arr.toSeq))
    rectangles.toArray
  }
}
