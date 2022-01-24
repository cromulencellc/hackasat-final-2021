//Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
//Date        : Mon May 17 11:30:11 2021
//Host        : dev1 running 64-bit Ubuntu 18.04.5 LTS
//Command     : generate_target adcs_bd_wrapper.bd
//Design      : adcs_bd_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module adcs_bd_wrapper
   (DDR_addr,
    DDR_ba,
    DDR_cas_n,
    DDR_ck_n,
    DDR_ck_p,
    DDR_cke,
    DDR_cs_n,
    DDR_dm,
    DDR_dq,
    DDR_dqs_n,
    DDR_dqs_p,
    DDR_odt,
    DDR_ras_n,
    DDR_reset_n,
    DDR_we_n,
    FIXED_IO_ddr_vrn,
    FIXED_IO_ddr_vrp,
    FIXED_IO_mio,
    FIXED_IO_ps_clk,
    FIXED_IO_ps_porb,
    FIXED_IO_ps_srstb,
    Vaux14_0_v_n,
    Vaux14_0_v_p,
    Vaux1_0_v_n,
    Vaux1_0_v_p,
    Vaux2_0_v_n,
    Vaux2_0_v_p,
    Vaux3_0_v_n,
    Vaux3_0_v_p,
    Vaux4_0_v_n,
    Vaux4_0_v_p,
    Vaux7_0_v_n,
    Vaux7_0_v_p,
    Vaux8_0_v_n,
    Vaux8_0_v_p,
    gpio_rtl_0_tri_o,
    gpio_rtl_1_tri_i,
    gpio_rtl_2_tri_io,
    gpio_rtl_3_tri_io,
    gpio_rtl_4_tri_o,
    uart_rtl_0_rxd,
    uart_rtl_0_txd,
    uart_rtl_1_rxd,
    uart_rtl_1_txd);
  inout [14:0]DDR_addr;
  inout [2:0]DDR_ba;
  inout DDR_cas_n;
  inout DDR_ck_n;
  inout DDR_ck_p;
  inout DDR_cke;
  inout DDR_cs_n;
  inout [3:0]DDR_dm;
  inout [31:0]DDR_dq;
  inout [3:0]DDR_dqs_n;
  inout [3:0]DDR_dqs_p;
  inout DDR_odt;
  inout DDR_ras_n;
  inout DDR_reset_n;
  inout DDR_we_n;
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ddr_vrp;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_clk;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ps_srstb;
  input Vaux14_0_v_n;
  input Vaux14_0_v_p;
  input Vaux1_0_v_n;
  input Vaux1_0_v_p;
  input Vaux2_0_v_n;
  input Vaux2_0_v_p;
  input Vaux3_0_v_n;
  input Vaux3_0_v_p;
  input Vaux4_0_v_n;
  input Vaux4_0_v_p;
  input Vaux7_0_v_n;
  input Vaux7_0_v_p;
  input Vaux8_0_v_n;
  input Vaux8_0_v_p;
  output [4:0]gpio_rtl_0_tri_o;
  input [3:0]gpio_rtl_1_tri_i;
  inout [3:0]gpio_rtl_2_tri_io;
  inout [2:0]gpio_rtl_3_tri_io;
  output [0:0]gpio_rtl_4_tri_o;
  input uart_rtl_0_rxd;
  output uart_rtl_0_txd;
  input uart_rtl_1_rxd;
  output uart_rtl_1_txd;

  wire [14:0]DDR_addr;
  wire [2:0]DDR_ba;
  wire DDR_cas_n;
  wire DDR_ck_n;
  wire DDR_ck_p;
  wire DDR_cke;
  wire DDR_cs_n;
  wire [3:0]DDR_dm;
  wire [31:0]DDR_dq;
  wire [3:0]DDR_dqs_n;
  wire [3:0]DDR_dqs_p;
  wire DDR_odt;
  wire DDR_ras_n;
  wire DDR_reset_n;
  wire DDR_we_n;
  wire FIXED_IO_ddr_vrn;
  wire FIXED_IO_ddr_vrp;
  wire [53:0]FIXED_IO_mio;
  wire FIXED_IO_ps_clk;
  wire FIXED_IO_ps_porb;
  wire FIXED_IO_ps_srstb;
  wire Vaux14_0_v_n;
  wire Vaux14_0_v_p;
  wire Vaux1_0_v_n;
  wire Vaux1_0_v_p;
  wire Vaux2_0_v_n;
  wire Vaux2_0_v_p;
  wire Vaux3_0_v_n;
  wire Vaux3_0_v_p;
  wire Vaux4_0_v_n;
  wire Vaux4_0_v_p;
  wire Vaux7_0_v_n;
  wire Vaux7_0_v_p;
  wire Vaux8_0_v_n;
  wire Vaux8_0_v_p;
  wire [4:0]gpio_rtl_0_tri_o;
  wire [3:0]gpio_rtl_1_tri_i;
  wire [0:0]gpio_rtl_2_tri_i_0;
  wire [1:1]gpio_rtl_2_tri_i_1;
  wire [2:2]gpio_rtl_2_tri_i_2;
  wire [3:3]gpio_rtl_2_tri_i_3;
  wire [0:0]gpio_rtl_2_tri_io_0;
  wire [1:1]gpio_rtl_2_tri_io_1;
  wire [2:2]gpio_rtl_2_tri_io_2;
  wire [3:3]gpio_rtl_2_tri_io_3;
  wire [0:0]gpio_rtl_2_tri_o_0;
  wire [1:1]gpio_rtl_2_tri_o_1;
  wire [2:2]gpio_rtl_2_tri_o_2;
  wire [3:3]gpio_rtl_2_tri_o_3;
  wire [0:0]gpio_rtl_2_tri_t_0;
  wire [1:1]gpio_rtl_2_tri_t_1;
  wire [2:2]gpio_rtl_2_tri_t_2;
  wire [3:3]gpio_rtl_2_tri_t_3;
  wire [0:0]gpio_rtl_3_tri_i_0;
  wire [1:1]gpio_rtl_3_tri_i_1;
  wire [2:2]gpio_rtl_3_tri_i_2;
  wire [0:0]gpio_rtl_3_tri_io_0;
  wire [1:1]gpio_rtl_3_tri_io_1;
  wire [2:2]gpio_rtl_3_tri_io_2;
  wire [0:0]gpio_rtl_3_tri_o_0;
  wire [1:1]gpio_rtl_3_tri_o_1;
  wire [2:2]gpio_rtl_3_tri_o_2;
  wire [0:0]gpio_rtl_3_tri_t_0;
  wire [1:1]gpio_rtl_3_tri_t_1;
  wire [2:2]gpio_rtl_3_tri_t_2;
  wire [0:0]gpio_rtl_4_tri_o;
  wire uart_rtl_0_rxd;
  wire uart_rtl_0_txd;
  wire uart_rtl_1_rxd;
  wire uart_rtl_1_txd;

  adcs_bd adcs_bd_i
       (.DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .Vaux14_0_v_n(Vaux14_0_v_n),
        .Vaux14_0_v_p(Vaux14_0_v_p),
        .Vaux1_0_v_n(Vaux1_0_v_n),
        .Vaux1_0_v_p(Vaux1_0_v_p),
        .Vaux2_0_v_n(Vaux2_0_v_n),
        .Vaux2_0_v_p(Vaux2_0_v_p),
        .Vaux3_0_v_n(Vaux3_0_v_n),
        .Vaux3_0_v_p(Vaux3_0_v_p),
        .Vaux4_0_v_n(Vaux4_0_v_n),
        .Vaux4_0_v_p(Vaux4_0_v_p),
        .Vaux7_0_v_n(Vaux7_0_v_n),
        .Vaux7_0_v_p(Vaux7_0_v_p),
        .Vaux8_0_v_n(Vaux8_0_v_n),
        .Vaux8_0_v_p(Vaux8_0_v_p),
        .gpio_rtl_0_tri_o(gpio_rtl_0_tri_o),
        .gpio_rtl_1_tri_i(gpio_rtl_1_tri_i),
        .gpio_rtl_2_tri_i({gpio_rtl_2_tri_i_3,gpio_rtl_2_tri_i_2,gpio_rtl_2_tri_i_1,gpio_rtl_2_tri_i_0}),
        .gpio_rtl_2_tri_o({gpio_rtl_2_tri_o_3,gpio_rtl_2_tri_o_2,gpio_rtl_2_tri_o_1,gpio_rtl_2_tri_o_0}),
        .gpio_rtl_2_tri_t({gpio_rtl_2_tri_t_3,gpio_rtl_2_tri_t_2,gpio_rtl_2_tri_t_1,gpio_rtl_2_tri_t_0}),
        .gpio_rtl_3_tri_i({gpio_rtl_3_tri_i_2,gpio_rtl_3_tri_i_1,gpio_rtl_3_tri_i_0}),
        .gpio_rtl_3_tri_o({gpio_rtl_3_tri_o_2,gpio_rtl_3_tri_o_1,gpio_rtl_3_tri_o_0}),
        .gpio_rtl_3_tri_t({gpio_rtl_3_tri_t_2,gpio_rtl_3_tri_t_1,gpio_rtl_3_tri_t_0}),
        .gpio_rtl_4_tri_o(gpio_rtl_4_tri_o),
        .uart_rtl_0_rxd(uart_rtl_0_rxd),
        .uart_rtl_0_txd(uart_rtl_0_txd),
        .uart_rtl_1_rxd(uart_rtl_1_rxd),
        .uart_rtl_1_txd(uart_rtl_1_txd));
  IOBUF gpio_rtl_2_tri_iobuf_0
       (.I(gpio_rtl_2_tri_o_0),
        .IO(gpio_rtl_2_tri_io[0]),
        .O(gpio_rtl_2_tri_i_0),
        .T(gpio_rtl_2_tri_t_0));
  IOBUF gpio_rtl_2_tri_iobuf_1
       (.I(gpio_rtl_2_tri_o_1),
        .IO(gpio_rtl_2_tri_io[1]),
        .O(gpio_rtl_2_tri_i_1),
        .T(gpio_rtl_2_tri_t_1));
  IOBUF gpio_rtl_2_tri_iobuf_2
       (.I(gpio_rtl_2_tri_o_2),
        .IO(gpio_rtl_2_tri_io[2]),
        .O(gpio_rtl_2_tri_i_2),
        .T(gpio_rtl_2_tri_t_2));
  IOBUF gpio_rtl_2_tri_iobuf_3
       (.I(gpio_rtl_2_tri_o_3),
        .IO(gpio_rtl_2_tri_io[3]),
        .O(gpio_rtl_2_tri_i_3),
        .T(gpio_rtl_2_tri_t_3));
  IOBUF gpio_rtl_3_tri_iobuf_0
       (.I(gpio_rtl_3_tri_o_0),
        .IO(gpio_rtl_3_tri_io[0]),
        .O(gpio_rtl_3_tri_i_0),
        .T(gpio_rtl_3_tri_t_0));
  IOBUF gpio_rtl_3_tri_iobuf_1
       (.I(gpio_rtl_3_tri_o_1),
        .IO(gpio_rtl_3_tri_io[1]),
        .O(gpio_rtl_3_tri_i_1),
        .T(gpio_rtl_3_tri_t_1));
  IOBUF gpio_rtl_3_tri_iobuf_2
       (.I(gpio_rtl_3_tri_o_2),
        .IO(gpio_rtl_3_tri_io[2]),
        .O(gpio_rtl_3_tri_i_2),
        .T(gpio_rtl_3_tri_t_2));
endmodule
