#!/bin/sh
ftest="../test.c"
f00="00_main.c"
f01="01_var_defn2.c"
f02="02_var_defn3.c"
f03="03_arr_defn2.c"
f04="04_arr_defn3.c"
f05="05_arr_defn4.c"
f06="06_const_var_defn2.c"
f07="07_const_var_defn3.c"
f08="08_const_array_defn.c"
f09="09_func_defn.c"
f10="10_var_defn_func.c"
f11="11_add2.c"
f12="12_addc.c"
f13="13_sub2.c"
f14="14_subc.c"
f15="15_mul.c"
f16="16_mulc.c"
f17="17_div.c"
f18="18_divc.c"
f19="19_mod.c"
f20="20_rem.c"
f21="21_if_test2.c"
f22="22_if_test3.c"
f23="23_if_test4.c"
f24="24_if_test5.c"
f25="25_while_if.c"
f26="26_while_test1.c"
f27="27_while_test2.c"
f28="28_while_test3.c"
f29="29_break.c"
f30="30_continue.c"
f31="31_while_if_test1.c"
f32="32_while_if_test2.c"
f33="33_while_if_test3.c"
f34="34_arr_expr_len.c"
f35="35_op_priority1.c"
f36="36_op_priority2.c"
f37="37_op_priority3.c"
f38="38_op_priority4.c"
f39="39_op_priority5.c"
f40="40_unary_op.c"
f41="41_unary_op2.c"
f42="42_empty_stmt.c"
f43="43_logi_assign.c"
f44="44_stmt_expr.c"
f45="45_comment1.c"
f46="46_hex_defn.c"
f47="47_hex_oct_add.c"
f48="48_assign_complex_expr.c"
f49="49_if_complex_expr.c"
f50="50_short_circuit.c"
f51="51_short_circuit3.c"
f52="52_scope.c"
f53="53_scope2.c"
f54="54_hidden_var.c"
f55="55_sort_test1.c"
f56="56_sort_test2.c"
f57="57_sort_test3.c"
f58="58_sort_test4.c"
f59="59_sort_test5.c"
f60="60_sort_test6.c"
f61="61_sort_test7.c"
f62="62_percolation.c"
f63="63_big_int_mul.c"
f64="64_calculator.c"
f65="65_color.c"
f66="66_exgcd.c"
f67="67_reverse_output.c"
f68="68_brainfk.c"
f69="69_expr_eval.c"
f70="70_dijkstra.c"
f71="71_full_conn.c"
f72="72_hanoi.c"
f73="73_int_io.c"
f74="74_kmp.c"
f75="75_max_flow.c"
f76="76_n_queens.c"
f77="77_substr.c"
f78="78_side_effect.c"
f79="79_var_name.c"
f80="80_chaos_token.c"
f81="81_skip_spaces.c"
f82="82_long_func.c"
f83="83_long_array.c"
f84="84_long_array2.c"
f85="85_long_code.c"
f86="86_long_code2.c"
f87="87_many_params.c"
f88="88_many_params2.c"
f89="89_many_globals.c"
f90="90_many_locals.c"
f91="91_many_locals2.c"
f92="92_register_alloc.c"
f93="93_nested_calls.c"
f94="94_nested_loops.c"
f95="95_float.c"
f96="96_matrix_add.c"
f97="97_matrix_sub.c"
f98="98_matrix_mul.c"
f99="99_matrix_tran.c"
if [ $1 = "test" ]
then
source_file=$ftest
elif [ $1 = "00" ]
then
source_file=$f00
elif [ $1 = "01" ]
then
source_file=$f01
elif [ $1 = "02" ]
then
source_file=$f02
elif [ $1 = "03" ]
then
source_file=$f03
elif [ $1 = "04" ]
then
source_file=$f04
elif [ $1 = "05" ]
then
source_file=$f05
elif [ $1 = "06" ]
then
source_file=$f06
elif [ $1 = "07" ]
then
source_file=$f07
elif [ $1 = "08" ]
then
source_file=$f08
elif [ $1 = "09" ]
then
source_file=$f09
elif [ $1 = "10" ]
then
source_file=$f10
elif [ $1 = "11" ]
then
source_file=$f11
elif [ $1 = "12" ]
then
source_file=$f12
elif [ $1 = "13" ]
then
source_file=$f13
elif [ $1 = "14" ]
then
source_file=$f14
elif [ $1 = "15" ]
then
source_file=$f15
elif [ $1 = "16" ]
then
source_file=$f16
elif [ $1 = "17" ]
then
source_file=$f17
elif [ $1 = "18" ]
then
source_file=$f18
elif [ $1 = "19" ]
then
source_file=$f19
elif [ $1 = "20" ]
then
source_file=$f20
elif [ $1 = "21" ]
then
source_file=$f21
elif [ $1 = "22" ]
then
source_file=$f22
elif [ $1 = "23" ]
then
source_file=$f23
elif [ $1 = "24" ]
then
source_file=$f24
elif [ $1 = "25" ]
then
source_file=$f25
elif [ $1 = "26" ]
then
source_file=$f26
elif [ $1 = "27" ]
then
source_file=$f27
elif [ $1 = "28" ]
then
source_file=$f28
elif [ $1 = "29" ]
then
source_file=$f29
elif [ $1 = "30" ]
then
source_file=$f30
elif [ $1 = "31" ]
then
source_file=$f31
elif [ $1 = "32" ]
then
source_file=$f32
elif [ $1 = "33" ]
then
source_file=$f33
elif [ $1 = "34" ]
then
source_file=$f34
elif [ $1 = "35" ]
then
source_file=$f35
elif [ $1 = "36" ]
then
source_file=$f36
elif [ $1 = "37" ]
then
source_file=$f37
elif [ $1 = "38" ]
then
source_file=$f38
elif [ $1 = "39" ]
then
source_file=$f39
elif [ $1 = "40" ]
then
source_file=$f40
elif [ $1 = "41" ]
then
source_file=$f41
elif [ $1 = "42" ]
then
source_file=$f42
elif [ $1 = "43" ]
then
source_file=$f43
elif [ $1 = "44" ]
then
source_file=$f44
elif [ $1 = "45" ]
then
source_file=$f45
elif [ $1 = "46" ]
then
source_file=$f46
elif [ $1 = "47" ]
then
source_file=$f47
elif [ $1 = "48" ]
then
source_file=$f48
elif [ $1 = "49" ]
then
source_file=$f49
elif [ $1 = "50" ]
then
source_file=$f50
elif [ $1 = "51" ]
then
source_file=$f51
elif [ $1 = "52" ]
then
source_file=$f52
elif [ $1 = "53" ]
then
source_file=$f53
elif [ $1 = "54" ]
then
source_file=$f54
elif [ $1 = "55" ]
then
source_file=$f55
elif [ $1 = "56" ]
then
source_file=$f56
elif [ $1 = "57" ]
then
source_file=$f57
elif [ $1 = "58" ]
then
source_file=$f58
elif [ $1 = "59" ]
then
source_file=$f59
elif [ $1 = "60" ]
then
source_file=$f60
elif [ $1 = "61" ]
then
source_file=$f61
elif [ $1 = "62" ]
then
source_file=$f62
elif [ $1 = "63" ]
then
source_file=$f63
elif [ $1 = "64" ]
then
source_file=$f64
elif [ $1 = "65" ]
then
source_file=$f65
elif [ $1 = "66" ]
then
source_file=$f66
elif [ $1 = "67" ]
then
source_file=$f67
elif [ $1 = "68" ]
then
source_file=$f68
elif [ $1 = "69" ]
then
source_file=$f69
elif [ $1 = "70" ]
then
source_file=$f70
elif [ $1 = "71" ]
then
source_file=$f71
elif [ $1 = "72" ]
then
source_file=$f72
elif [ $1 = "73" ]
then
source_file=$f73
elif [ $1 = "74" ]
then
source_file=$f74
elif [ $1 = "75" ]
then
source_file=$f75
elif [ $1 = "76" ]
then
source_file=$f76
elif [ $1 = "77" ]
then
source_file=$f77
elif [ $1 = "78" ]
then
source_file=$f78
elif [ $1 = "79" ]
then
source_file=$f79
elif [ $1 = "80" ]
then
source_file=$f80
elif [ $1 = "81" ]
then
source_file=$f81
elif [ $1 = "82" ]
then
source_file=$f82
elif [ $1 = "83" ]
then
source_file=$f83
elif [ $1 = "84" ]
then
source_file=$f84
elif [ $1 = "85" ]
then
source_file=$f85
elif [ $1 = "86" ]
then
source_file=$f86
elif [ $1 = "87" ]
then
source_file=$f87
elif [ $1 = "88" ]
then
source_file=$f88
elif [ $1 = "89" ]
then
source_file=$f89
elif [ $1 = "90" ]
then
source_file=$f90
elif [ $1 = "91" ]
then
source_file=$f91
elif [ $1 = "92" ]
then
source_file=$f92
elif [ $1 = "93" ]
then
source_file=$f93
elif [ $1 = "94" ]
then
source_file=$f94
elif [ $1 = "95" ]
then
source_file=$f95
elif [ $1 = "96" ]
then
source_file=$f96
elif [ $1 = "97" ]
then
source_file=$f97
elif [ $1 = "98" ]
then
source_file=$f98
elif [ $1 = "99" ]
then
source_file=$f99
fi
echo $source_file
./build/bin/sysyc ./test/functionalc/$source_file 1 > ./test/ir.txt
# generate assemble code
arm-none-linux-gnueabihf-gcc -marm -O0 -Wno-implicit -S ./test/functionalc/$source_file -o ./test/ref.S
./build/bin/sysyc ./test/functionalc/$source_file > ./test/mine.S
# generate executable file
arm-none-linux-gnueabihf-gcc -marm ./test/ref.S -o ./test/ref.out -L. -lsysy -static
arm-none-linux-gnueabihf-gcc -marm ./test/mine.S -o ./test/mine.out -L. -lsysy -static