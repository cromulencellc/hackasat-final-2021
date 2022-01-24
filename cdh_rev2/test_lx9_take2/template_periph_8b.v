//----------------------------------------------------------------------------
// Copyright (C) 2009 , Olivier Girard
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the authors nor the names of its contributors
//       may be used to endorse or promote products derived from this software
//       without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE
//
//----------------------------------------------------------------------------
//
// *File Name: template_periph_8b.v
// 
// *Module Description:
//                       8 bit peripheral template.
//
// *Author(s):
//              - Olivier Girard,    olgirard@gmail.com
//
//----------------------------------------------------------------------------
// $Rev$
// $LastChangedBy$
// $LastChangedDate$
//----------------------------------------------------------------------------

module  template_periph_8b (

// OUTPUTs
    per_dout,                       // Peripheral data output

// INPUTs
    mclk,                           // Main system clock
    per_addr,                       // Peripheral address
    per_din,                        // Peripheral data input
    per_en,                         // Peripheral enable (high active)
    per_we,                         // Peripheral write enable (high active)
    puc_rst                         // Main system reset
);

// OUTPUTs
//=========
output      [15:0] per_dout;        // Peripheral data output

// INPUTs
//=========
input              mclk;            // Main system clock
input       [13:0] per_addr;        // Peripheral address
input       [15:0] per_din;         // Peripheral data input
input              per_en;          // Peripheral enable (high active)
input        [1:0] per_we;          // Peripheral write enable (high active)
input              puc_rst;         // Main system reset


//=============================================================================
// 1)  PARAMETER DECLARATION
//=============================================================================

// Register base address (must be aligned to decoder bit width)
parameter       [14:0] BASE_ADDR   = 15'h0090;

// Decoder bit width (defines how many bits are considered for address decoding)
parameter              DEC_WD      =  2;

// Register addresses offset
parameter [DEC_WD-1:0] CNTRL1      =  'h0,
                       CNTRL2      =  'h1,
                       CNTRL3      =  'h2,
                       CNTRL4      =  'h3;

   
// Register one-hot decoder utilities
parameter              DEC_SZ      =  (1 << DEC_WD);
parameter [DEC_SZ-1:0] BASE_REG    =  {{DEC_SZ-1{1'b0}}, 1'b1};

// Register one-hot decoder
parameter [DEC_SZ-1:0] CNTRL1_D  = (BASE_REG << CNTRL1),
                       CNTRL2_D  = (BASE_REG << CNTRL2), 
                       CNTRL3_D  = (BASE_REG << CNTRL3), 
                       CNTRL4_D  = (BASE_REG << CNTRL4); 


//============================================================================
// 2)  REGISTER DECODER
//============================================================================

// Local register selection
wire              reg_sel      =  per_en & (per_addr[13:DEC_WD-1]==BASE_ADDR[14:DEC_WD]);

// Register local address
wire [DEC_WD-1:0] reg_addr     =  {1'b0, per_addr[DEC_WD-2:0]};

// Register address decode
wire [DEC_SZ-1:0] reg_dec      = (CNTRL1_D  &  {DEC_SZ{(reg_addr==(CNTRL1 >>1))}}) |
                                 (CNTRL2_D  &  {DEC_SZ{(reg_addr==(CNTRL2 >>1))}}) |
                                 (CNTRL3_D  &  {DEC_SZ{(reg_addr==(CNTRL3 >>1))}}) |
                                 (CNTRL4_D  &  {DEC_SZ{(reg_addr==(CNTRL4 >>1))}});

// Read/Write probes
wire              reg_lo_write =  per_we[0] & reg_sel;
wire              reg_hi_write =  per_we[1] & reg_sel;
wire              reg_read     = ~|per_we   & reg_sel;

// Read/Write vectors
wire [DEC_SZ-1:0] reg_hi_wr    = reg_dec & {DEC_SZ{reg_hi_write}};
wire [DEC_SZ-1:0] reg_lo_wr    = reg_dec & {DEC_SZ{reg_lo_write}};
wire [DEC_SZ-1:0] reg_rd       = reg_dec & {DEC_SZ{reg_read}};


//============================================================================
// 3) REGISTERS
//============================================================================

// CNTRL1 Register
//-----------------
reg  [7:0] cntrl1;

wire       cntrl1_wr  = CNTRL1[0] ? reg_hi_wr[CNTRL1] : reg_lo_wr[CNTRL1];
wire [7:0] cntrl1_nxt = CNTRL1[0] ? per_din[15:8]     : per_din[7:0];

always @ (posedge mclk or posedge puc_rst)
  if (puc_rst)        cntrl1 <=  8'h00;
  else if (cntrl1_wr) cntrl1 <=  cntrl1_nxt;

   
// CNTRL2 Register
//-----------------
reg  [7:0] cntrl2;

wire       cntrl2_wr  = CNTRL2[0] ? reg_hi_wr[CNTRL2] : reg_lo_wr[CNTRL2];
wire [7:0] cntrl2_nxt = CNTRL2[0] ? per_din[15:8]     : per_din[7:0];

always @ (posedge mclk or posedge puc_rst)
  if (puc_rst)        cntrl2 <=  8'h00;
  else if (cntrl2_wr) cntrl2 <=  cntrl2_nxt;

   
// CNTRL3 Register
//-----------------
reg  [7:0] cntrl3;

wire       cntrl3_wr  = CNTRL3[0] ? reg_hi_wr[CNTRL3] : reg_lo_wr[CNTRL3];
wire [7:0] cntrl3_nxt = CNTRL3[0] ? per_din[15:8]     : per_din[7:0];

always @ (posedge mclk or posedge puc_rst)
  if (puc_rst)        cntrl3 <=  8'h00;
  else if (cntrl3_wr) cntrl3 <=  cntrl3_nxt;

   
// CNTRL4 Register
//-----------------
reg  [7:0] cntrl4;

wire       cntrl4_wr  = CNTRL4[0] ? reg_hi_wr[CNTRL4] : reg_lo_wr[CNTRL4];
wire [7:0] cntrl4_nxt = CNTRL4[0] ? per_din[15:8]     : per_din[7:0];

always @ (posedge mclk or posedge puc_rst)
  if (puc_rst)        cntrl4 <=  8'h00;
  else if (cntrl4_wr) cntrl4 <=  cntrl4_nxt;



//============================================================================
// 4) DATA OUTPUT GENERATION
//============================================================================

// Data output mux
wire [15:0] cntrl1_rd   = {8'h00, (cntrl1  & {8{reg_rd[CNTRL1]}})}  << (8 & {4{CNTRL1[0]}});
wire [15:0] cntrl2_rd   = {8'h00, (cntrl2  & {8{reg_rd[CNTRL2]}})}  << (8 & {4{CNTRL2[0]}});
wire [15:0] cntrl3_rd   = {8'h00, (cntrl3  & {8{reg_rd[CNTRL3]}})}  << (8 & {4{CNTRL3[0]}});
wire [15:0] cntrl4_rd   = {8'h00, (cntrl4  & {8{reg_rd[CNTRL4]}})}  << (8 & {4{CNTRL4[0]}});

wire [15:0] per_dout  =  cntrl1_rd  |
                         cntrl2_rd  |
                         cntrl3_rd  |
                         cntrl4_rd;

   
endmodule // template_periph_8b
