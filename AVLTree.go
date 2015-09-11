package main

import (
	"container/list"
	"fmt"
	"io"
	"math/rand"
	"os"
	"runtime"
	"strconv"
	"sync"
	"time"
)

var nodes int = 0

type AVLnode struct {
	val    int
	left   *AVLnode
	right  *AVLnode
	height int
	sync.Mutex
}

//(a avlnode) makes it part of the 'class'

func insert(tree *AVLnode, insertthis int) *AVLnode {
	if tree == nil {
		var temp *AVLnode = &AVLnode{val: insertthis, left: nil, right: nil, height: 0}
		return temp
	} else {
		if tree.val < insertthis {
			tree.right = insert(tree.right, insertthis)
		} else {
			tree.left = insert(tree.left, insertthis)
		}
	}
	return tree
}

func AVLinsert(tree *AVLnode, toins int) {

	if tree != nil && nodes < 1000000 {
		tree.Lock()

		insertnode(tree, toins)
		nodes++
		tree.Unlock()
	}

}

func insertnode(tree *AVLnode, toins int) {
	if tree != nil {
		//tree.Lock()

		if toins > tree.val {
			if tree.right != nil {
				insertnode(tree.right, toins)
				tree.right = rebalance(tree.right)
				//
			} else {
				tree.right = &AVLnode{val: toins, left: nil, right: nil}
			}
		} else {

			if tree.left != nil {
				insertnode(tree.left, toins)
				tree.left = rebalance(tree.left)
				//
			} else {
				tree.left = &AVLnode{val: toins, left: nil, right: nil}
			}
		}
		//tree.Unlock()
	}
}

func delete(tree *AVLnode, todel *AVLnode) *AVLnode {

	var temp *AVLnode = findparent(tree, todel)
	if isleaf(todel) {
		if temp.left == todel {
			temp.left = nil
		} else {
			temp.right = nil
		}

	} else if onlyleftchild(todel) {
		if temp.left != nil && temp.left == todel {
			temp.left = todel.left
		} else {
			temp.right = todel.left
		}
	} else if onlyrightchild(todel) {
		if temp.left != nil && temp.left == todel {
			temp.left = todel.right
		} else {
			temp.right = todel.right
		}
	} else { //It has two children. the terror!
		var maxleft *AVLnode = findmax(todel.left)
		var minright *AVLnode = findmin(todel.right)

		//true for right false for left
		if maxleft.val < minright.val {
			todel.val = minright.val
			return delete(tree, minright)
		} else {
			todel.val = maxleft.val
			return delete(tree, maxleft)
		}

	}

	return tree

}

func AVLdelete(tree *AVLnode, todel int) *AVLnode {

	if tree == nil {
		return nil
	}

	tree.Lock()

	if todel < tree.val {
		tree.left = AVLdelete(tree.left, todel)
	} else if todel > tree.val {
		tree.right = AVLdelete(tree.right, todel)
	} else {
		var temp1 *AVLnode = tree.left
		var temp2 *AVLnode = tree.right
		tree.Unlock()
		tree = nil
		runtime.GC()
		if temp2 == nil {
			//tree.Unlock()
			return temp1
		}
		var minnode *AVLnode = findmin(temp2)
		minnode.right = removemin(temp2)
		minnode.left = temp1
		//tree.Unlock()
		return rebalance(minnode)
	}
	tree.Unlock()
	return tree
}

func inord(tree *AVLnode) {
	if tree != nil {
		inord(tree.left)
		fmt.Println(tree.val)
		inord(tree.right)
	}
}

func chaninord(tree *AVLnode, comm chan string, temp string) {
	if tree != nil {
		inord(tree.left)
		temp += string(tree.val) + " "
		inord(tree.right)
	}

	comm <- temp
}

func preord(tree *AVLnode) {
	if tree != nil {
		fmt.Println(tree.val)
		inord(tree.left)
		inord(tree.right)
	}
}

func findnode(tree *AVLnode, tofind int) *AVLnode {

	if tree != nil {
		//tree.Lock()
		if tree.val == tofind {
			return tree
		} else if tofind < tree.val {
			return findnode(tree.left, tofind)
		} else {
			return findnode(tree.right, tofind)
		}
		//tree.Unlock()
	}

	return nil

}

func findmin(tree *AVLnode) *AVLnode {
	if tree.left == nil && tree.right == nil {
		return tree
	} else if tree.left != nil {
		return findmin(tree.left)
	} else {
		return tree
	}
	return nil

}

func removemin(tree *AVLnode) *AVLnode {
	if tree.left == nil {
		return tree.right
	}
	tree.left = removemin(tree.left)
	return rebalance(tree)
}

func findmax(tree *AVLnode) *AVLnode {
	if tree.left == nil && tree.right == nil {
		return tree
	} else if tree.right != nil {
		return findmax(tree.right)
	} else {
		return tree
	}
	return nil
}

func findparent(tree *AVLnode, anode *AVLnode) *AVLnode {
	if tree == nil || anode == tree {
		return nil
	} else if tree.left == anode || tree.right == anode {
		return tree
	} else if anode.val < tree.val {
		return findparent(tree.left, anode)
	} else if anode.val > tree.val {
		return findparent(tree.right, anode)
	}
	return nil

}

func isleaf(tree *AVLnode) bool {
	if tree.left == nil && tree.right == nil {
		return true
	}

	return false
}

func onlyleftchild(tree *AVLnode) bool {
	if tree.right == nil && tree.left != nil {
		return true
	} else {
		return false
	}

	return false

}

func onlyrightchild(tree *AVLnode) bool {
	if tree.right != nil && tree.left == nil {
		return true
	} else {
		return false
	}

	return false

}

func balancefactor(tree *AVLnode) int {
	return height(tree.right) - height(tree.left)
}

func height(tree *AVLnode) int {
	if tree == nil {
		return 0
	}
	return tree.height
}

func fixheight(tree *AVLnode) {
	var hleft int = height(tree.left)
	var hright int = height(tree.right)
	var temp int

	if hleft > hright {
		temp = hleft
	} else {
		temp = hright
	}

	tree.height = temp + 1

}

func rotateright(p *AVLnode) *AVLnode {
	/*var q *AVLnode = p.left
	p.left = q.right
	q.right = p
	fixheight(p)
	fixheight(q)
	return q
	*/
	if p != nil {

		var q *AVLnode = p.left
		if q != nil {
			p.left = q.right
			q.right = p
		}

		if p != nil {
			fixheight(p)
		}

		if q != nil {
			fixheight(q)
		}

		return q
	}

	return nil

}

func rotateleft(q *AVLnode) *AVLnode {
	if q != nil {

		var p *AVLnode = q.right
		if p != nil {
			q.right = p.left
			p.left = q
		}

		if q != nil {
			fixheight(q)
		}

		if p != nil {
			fixheight(p)
		}

		return p
	}

	return nil

}

func rebalance(tree *AVLnode) *AVLnode {

	if tree != nil {
		fixheight(tree)

		if balancefactor(tree) == 2 {
			if tree.right != nil && balancefactor(tree.right) < 0 {
				tree.right = rotateright(tree.right)
			}
			return rotateleft(tree)
		}

		if balancefactor(tree) == -2 {
			if tree.left != nil && balancefactor(tree.left) > 0 {
				tree.left = rotateleft(tree.left)
			}
			return rotateleft(tree)
		}
	}

	return tree

}

func printBFS(tree *AVLnode) {
	myqu := list.New()
	myqu.PushBack(tree)
	for e := myqu.Front(); e != nil; e = e.Next() {
		var temp *AVLnode = e.Value.(*AVLnode)
		fmt.Println(temp.val)
		if temp.left != nil {
			myqu.PushBack(temp.left)
		}
		if temp.right != nil {
			myqu.PushBack(temp.right)
		}
	}

}

func dowop(whichones int, treesize int, comm chan string, wg *sync.WaitGroup) {

	//r := rand.New(rand.NewSource(time.Now().UnixNano()))
	//fmt.Println(r.Int() % 10000)
	//wg.Add(1)
	var x *AVLnode = &AVLnode{val: rand.Intn(treesize), left: nil, right: nil}

	for i := 0; i < treesize-1; i++ {
		insert(x, rand.Intn(treesize))
	}

	//comm <- chaninord(x, comm, "")
	inord(x)

	if whichones == 1 {
		//only search
		//fmt.Println("no!")
		comm <- "no!"

	} else if whichones == 2 {
		fmt.Println("yes!")
		//insert and delete

	} else if whichones == 3 {
		//all three
		fmt.Println("Maybe!")

	}

	wg.Done()

}

func createrandtree(size int, maxval int) *AVLnode {

	var x *AVLnode = &AVLnode{val: rand.Intn(maxval), left: nil, right: nil}

	for i := 0; i < size-1; i++ {
		AVLinsert(x, rand.Intn(maxval))
	}

	return x

}

func donAVGOps(nops int, maxval int, tree *AVLnode, comm chan bool) {

	//defer wait.Done()

	for i := 0; i < nops; i++ {
		var randomnum int = rand.Intn(100)
		if randomnum == 0 {
			AVLdelete(tree, rand.Intn(maxval))
			//fmt.Println("del")
		}

		if randomnum <= 9 && randomnum > 0 {
			AVLinsert(tree, rand.Intn(maxval))
			//fmt.Println("ins")
		}

		if randomnum > 9 {
			//fmt.Println("Do nothing.")
			//AVLdelete(tree, rand.Intn(maxval))
			findnode(tree, rand.Intn(maxval))
			//fmt.Println("find")
		}

	}

	comm <- true

	//fmt.Println("-1")

}

func dospecificOps(nops int, maxval int, tree *AVLnode, anOp int, wg *sync.WaitGroup) {

	for i := 0; i < nops; i++ {

		if anOp == 0 {
			AVLdelete(tree, rand.Intn(maxval))
		}

		if anOp == 1 {
			AVLinsert(tree, rand.Intn(maxval))
		}

		if anOp == 2 {
			//fmt.Println("Do nothing.")
			//AVLdelete(tree, rand.Intn(maxval))
			findnode(tree, rand.Intn(maxval))
		}

	}

}

func main() {

	filename := "output.txt"

	file, err := os.Create(filename)

	if err != nil {
		fmt.Println(err)
	}

	var towrite string = " "

	for z := 0; z < 7; z++ {
		var threads int
		switch z {
		case 0:
			threads = 1
		case 1:
			threads = 2
		case 2:
			threads = 4
		case 3:
			threads = 8
		case 4:
			threads = 16
		case 5:
			threads = 32
		case 6:
			threads = 64

		}

		towrite += "\nThreads: " + strconv.Itoa(threads)

		//fmt.Println(threads)

		for iters := 0; iters < 10; iters++ {

			var a *AVLnode = &AVLnode{val: -1, left: nil, right: nil, height: 0}

			teststart := time.Now()

			done := make(chan bool)

			var numthreads int = threads
			for i := 0; i < numthreads; i++ {
				var opstodivy int = 100000000
				go donAVGOps(opstodivy/numthreads, 1000000000, a, done)

			}

			for j := 0; j < numthreads; j++ {
				<-done
				//fmt.Println("done.")
			}

			testtime := time.Since(teststart)

			a = nil

			//fmt.Println("time taken:", testtime.Seconds())

			towrite += fmt.Sprintf(" %.6f ", testtime.Seconds()) 

		}

	}

	fmt.Println(towrite)

	n, err := io.WriteString(file, towrite)

	if err != nil {
		fmt.Println(n, err)
	}

	file.Close()

}
