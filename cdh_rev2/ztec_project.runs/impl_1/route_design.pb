
Q
Command: %s
53*	vivadotcl2 
route_design2default:defaultZ4-113h px? 
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
p
,Running DRC as a precondition to command %s
22*	vivadotcl2 
route_design2default:defaultZ4-22h px? 
P
Running DRC with %s threads
24*drc2
82default:defaultZ23-27h px? 
V
DRC finished with %s
79*	vivadotcl2
0 Errors2default:defaultZ4-198h px? 
e
BPlease refer to the DRC report (report_drc) for more information.
80*	vivadotclZ4-199h px? 
V

Starting %s Task
103*constraints2
Routing2default:defaultZ18-103h px? 
}
BMultithreading enabled for route_design using a maximum of %s CPUs17*	routeflow2
82default:defaultZ35-254h px? 
p

Phase %s%s
101*constraints2
1 2default:default2#
Build RT Design2default:defaultZ18-101h px? 
C
.Phase 1 Build RT Design | Checksum: 168577008
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:26 ; elapsed = 00:00:18 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 6920 ; free virtual = 121692default:defaulth px? 
v

Phase %s%s
101*constraints2
2 2default:default2)
Router Initialization2default:defaultZ18-101h px? 
o

Phase %s%s
101*constraints2
2.1 2default:default2 
Create Timer2default:defaultZ18-101h px? 
B
-Phase 2.1 Create Timer | Checksum: 168577008
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:27 ; elapsed = 00:00:19 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 6920 ; free virtual = 121702default:defaulth px? 
{

Phase %s%s
101*constraints2
2.2 2default:default2,
Fix Topology Constraints2default:defaultZ18-101h px? 
N
9Phase 2.2 Fix Topology Constraints | Checksum: 168577008
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:27 ; elapsed = 00:00:19 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 6882 ; free virtual = 121322default:defaulth px? 
t

Phase %s%s
101*constraints2
2.3 2default:default2%
Pre Route Cleanup2default:defaultZ18-101h px? 
G
2Phase 2.3 Pre Route Cleanup | Checksum: 168577008
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:27 ; elapsed = 00:00:19 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 6894 ; free virtual = 121442default:defaulth px? 
p

Phase %s%s
101*constraints2
2.4 2default:default2!
Update Timing2default:defaultZ18-101h px? 
C
.Phase 2.4 Update Timing | Checksum: 1b47bf6c1
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:00:49 ; elapsed = 00:00:29 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 6882 ; free virtual = 121322default:defaulth px? 
?
Intermediate Timing Summary %s164*route2K
7| WNS=-5.078 | TNS=-9.881 | WHS=-2.479 | THS=-986.122|
2default:defaultZ35-416h px? 
I
4Phase 2 Router Initialization | Checksum: 1dee145b1
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:01:00 ; elapsed = 00:00:32 . Memory (MB): peak = 3170.668 ; gain = 0.000 ; free physical = 6885 ; free virtual = 121352default:defaulth px? 
p

Phase %s%s
101*constraints2
3 2default:default2#
Initial Routing2default:defaultZ18-101h px? 
C
.Phase 3 Initial Routing | Checksum: 1dc0a4128
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:06:02 ; elapsed = 00:01:24 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6834 ; free virtual = 120902default:defaulth px? 
?
>Design has %s pins with tight setup and hold constraints.

%s
244*route2
482default:default2?
?The top 5 pins with tight setup and hold constraints:

+--------------------------+--------------------------+----------------------------------------------------------------------------------------------------------+
|       Launch Clock       |      Capture Clock       |                                                 Pin                                                      |
+--------------------------+--------------------------+----------------------------------------------------------------------------------------------------------+
|                  dco_clk |                clk_pll_i |                                                                  leon3_system_1/ua2.uart2/r_reg[rxf][0]/D|
|                  dco_clk |                clk_pll_i |                                                   leon3_system_1/spi_gen.spimctrl1/r_reg[spii][0][miso]/D|
|                  dco_clk |                clk_pll_i |                                                                  leon3_system_1/ua1.uart1/r_reg[rxf][0]/D|
|                  dco_clk |                clk_pll_i |                                                   leon3_system_1/dcomgen.dcom0/dcom_uart0/r_reg[rxf][0]/D|
|                clk_pll_i |                clk_pll_i |                           leon3_system_1/eth0.e1/m100.u0/edclramnft.r1/xc2v.x0/a6.x0/a11.x[1].r0/DIADI[5]|
+--------------------------+--------------------------+----------------------------------------------------------------------------------------------------------+

File with complete list of pins: tight_setup_hold_pins.txt
2default:defaultZ35-580h px? 
s

Phase %s%s
101*constraints2
4 2default:default2&
Rip-up And Reroute2default:defaultZ18-101h px? 
u

Phase %s%s
101*constraints2
4.1 2default:default2&
Global Iteration 02default:defaultZ18-101h px? 
?
Intermediate Timing Summary %s164*route2J
6| WNS=-5.076 | TNS=-71.820| WHS=N/A    | THS=N/A    |
2default:defaultZ35-416h px? 
H
3Phase 4.1 Global Iteration 0 | Checksum: 1280cfdda
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:08:50 ; elapsed = 00:02:53 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6817 ; free virtual = 120752default:defaulth px? 
u

Phase %s%s
101*constraints2
4.2 2default:default2&
Global Iteration 12default:defaultZ18-101h px? 
?
Intermediate Timing Summary %s164*route2J
6| WNS=-5.076 | TNS=-67.712| WHS=N/A    | THS=N/A    |
2default:defaultZ35-416h px? 
H
3Phase 4.2 Global Iteration 1 | Checksum: 1a5ce96b2
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:33 ; elapsed = 00:03:54 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6836 ; free virtual = 120942default:defaulth px? 
F
1Phase 4 Rip-up And Reroute | Checksum: 1a5ce96b2
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:33 ; elapsed = 00:03:54 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6836 ; free virtual = 120942default:defaulth px? 
|

Phase %s%s
101*constraints2
5 2default:default2/
Delay and Skew Optimization2default:defaultZ18-101h px? 
p

Phase %s%s
101*constraints2
5.1 2default:default2!
Delay CleanUp2default:defaultZ18-101h px? 
r

Phase %s%s
101*constraints2
5.1.1 2default:default2!
Update Timing2default:defaultZ18-101h px? 
E
0Phase 5.1.1 Update Timing | Checksum: 14e39d635
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:36 ; elapsed = 00:03:55 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6838 ; free virtual = 120922default:defaulth px? 
?
Intermediate Timing Summary %s164*route2J
6| WNS=-5.076 | TNS=-67.004| WHS=N/A    | THS=N/A    |
2default:defaultZ35-416h px? 
C
.Phase 5.1 Delay CleanUp | Checksum: 1c5a4a3ff
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:36 ; elapsed = 00:03:55 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6837 ; free virtual = 120922default:defaulth px? 
z

Phase %s%s
101*constraints2
5.2 2default:default2+
Clock Skew Optimization2default:defaultZ18-101h px? 
M
8Phase 5.2 Clock Skew Optimization | Checksum: 1c5a4a3ff
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:36 ; elapsed = 00:03:55 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6837 ; free virtual = 120922default:defaulth px? 
O
:Phase 5 Delay and Skew Optimization | Checksum: 1c5a4a3ff
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:37 ; elapsed = 00:03:56 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6838 ; free virtual = 120932default:defaulth px? 
n

Phase %s%s
101*constraints2
6 2default:default2!
Post Hold Fix2default:defaultZ18-101h px? 
p

Phase %s%s
101*constraints2
6.1 2default:default2!
Hold Fix Iter2default:defaultZ18-101h px? 
r

Phase %s%s
101*constraints2
6.1.1 2default:default2!
Update Timing2default:defaultZ18-101h px? 
E
0Phase 6.1.1 Update Timing | Checksum: 2150bdfa8
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:40 ; elapsed = 00:03:57 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6835 ; free virtual = 120902default:defaulth px? 
?
Intermediate Timing Summary %s164*route2J
6| WNS=-5.076 | TNS=-67.300| WHS=-1.547 | THS=-1.738 |
2default:defaultZ35-416h px? 
B
-Phase 6.1 Hold Fix Iter | Checksum: cd0c9613
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:41 ; elapsed = 00:03:57 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6826 ; free virtual = 120872default:defaulth px? 
?
?The router encountered %s pins that are both setup-critical and hold-critical and tried to fix hold violations at the expense of setup slack. Such pins are:
%s201*route2
532default:default2?
?	leon3_system_1/rst0/async.r[4]_i_1__0/I0
	leon3_system_1/eth0.e1/m100.u0/edclramnft.r1/xc2v.x0/a6.x0/a11.x[1].r0/DIADI[5]
	leon3_system_1/eth0.e1/m100.u0/edclramnft.r1/xc2v.x0/a6.x0/a11.x[1].r0/DIADI[4]
	leon3_system_1/eth0.e1/m100.u0/edclramnft.r1/xc2v.x0/a6.x0/a11.x[1].r0/DIADI[3]
	leon3_system_1/eth0.e1/m100.u0/edclramnft.r1/xc2v.x0/a6.x0/a11.x[1].r0/DIADI[0]
	leon3_system_1/eth0.e1/m100.u0/edclramnft.r1/xc2v.x0/a6.x0/a11.x[0].r0/DIADI[6]
	leon3_system_1/eth0.e1/m100.u0/edclramnft.r1/xc2v.x0/a6.x0/a11.x[0].r0/DIPADIP[0]
	omsp_system_radio_inst/gpio_0/LEON3_LED7_PIN_i_1/I0
	omsp_system_radio_inst/gpio_0/data_rx[15]_i_4__0/I1
	omsp_system_radio_inst/gpio_0/data_rx[15]_i_4__0/I2
	.. and 43 more pins.
2default:defaultZ35-468h px? 
@
+Phase 6 Post Hold Fix | Checksum: da1fa204
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:41 ; elapsed = 00:03:57 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6826 ; free virtual = 120872default:defaulth px? 
o

Phase %s%s
101*constraints2
7 2default:default2"
Route finalize2default:defaultZ18-101h px? 
B
-Phase 7 Route finalize | Checksum: 13fee1ec0
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:41 ; elapsed = 00:03:58 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6825 ; free virtual = 120862default:defaulth px? 
v

Phase %s%s
101*constraints2
8 2default:default2)
Verifying routed nets2default:defaultZ18-101h px? 
I
4Phase 8 Verifying routed nets | Checksum: 13fee1ec0
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:41 ; elapsed = 00:03:58 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6824 ; free virtual = 120852default:defaulth px? 
r

Phase %s%s
101*constraints2
9 2default:default2%
Depositing Routes2default:defaultZ18-101h px? 
E
0Phase 9 Depositing Routes | Checksum: 166644edf
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:43 ; elapsed = 00:04:00 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6827 ; free virtual = 120852default:defaulth px? 
t

Phase %s%s
101*constraints2
10 2default:default2&
Post Router Timing2default:defaultZ18-101h px? 
q

Phase %s%s
101*constraints2
10.1 2default:default2!
Update Timing2default:defaultZ18-101h px? 
C
.Phase 10.1 Update Timing | Checksum: d9ec32bd
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:46 ; elapsed = 00:04:01 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6823 ; free virtual = 120772default:defaulth px? 
?
Estimated Timing Summary %s
57*route2J
6| WNS=-5.072 | TNS=-69.506| WHS=0.065  | THS=0.000  |
2default:defaultZ35-57h px? 
B
!Router estimated timing not met.
128*routeZ35-328h px? 
F
1Phase 10 Post Router Timing | Checksum: d9ec32bd
*commonh px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:46 ; elapsed = 00:04:01 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6822 ; free virtual = 120772default:defaulth px? 
@
Router Completed Successfully
2*	routeflowZ35-16h px? 
?

%s
*constraints2?
?Time (s): cpu = 00:10:46 ; elapsed = 00:04:01 . Memory (MB): peak = 3439.617 ; gain = 268.949 ; free physical = 6901 ; free virtual = 121562default:defaulth px? 
Z
Releasing license: %s
83*common2"
Implementation2default:defaultZ17-83h px? 
?
G%s Infos, %s Warnings, %s Critical Warnings and %s Errors encountered.
28*	vivadotcl2
1612default:default2
342default:default2
02default:default2
02default:defaultZ4-41h px? 
^
%s completed successfully
29*	vivadotcl2 
route_design2default:defaultZ4-42h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2"
route_design: 2default:default2
00:10:512default:default2
00:04:042default:default2
3439.6172default:default2
268.9492default:default2
69012default:default2
121562default:defaultZ17-722h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2.
Netlist sorting complete. 2default:default2
00:00:002default:default2
00:00:002default:default2
3439.6172default:default2
0.0002default:default2
69002default:default2
121552default:defaultZ17-722h px? 
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
00:00:072default:default2
00:00:022default:default2
3439.6172default:default2
0.0002default:default2
68162default:default2
121272default:defaultZ17-722h px? 
?
 The %s '%s' has been generated.
621*common2

checkpoint2default:default2g
S/home/jlatimer/git/ztec_project/ztec_project.runs/impl_1/openMSP430_fpga_routed.dcp2default:defaultZ17-1381h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2&
write_checkpoint: 2default:default2
00:00:102default:default2
00:00:062default:default2
3439.6172default:default2
0.0002default:default2
68762default:default2
121442default:defaultZ17-722h px? 
?
%s4*runtcl2?
?Executing : report_drc -file openMSP430_fpga_drc_routed.rpt -pb openMSP430_fpga_drc_routed.pb -rpx openMSP430_fpga_drc_routed.rpx
2default:defaulth px? 
?
Command: %s
53*	vivadotcl2?
ureport_drc -file openMSP430_fpga_drc_routed.rpt -pb openMSP430_fpga_drc_routed.pb -rpx openMSP430_fpga_drc_routed.rpx2default:defaultZ4-113h px? 
>
IP Catalog is up to date.1232*coregenZ19-1839h px? 
P
Running DRC with %s threads
24*drc2
82default:defaultZ23-27h px? 
?
#The results of DRC are in file %s.
168*coretcl2?
W/home/jlatimer/git/ztec_project/ztec_project.runs/impl_1/openMSP430_fpga_drc_routed.rptW/home/jlatimer/git/ztec_project/ztec_project.runs/impl_1/openMSP430_fpga_drc_routed.rpt2default:default8Z2-168h px? 
\
%s completed successfully
29*	vivadotcl2

report_drc2default:defaultZ4-42h px? 
?
%s4*runtcl2?
?Executing : report_methodology -file openMSP430_fpga_methodology_drc_routed.rpt -pb openMSP430_fpga_methodology_drc_routed.pb -rpx openMSP430_fpga_methodology_drc_routed.rpx
2default:defaulth px? 
?
Command: %s
53*	vivadotcl2?
?report_methodology -file openMSP430_fpga_methodology_drc_routed.rpt -pb openMSP430_fpga_methodology_drc_routed.pb -rpx openMSP430_fpga_methodology_drc_routed.rpx2default:defaultZ4-113h px? 
E
%Done setting XDC timing constraints.
35*timingZ38-35h px? 
Y
$Running Methodology with %s threads
74*drc2
82default:defaultZ23-133h px? 
?
2The results of Report Methodology are in file %s.
450*coretcl2?
c/home/jlatimer/git/ztec_project/ztec_project.runs/impl_1/openMSP430_fpga_methodology_drc_routed.rptc/home/jlatimer/git/ztec_project/ztec_project.runs/impl_1/openMSP430_fpga_methodology_drc_routed.rpt2default:default8Z2-1520h px? 
d
%s completed successfully
29*	vivadotcl2&
report_methodology2default:defaultZ4-42h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2(
report_methodology: 2default:default2
00:00:202default:default2
00:00:062default:default2
3527.6602default:default2
0.0002default:default2
68262default:default2
120952default:defaultZ17-722h px? 
?
%s4*runtcl2?
?Executing : report_power -file openMSP430_fpga_power_routed.rpt -pb openMSP430_fpga_power_summary_routed.pb -rpx openMSP430_fpga_power_routed.rpx
2default:defaulth px? 
?
Command: %s
53*	vivadotcl2?
?report_power -file openMSP430_fpga_power_routed.rpt -pb openMSP430_fpga_power_summary_routed.pb -rpx openMSP430_fpga_power_routed.rpx2default:defaultZ4-113h px? 
E
%Done setting XDC timing constraints.
35*timingZ38-35h px? 
K
,Running Vector-less Activity Propagation...
51*powerZ33-51h px? 
P
3
Finished Running Vector-less Activity Propagation
1*powerZ33-1h px? 
?
?Detected over-assertion of set/reset/preset/clear net with high fanouts, power estimation might not be accurate. Please run Tool - Power Constraint Wizard to set proper switching activities for control signals.282*powerZ33-332h px? 
?
G%s Infos, %s Warnings, %s Critical Warnings and %s Errors encountered.
28*	vivadotcl2
1732default:default2
352default:default2
02default:default2
02default:defaultZ4-41h px? 
^
%s completed successfully
29*	vivadotcl2 
report_power2default:defaultZ4-42h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2"
report_power: 2default:default2
00:00:212default:default2
00:00:102default:default2
3527.6602default:default2
0.0002default:default2
67612default:default2
120572default:defaultZ17-722h px? 
?
%s4*runtcl2
kExecuting : report_route_status -file openMSP430_fpga_route_status.rpt -pb openMSP430_fpga_route_status.pb
2default:defaulth px? 
?
%s4*runtcl2?
?Executing : report_timing_summary -max_paths 10 -file openMSP430_fpga_timing_summary_routed.rpt -pb openMSP430_fpga_timing_summary_routed.pb -rpx openMSP430_fpga_timing_summary_routed.rpx -warn_on_violation 
2default:defaulth px? 
r
UpdateTimingParams:%s.
91*timing29
% Speed grade: -2, Delay Type: min_max2default:defaultZ38-91h px? 
|
CMultithreading enabled for timing update using a maximum of %s CPUs155*timing2
82default:defaultZ38-191h px? 
?
rThe design failed to meet the timing requirements. Please see the %s report for details on the timing violations.
188*timing2"
timing summary2default:defaultZ38-282h px? 
?
%s4*runtcl2l
XExecuting : report_incremental_reuse -file openMSP430_fpga_incremental_reuse_routed.rpt
2default:defaulth px? 
g
BIncremental flow is disabled. No incremental reuse Info to report.423*	vivadotclZ4-1062h px? 
?
%s4*runtcl2l
XExecuting : report_clock_utilization -file openMSP430_fpga_clock_utilization_routed.rpt
2default:defaulth px? 
?
%s4*runtcl2?
?Executing : report_bus_skew -warn_on_violation -file openMSP430_fpga_bus_skew_routed.rpt -pb openMSP430_fpga_bus_skew_routed.pb -rpx openMSP430_fpga_bus_skew_routed.rpx
2default:defaulth px? 
r
UpdateTimingParams:%s.
91*timing29
% Speed grade: -2, Delay Type: min_max2default:defaultZ38-91h px? 
|
CMultithreading enabled for timing update using a maximum of %s CPUs155*timing2
82default:defaultZ38-191h px? 


End Record