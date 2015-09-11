-----------------------------------------------------------------------------
--
-- Module      :  TreeADT
-- Copyright   :
-- License     :  AllRightsReserved
--
-- Maintainer  :
-- Stability   :
-- Portability :
--
-- |
--
-----------------------------------------------------------------------------

module TreeADT where

data Tree a = EmptyTree
    | Node a (Tree a) (Tree a) Int deriving (Show, Read, Eq)

height :: Tree a -> Int
height (Node _ _ _ height) = height


--singleton :: a -> Tree a -> Int
--singleton x = Node x EmptyTree EmptyTree height
emptyTree ::  a -> Tree a
emptyTree a = Node a EmptyTree EmptyTree height


treeInsert :: (Ord a) => a -> Tree a -> Tree a
treeInsert x EmptyTree = emptyTree x height
treeInsert x (Node a left right) height
    | x == a = Node x left right height
    | x < a  = Node a (treeInsert x left height) right height
    | x > a  = Node a left (treeInsert x right height)	height

treeElem :: (Ord a) => a -> Tree a -> Bool
treeElem x EmptyTree = False
treeElem x (Node a left right height)
    | x == a = True
    | x < a  = treeElem x left
    | x > a  = treeElem x right





