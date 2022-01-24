// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2019.2 (lin64) Build 2708876 Wed Nov  6 21:39:14 MST 2019
// Date        : Sat Apr 25 09:17:01 2020
// Host        : ubuntu running 64-bit Ubuntu 18.04.4 LTS
// Command     : write_verilog -force -mode synth_stub
//               /home/user/hackthesat/fpga/new_project/ztec_project/ztec_project.srcs/sources_1/ip/ram_16x16k_sp_dmem/ram_16x16k_sp_dmem_stub.v
// Design      : ram_16x16k_sp_dmem
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a75tcsg324-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* x_core_info = "blk_mem_gen_v8_4_4,Vivado 2019.2" *)
module ram_16x16k_sp_dmem(clka, ena, wea, addra, dina, douta)
/* synthesis syn_black_box black_box_pad_pin="clka,ena,wea[1:0],addra[13:0],dina[15:0],douta[15:0]" */;
  input clka;
  input ena;
  input [1:0]wea;
  input [13:0]addra;
  input [15:0]dina;
  output [15:0]douta;
endmodule
