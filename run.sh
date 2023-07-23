#!/bin/sh
###
 # @Description: 
 # @Author: 王清哲
 # @Date: 2023-06-19 19:28:47
 # @LastEditTime: 2023-07-22 11:09:01
 # @LastEditors: 王清哲
### 
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?