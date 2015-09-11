package com.scalaapp

/**
 * Created by Paul on 3/12/2015.
 */

import java.io._


abstract class bTree {

  case object EmptyTree extends bTree

  case class Nodeb(value: Int, left : bTree, right : bTree) extends bTree




  def getval(t : bTree): Int = t match {
    case EmptyTree => 0
    case Nodeb(value, _, _) => value
  }

}


object ScalaApp extends bTree{
  def main(args: Array[String]) {

    println("Hello, world!")
    val writer = new PrintWriter("myfile.txt")
    writer.write("Writing line for line" + util.Properties.lineSeparator)
    writer.write("Another line here" + util.Properties.lineSeparator)
    writer.close()

    println("about to print \n")
    val tree = new Nodeb(6, Nodeb(4, EmptyTree, EmptyTree), Nodeb(7, EmptyTree, EmptyTree))
   // val theval = tree.getval(tree)
    println(tree)
  }
}



