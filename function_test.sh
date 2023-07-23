#!/bin/sh
###
 # @Description: 
 # @Author: 王清哲
 # @Date: 2023-07-23 19:02:50
 # @LastEditTime: 2023-07-23 19:19:44
 # @LastEditors: 王清哲
### 
# 文件名
f38="./test/functionalc/38_op_priority4.in"
f39="./test/functionalc/39_op_priority5.in"
f40="./test/functionalc/40_unary_op.in"
f41="./test/functionalc/41_unary_op2.in"
f42="./test/functionalc/42_empty_stmt.in"
f43="./test/functionalc/43_logi_assign.in"
f44="./test/functionalc/44_stmt_expr.in"
f45="./test/functionalc/45_comment1.in"
f46="./test/functionalc/46_hex_defn.in"
f47="./test/functionalc/47_hex_oct_add.in"
f48="./test/functionalc/48_assign_complex_expr.in"
f49="./test/functionalc/49_if_complex_expr.in"
f50="./test/functionalc/50_short_circuit.in"
f51="./test/functionalc/51_short_circuit3.in"
f52="./test/functionalc/52_scope.in"
f53="./test/functionalc/53_scope2.in"
f54="./test/functionalc/54_hidden_var.in"
f55="./test/functionalc/55_sort_test1.in"
f56="./test/functionalc/56_sort_test2.in"
f57="./test/functionalc/57_sort_test3.in"
f58="./test/functionalc/58_sort_test4.in"
f59="./test/functionalc/59_sort_test5.in"
f60="./test/functionalc/60_sort_test6.in"
f61="./test/functionalc/61_sort_test7.in"
f62="./test/functionalc/62_percolation.in"
f63="./test/functionalc/63_big_int_mul.in"
f64="./test/functionalc/64_calculator.in"
f65="./test/functionalc/65_color.in"
f66="./test/functionalc/66_exgcd.in"
f67="./test/functionalc/67_reverse_output.in"
f68="./test/functionalc/68_brainfk.in"
f69="./test/functionalc/69_expr_eval.in"
f70="./test/functionalc/70_dijkstra.in"
f71="./test/functionalc/71_full_conn.in"
f72="./test/functionalc/72_hanoi.in"
f73="./test/functionalc/73_int_io.in"
f74="./test/functionalc/74_kmp.in"
f75="./test/functionalc/75_max_flow.in"
f76="./test/functionalc/76_n_queens.in"
f77="./test/functionalc/77_substr.in"
f78="./test/functionalc/78_side_effect.in"
f79="./test/functionalc/79_var_name.in"
f80="./test/functionalc/80_chaos_token.in"
f81="./test/functionalc/81_skip_spaces.in"
f82="./test/functionalc/82_long_func.in"
f83="./test/functionalc/83_long_array.in"
f84="./test/functionalc/84_long_array2.in"
f85="./test/functionalc/85_long_code.in"
f86="./test/functionalc/86_long_code2.in"
f87="./test/functionalc/87_many_params.in"
f88="./test/functionalc/88_many_params2.in"
f89="./test/functionalc/89_many_globals.in"
f90="./test/functionalc/90_many_locals.in"
f91="./test/functionalc/91_many_locals2.in"
f92="./test/functionalc/92_register_alloc.in"
f93="./test/functionalc/93_nested_calls.in"
f94="./test/functionalc/94_nested_loops.in"
f95="./test/functionalc/95_float.in"
f96="./test/functionalc/96_matrix_add.in"
f97="./test/functionalc/97_matrix_sub.in"
f98="./test/functionalc/98_matrix_mul.in"
f99="./test/functionalc/99_matrix_tran.in"
echo '#################0#################'
./test.sh 00
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################1#################'
./test.sh 01
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################2#################'
./test.sh 02
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################3#################'
./test.sh 03
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################4#################'
./test.sh 04
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################5#################'
./test.sh 05
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################6#################'
./test.sh 06
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################7#################'
./test.sh 07
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################8#################'
./test.sh 08
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################9#################'
./test.sh 09
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################10#################'
./test.sh 10
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################11#################'
./test.sh 11
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################12#################'
./test.sh 12
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################13#################'
./test.sh 13
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################14#################'
./test.sh 14
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################15#################'
./test.sh 15
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################16#################'
./test.sh 16
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################17#################'
./test.sh 17
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################18#################'
./test.sh 18
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################19#################'
./test.sh 19
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################20#################'
./test.sh 20
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################21#################'
./test.sh 21
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################22#################'
./test.sh 22
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################23#################'
./test.sh 23
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################24#################'
./test.sh 24
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################25#################'
./test.sh 25
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################26#################'
./test.sh 26
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################27#################'
./test.sh 27
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################28#################'
./test.sh 28
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################29#################'
./test.sh 29
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################30#################'
./test.sh 30
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################31#################'
./test.sh 31
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################32#################'
./test.sh 32
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################33#################'
./test.sh 33
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################34#################'
./test.sh 34
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################35#################'
./test.sh 35
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################36#################'
./test.sh 36
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################37#################'
./test.sh 37
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################38#################'
./test.sh 38
echo "reference:"
qemu-arm ./test/ref.out < $f38
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f38
echo $?
echo '####################################'
echo '#################39#################'
./test.sh 39
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################40#################'
./test.sh 40
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################41#################'
./test.sh 41
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################42#################'
./test.sh 42
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################43#################'
./test.sh 43
echo "reference:"
qemu-arm ./test/ref.out < $f43
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f43
echo $?
echo '####################################'
echo '#################44#################'
./test.sh 44
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################45#################'
./test.sh 45
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################46#################'
./test.sh 46
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################47#################'
./test.sh 47
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################48#################'
./test.sh 48
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################49#################'
./test.sh 49
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################50#################'
./test.sh 50
echo "reference:"
qemu-arm ./test/ref.out < $f50
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f50
echo $?
echo '####################################'
echo '#################51#################'
./test.sh 51
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################52#################'
./test.sh 52
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################53#################'
./test.sh 53
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################54#################'
./test.sh 54
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################55#################'
./test.sh 55
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################56#################'
./test.sh 56
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################57#################'
./test.sh 57
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################58#################'
./test.sh 58
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################59#################'
./test.sh 59
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################60#################'
./test.sh 60
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################61#################'
./test.sh 61
echo "reference:"
qemu-arm ./test/ref.out < $f61
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f61
echo $?
echo '####################################'
echo '#################62#################'
./test.sh 62
echo "reference:"
qemu-arm ./test/ref.out < $f62
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f62
echo $?
echo '####################################'
echo '#################63#################'
./test.sh 63
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################64#################'
./test.sh 64
echo "reference:"
qemu-arm ./test/ref.out < $f64
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f64
echo $?
echo '####################################'
echo '#################65#################'
./test.sh 65
echo "reference:"
qemu-arm ./test/ref.out < $f65
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f65
echo $?
echo '####################################'
echo '#################66#################'
./test.sh 66
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################67#################'
./test.sh 67
echo "reference:"
qemu-arm ./test/ref.out < $f67
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f67
echo $?
echo '####################################'
echo '#################68#################'
./test.sh 68
echo "reference:"
qemu-arm ./test/ref.out < $f68
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f68
echo $?
echo '####################################'
echo '#################69#################'
./test.sh 69
echo "reference:"
qemu-arm ./test/ref.out < $f69
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f69
echo $?
echo '####################################'
echo '#################70#################'
./test.sh 70
echo "reference:"
qemu-arm ./test/ref.out < $f70
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f70
echo $?
echo '####################################'
echo '#################71#################'
./test.sh 71
echo "reference:"
qemu-arm ./test/ref.out < $f71
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f71
echo $?
echo '####################################'
echo '#################72#################'
./test.sh 72
echo "reference:"
qemu-arm ./test/ref.out < $f72
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f72
echo $?
echo '####################################'
echo '#################73#################'
./test.sh 73
echo "reference:"
qemu-arm ./test/ref.out < $f73
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f73
echo $?
echo '####################################'
echo '#################74#################'
./test.sh 74
echo "reference:"
qemu-arm ./test/ref.out < $f74
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f74
echo $?
echo '####################################'
echo '#################75#################'
./test.sh 75
echo "reference:"
qemu-arm ./test/ref.out < $f75
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f75
echo $?
echo '####################################'
echo '#################76#################'
./test.sh 76
echo "reference:"
qemu-arm ./test/ref.out < $f76
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f76
echo $?
echo '####################################'
echo '#################77#################'
./test.sh 77
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################78#################'
./test.sh 78
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################79#################'
./test.sh 79
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################80#################'
./test.sh 80
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################81#################'
./test.sh 81
echo "reference:"
qemu-arm ./test/ref.out < $f81
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f81
echo $?
echo '####################################'
echo '#################82#################'
./test.sh 82
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################83#################'
./test.sh 83
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################84#################'
./test.sh 84
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################85#################'
./test.sh 85
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################86#################'
./test.sh 86
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################87#################'
./test.sh 87
echo "reference:"
qemu-arm ./test/ref.out < $f87
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f87
echo $?
echo '####################################'
echo '#################88#################'
./test.sh 88
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################89#################'
./test.sh 89
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################90#################'
./test.sh 90
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################91#################'
./test.sh 91
echo "reference:"
qemu-arm ./test/ref.out < $f91
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f91
echo $?
echo '####################################'
echo '#################92#################'
./test.sh 92
echo "reference:"
qemu-arm ./test/ref.out < $f92
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f92
echo $?
echo '####################################'
echo '#################93#################'
./test.sh 93
echo "reference:"
qemu-arm ./test/ref.out < $f93
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f93
echo $?
echo '####################################'
echo '#################94#################'
./test.sh 94
echo "reference:"
qemu-arm ./test/ref.out < $f94
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f94
echo $?
echo '####################################'
echo '#################95#################'
./test.sh 95
echo "reference:"
qemu-arm ./test/ref.out < $f95
echo $?
echo "mine:"
qemu-arm ./test/mine.out < $f95
echo $?
echo '####################################'
echo '#################96#################'
./test.sh 96
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################97#################'
./test.sh 97
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################98#################'
./test.sh 98
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
echo '#################99#################'
./test.sh 99
echo "reference:"
qemu-arm ./test/ref.out
echo $?
echo "mine:"
qemu-arm ./test/mine.out
echo $?
echo '####################################'
