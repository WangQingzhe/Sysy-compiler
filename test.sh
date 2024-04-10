#!/bin/sh
ftest="test.c"
f00="functionalc/00_main.c"
f01="functionalc/01_var_defn2.c"
f02="functionalc/02_var_defn3.c"
f03="functionalc/03_arr_defn2.c"
f04="functionalc/04_arr_defn3.c"
f05="functionalc/05_arr_defn4.c"
f06="functionalc/06_const_var_defn2.c"
f07="functionalc/07_const_var_defn3.c"
f08="functionalc/08_const_array_defn.c"
f09="functionalc/09_func_defn.c"
f10="functionalc/10_var_defn_func.c"
f11="functionalc/11_add2.c"
f12="functionalc/12_addc.c"
f13="functionalc/13_sub2.c"
f14="functionalc/14_subc.c"
f15="functionalc/15_mul.c"
f16="functionalc/16_mulc.c"
f17="functionalc/17_div.c"
f18="functionalc/18_divc.c"
f19="functionalc/19_mod.c"
f20="functionalc/20_rem.c"
f21="functionalc/21_if_test2.c"
f22="functionalc/22_if_test3.c"
f23="functionalc/23_if_test4.c"
f24="functionalc/24_if_test5.c"
f25="functionalc/25_while_if.c"
f26="functionalc/26_while_test1.c"
f27="functionalc/27_while_test2.c"
f28="functionalc/28_while_test3.c"
f29="functionalc/29_break.c"
f30="functionalc/30_continue.c"
f31="functionalc/31_while_if_test1.c"
f32="functionalc/32_while_if_test2.c"
f33="functionalc/33_while_if_test3.c"
f34="functionalc/34_arr_expr_len.c"
f35="functionalc/35_op_priority1.c"
f36="functionalc/36_op_priority2.c"
f37="functionalc/37_op_priority3.c"
f38="functionalc/38_op_priority4.c"
f39="functionalc/39_op_priority5.c"
f40="functionalc/40_unary_op.c"
f41="functionalc/41_unary_op2.c"
f42="functionalc/42_empty_stmt.c"
f43="functionalc/43_logi_assign.c"
f44="functionalc/44_stmt_expr.c"
f45="functionalc/45_comment1.c"
f46="functionalc/46_hex_defn.c"
f47="functionalc/47_hex_oct_add.c"
f48="functionalc/48_assign_complex_expr.c"
f49="functionalc/49_if_complex_expr.c"
f50="functionalc/50_short_circuit.c"
f51="functionalc/51_short_circuit3.c"
f52="functionalc/52_scope.c"
f53="functionalc/53_scope2.c"
f54="functionalc/54_hidden_var.c"
f55="functionalc/55_sort_test1.c"
f56="functionalc/56_sort_test2.c"
f57="functionalc/57_sort_test3.c"
f58="functionalc/58_sort_test4.c"
f59="functionalc/59_sort_test5.c"
f60="functionalc/60_sort_test6.c"
f61="functionalc/61_sort_test7.c"
f62="functionalc/62_percolation.c"
f63="functionalc/63_big_int_mul.c"
f64="functionalc/64_calculator.c"
f65="functionalc/65_color.c"
f66="functionalc/66_exgcd.c"
f67="functionalc/67_reverse_output.c"
f68="functionalc/68_brainfk.c"
f69="functionalc/69_expr_eval.c"
f70="functionalc/70_dijkstra.c"
f71="functionalc/71_full_conn.c"
f72="functionalc/72_hanoi.c"
f73="functionalc/73_int_io.c"
f74="functionalc/74_kmp.c"
f75="functionalc/75_max_flow.c"
f76="functionalc/76_n_queens.c"
f77="functionalc/77_substr.c"
f78="functionalc/78_side_effect.c"
f79="functionalc/79_var_name.c"
f80="functionalc/80_chaos_token.c"
f81="functionalc/81_skip_spaces.c"
f82="functionalc/82_long_func.c"
f83="functionalc/83_long_array.c"
f84="functionalc/84_long_array2.c"
f85="functionalc/85_long_code.c"
f86="functionalc/86_long_code2.c"
f87="functionalc/87_many_params.c"
f88="functionalc/88_many_params2.c"
f89="functionalc/89_many_globals.c"
f90="functionalc/90_many_locals.c"
f91="functionalc/91_many_locals2.c"
f92="functionalc/92_register_alloc.c"
f93="functionalc/93_nested_calls.c"
f94="functionalc/94_nested_loops.c"
f95="functionalc/95_float.c"
f96="functionalc/96_matrix_add.c"
f97="functionalc/97_matrix_sub.c"
f98="functionalc/98_matrix_mul.c"
f99="functionalc/99_matrix_tran.c"
f100="hidden_functionalc/00_comment2.c"
f101="hidden_functionalc/01_multiple_returns.c"
f102="hidden_functionalc/02_ret_in_block.c"
f103="hidden_functionalc/03_branch.c"
f104="hidden_functionalc/04_break_continue.c"
f105="hidden_functionalc/05_param_name.c"
f106="hidden_functionalc/06_func_name.c"
f107="hidden_functionalc/07_arr_init_nd.c"
f108="hidden_functionalc/08_global_arr_init.c"
f109="hidden_functionalc/09_BFS.c"
f110="hidden_functionalc/10_DFS.c"
f111="hidden_functionalc/11_BST.c"
f112="hidden_functionalc/12_DSU.c"
f113="hidden_functionalc/13_LCA.c"
f114="hidden_functionalc/14_dp.c"
f115="hidden_functionalc/15_graph_coloring.c"
f116="hidden_functionalc/16_k_smallest.c"
f117="hidden_functionalc/17_maximal_clique.c"
f118="hidden_functionalc/18_prim.c"
f119="hidden_functionalc/19_search.c"
f120="hidden_functionalc/20_sort.c"
f121="hidden_functionalc/21_union_find.c"
f122="hidden_functionalc/22_matrix_multiply.c"
f123="hidden_functionalc/23_json.c"
f124="hidden_functionalc/24_array_only.c"
f125="hidden_functionalc/25_scope3.c"
f126="hidden_functionalc/26_scope4.c"
f127="hidden_functionalc/27_scope5.c"
f128="hidden_functionalc/28_side_effect2.c"
f129="hidden_functionalc/29_long_line.c"
f130="hidden_functionalc/30_many_dimensions.c"
f131="hidden_functionalc/31_many_indirections.c"
f132="hidden_functionalc/32_many_params3.c"
f133="hidden_functionalc/33_multi_branch.c"
f134="hidden_functionalc/34_multi_loop.c"
f135="hidden_functionalc/35_math.c"
f136="hidden_functionalc/36_rotate.c"
f137="hidden_functionalc/37_dct.c"
f138="hidden_functionalc/38_light2d.c"
f139="hidden_functionalc/39_fp_params.c"
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
elif [ $1 = "100" ]
then
source_file=$f100
elif [ $1 = "101" ]
then
source_file=$f101
elif [ $1 = "102" ]
then
source_file=$f102
elif [ $1 = "103" ]
then
source_file=$f103
elif [ $1 = "104" ]
then
source_file=$f104
elif [ $1 = "105" ]
then
source_file=$f105
elif [ $1 = "106" ]
then
source_file=$f106
elif [ $1 = "107" ]
then
source_file=$f107
elif [ $1 = "108" ]
then
source_file=$f108
elif [ $1 = "109" ]
then
source_file=$f109
elif [ $1 = "110" ]
then
source_file=$f110
elif [ $1 = "111" ]
then
source_file=$f111
elif [ $1 = "112" ]
then
source_file=$f112
elif [ $1 = "113" ]
then
source_file=$f113
elif [ $1 = "114" ]
then
source_file=$f114
elif [ $1 = "115" ]
then
source_file=$f115
elif [ $1 = "116" ]
then
source_file=$f116
elif [ $1 = "117" ]
then
source_file=$f117
elif [ $1 = "118" ]
then
source_file=$f118
elif [ $1 = "119" ]
then
source_file=$f119
elif [ $1 = "120" ]
then
source_file=$f120
elif [ $1 = "121" ]
then
source_file=$f121
elif [ $1 = "122" ]
then
source_file=$f122
elif [ $1 = "123" ]
then
source_file=$f123
elif [ $1 = "124" ]
then
source_file=$f124
elif [ $1 = "125" ]
then
source_file=$f125
elif [ $1 = "126" ]
then
source_file=$f126
elif [ $1 = "127" ]
then
source_file=$f127
elif [ $1 = "128" ]
then
source_file=$f128
elif [ $1 = "129" ]
then
source_file=$f129
elif [ $1 = "130" ]
then
source_file=$f130
elif [ $1 = "131" ]
then
source_file=$f131
elif [ $1 = "132" ]
then
source_file=$f132
elif [ $1 = "133" ]
then
source_file=$f133
elif [ $1 = "134" ]
then
source_file=$f134
elif [ $1 = "135" ]
then
source_file=$f135
elif [ $1 = "136" ]
then
source_file=$f136
elif [ $1 = "137" ]
then
source_file=$f137
elif [ $1 = "138" ]
then
source_file=$f138
elif [ $1 = "139" ]
then
source_file=$f139
fi
echo $source_file
./build/bin/sysyc ./test/$source_file 1 > ./test/ir.txt
# generate assemble code
arm-none-linux-gnueabihf-gcc -marm -O0 -Wno-implicit -S ./test/$source_file -o ./test/ref.S
./build/bin/sysyc ./test/$source_file > ./test/mine.S
# generate executable file
arm-none-linux-gnueabihf-gcc -marm ./test/ref.S -o ./test/ref.out -L. -lsysy -static
arm-none-linux-gnueabihf-gcc -marm ./test/mine.S -o ./test/mine.out -L. -lsysy -static