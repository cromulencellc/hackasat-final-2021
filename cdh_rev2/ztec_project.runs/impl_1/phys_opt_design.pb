
T
Command: %s
53*	vivadotcl2#
phys_opt_design2default:defaultZ4-113h px? 
?
@Attempting to get a license for feature '%s' and/or device '%s'
308*common2"
Implementation2default:default2
xc7a75t2default:defaultZ17-347h px? 
?
0Got license for feature '%s' and/or device '%s'
310*common2"
Implementation2default:default2
xc7a75t2default:defaultZ17-349h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2.
Netlist sorting complete. 2default:default2
00:00:002default:default2
00:00:00.012default:default2
3170.6682default:default2
0.0002default:default2
70212default:default2
122622default:defaultZ17-722h px? 
a

Starting %s Task
103*constraints2&
Physical Synthesis2default:defaultZ18-103h px? 
?

Phase %s%s
101*constraints2
1 2default:default25
!Physical Synthesis Initialization2default:defaultZ18-101h px? 
?
EMultithreading enabled for phys_opt_design using a maximum of %s CPUs380*physynth2
82default:defaultZ32-721h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.8052default:default2
-10.5792default:defaultZ32-619h px? 
T
?Phase 1 Physical Synthesis Initialization | Checksum: a3fc7645
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:14 ; elapsed = 00:00:04 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 7025 ; free virtual = 122672default:defaulth px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.8052default:default2
-10.5792default:defaultZ32-619h px? 
z

Phase %s%s
101*constraints2
2 2default:default2-
DSP Register Optimization2default:defaultZ18-101h px? 
j
FNo candidate cells for DSP register optimization found in the design.
274*physynthZ32-456h px? 
?
aEnd %s Pass. Optimized %s %s. Created %s new %s, deleted %s existing %s and moved %s existing %s
415*physynth2
22default:default2
02default:default2
net or cell2default:default2
02default:default2
cell2default:default2
02default:default2
cell2default:default2
02default:default2
cell2default:defaultZ32-775h px? 
L
7Phase 2 DSP Register Optimization | Checksum: a3fc7645
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:14 ; elapsed = 00:00:04 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 7024 ; free virtual = 122652default:defaulth px? 
{

Phase %s%s
101*constraints2
3 2default:default2.
Critical Path Optimization2default:defaultZ18-101h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.8052default:default2
-10.5792default:defaultZ32-619h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default2?
Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]	Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]2default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default8Z32-702h px? 
?
(Processed net %s.  Re-placed instance %s337*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default2^
#leon3_system_1/ua2.uart2/r_reg[txd]	#leon3_system_1/ua2.uart2/r_reg[txd]2default:default8Z32-663h px? 
?
;Processed net %s. Optimization improves timing on the net.
394*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-735h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.7582default:default2
-10.5322default:defaultZ32-619h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default2^
#leon3_system_1/ua2.uart2/r_reg[txd]	#leon3_system_1/ua2.uart2/r_reg[txd]2default:default8Z32-662h px? 
?
BNet %s was not replicated - recommend -force_replication_on_nets.
315*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-572h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-702h px? 
?
(Processed net %s.  Re-placed instance %s337*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default2r
-debounce_umbilical_select/data_sync[0]_i_1__4	-debounce_umbilical_select/data_sync[0]_i_1__42default:default8Z32-663h px? 
?
;Processed net %s. Optimization improves timing on the net.
394*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default8Z32-735h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.2792default:default2
-10.0532default:defaultZ32-619h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default2r
-debounce_umbilical_select/data_sync[0]_i_1__4	-debounce_umbilical_select/data_sync[0]_i_1__42default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default8Z32-702h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
Vleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_iVleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_i2default:default8Z32-702h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default2?
Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]	Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]2default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default8Z32-702h px? 
?
(Processed net %s.  Re-placed instance %s337*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default2^
#leon3_system_1/ua2.uart2/r_reg[txd]	#leon3_system_1/ua2.uart2/r_reg[txd]2default:default8Z32-663h px? 
?
;Processed net %s. Optimization improves timing on the net.
394*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-735h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.1832default:default2
-9.9572default:defaultZ32-619h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default2^
#leon3_system_1/ua2.uart2/r_reg[txd]	#leon3_system_1/ua2.uart2/r_reg[txd]2default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-702h px? 
?
(Processed net %s.  Re-placed instance %s337*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default2r
-debounce_umbilical_select/data_sync[0]_i_1__4	-debounce_umbilical_select/data_sync[0]_i_1__42default:default8Z32-663h px? 
?
;Processed net %s. Optimization improves timing on the net.
394*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default8Z32-735h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.1772default:default2
-9.9512default:defaultZ32-619h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default2r
-debounce_umbilical_select/data_sync[0]_i_1__4	-debounce_umbilical_select/data_sync[0]_i_1__42default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default8Z32-702h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
Vleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_iVleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_i2default:default8Z32-702h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.1772default:default2
-9.9512default:defaultZ32-619h px? 
M
8Phase 3 Critical Path Optimization | Checksum: a3fc7645
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:16 ; elapsed = 00:00:05 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 7021 ; free virtual = 122622default:defaulth px? 
{

Phase %s%s
101*constraints2
4 2default:default2.
Critical Path Optimization2default:defaultZ18-101h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.1772default:default2
-9.9512default:defaultZ32-619h px? 
?
(Processed net %s.  Re-placed instance %s337*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default2?
Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]	Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]2default:default8Z32-663h px? 
?
;Processed net %s. Optimization improves timing on the net.
394*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default8Z32-735h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.0592default:default2
-9.8332default:defaultZ32-619h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default2?
Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]	Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]2default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default8Z32-702h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default2^
#leon3_system_1/ua2.uart2/r_reg[txd]	#leon3_system_1/ua2.uart2/r_reg[txd]2default:default8Z32-662h px? 
?
BNet %s was not replicated - recommend -force_replication_on_nets.
315*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-572h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-702h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default2r
-debounce_umbilical_select/data_sync[0]_i_1__4	-debounce_umbilical_select/data_sync[0]_i_1__42default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default8Z32-702h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
Vleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_iVleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_i2default:default8Z32-702h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default2?
Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]	Comsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync_reg[0]2default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]?omsp_system_radio_inst/uart_app/sync_cell_uart_rxd/data_sync[0]2default:default8Z32-702h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default2^
#leon3_system_1/ua2.uart2/r_reg[txd]	#leon3_system_1/ua2.uart2/r_reg[txd]2default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2h
(leon3_system_1/ua2.uart2/leon3_radio_txd(leon3_system_1/ua2.uart2/leon3_radio_txd2default:default8Z32-702h px? 
?
/Processed net %s.  Did not re-place instance %s336*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default2r
-debounce_umbilical_select/data_sync[0]_i_1__4	-debounce_umbilical_select/data_sync[0]_i_1__42default:default8Z32-662h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2T
debounce_umbilical_select/D[0]debounce_umbilical_select/D[0]2default:default8Z32-702h px? 
?
BPorcessed net %s. Optimizations did not improve timing on the net.366*physynth2?
Vleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_iVleon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst/u_mig_mig/u_ddr3_infrastructure/clk_pll_i2default:default8Z32-702h px? 
?
(%s %s Timing Summary | WNS=%s | TNS=%s |333*physynth2
	Estimated2default:default2
 2default:default2
-5.0592default:default2
-9.8332default:defaultZ32-619h px? 
M
8Phase 4 Critical Path Optimization | Checksum: a3fc7645
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:17 ; elapsed = 00:00:05 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 7022 ; free virtual = 122632default:defaulth px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2.
Netlist sorting complete. 2default:default2
00:00:00.012default:default2
00:00:002default:default2
3170.6682default:default2
0.0002default:default2
70212default:default2
122632default:defaultZ17-722h px? 
?
>Post Physical Optimization Timing Summary | WNS=%s | TNS=%s |
318*physynth2
-5.0592default:default2
-9.8332default:defaultZ32-603h px? 
B
-
Summary of Physical Synthesis Optimizations
*commonh px? 
B
-============================================
*commonh px? 


*commonh px? 


*commonh px? 
?
?-------------------------------------------------------------------------------------------------------------------------------------------------------------
*commonh px? 
?
?|  Optimization   |  WNS Gain (ns)  |  TNS Gain (ns)  |  Added Cells  |  Removed Cells  |  Optimized Cells/Nets  |  Dont Touch  |  Iterations  |  Elapsed   |
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*commonh px? 
?
?|  DSP Register   |          0.000  |          0.000  |            0  |              0  |                     0  |           0  |           1  |  00:00:00  |
|  Critical Path  |          0.746  |          0.746  |            0  |              0  |                     5  |           0  |           2  |  00:00:01  |
|  Total          |          0.746  |          0.746  |            0  |              0  |                     5  |           0  |           3  |  00:00:01  |
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*commonh px? 


*commonh px? 


*commonh px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2.
Netlist sorting complete. 2default:default2
00:00:002default:default2
00:00:002default:default2
3170.6682default:default2
0.0002default:default2
70212default:default2
122622default:defaultZ17-722h px? 
I
4Ending Physical Synthesis Task | Checksum: a3fc7645
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:17 ; elapsed = 00:00:05 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 7020 ; free virtual = 122622default:defaulth px? 
Z
Releasing license: %s
83*common2"
Implementation2default:defaultZ17-83h px? 
?
G%s Infos, %s Warnings, %s Critical Warnings and %s Errors encountered.
28*	vivadotcl2
1452default:default2
322default:default2
02default:default2
02default:defaultZ4-41h px? 
a
%s completed successfully
29*	vivadotcl2#
phys_opt_design2default:defaultZ4-42h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2%
phys_opt_design: 2default:default2
00:00:332default:default2
00:00:102default:default2
3170.6682default:default2
0.0002default:default2
70362default:default2
122782default:defaultZ17-722h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2.
Netlist sorting complete. 2default:default2
00:00:00.012default:default2
00:00:00.012default:default2
3170.6682default:default2
0.0002default:default2
70362default:default2
122782default:defaultZ17-722h px? 
H
&Writing timing data to binary archive.266*timingZ38-480h px? 
D
Writing placer database...
1603*designutilsZ20-1893h px? 
=
Writing XDEF routing.
211*designutilsZ20-211h px? 
J
#Writing XDEF routing logical nets.
209*designutilsZ20-209h px? 
J
#Writing XDEF routing special nets.
210*designutilsZ20-210h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2)
Write XDEF Complete: 2default:default2
00:00:062default:default2
00:00:022default:default2
3170.6682default:default2
0.0002default:default2
69642default:default2
122482default:defaultZ17-722h px? 
?
 The %s '%s' has been generated.
621*common2

checkpoint2default:default2h
T/home/jlatimer/git/ztec_project/ztec_project.runs/impl_1/openMSP430_fpga_physopt.dcp2default:defaultZ17-1381h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2&
write_checkpoint: 2default:default2
00:00:102default:default2
00:00:062default:default2
3170.6682default:default2
0.0002default:default2
70122default:default2
122642default:defaultZ17-722h px? 


End Record