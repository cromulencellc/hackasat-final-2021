
{
Command: %s
53*	vivadotcl2J
6link_design -top openMSP430_fpga -part xc7a75tcsg324-22default:defaultZ4-113h px? 
g
#Design is defaulting to srcset: %s
437*	planAhead2
	sources_12default:defaultZ12-437h px? 
j
&Design is defaulting to constrset: %s
434*	planAhead2
	constrs_12default:defaultZ12-434h px? 
V
Loading part %s157*device2#
xc7a75tcsg324-22default:defaultZ21-403h px? 
?
-Reading design checkpoint '%s' for cell '%s'
275*project2|
h/home/jlatimer/git/ztec_project/ztec_project.srcs/sources_1/ip/ram_16x16k_sp_dmem/ram_16x16k_sp_dmem.dcp2default:default2'
ram_dmem_omsp_radio2default:defaultZ1-454h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2.
Netlist sorting complete. 2default:default2
00:00:00.222default:default2
00:00:00.222default:default2
1740.9022default:default2
0.0002default:default2
79662default:default2
131582default:defaultZ17-722h px? 
h
-Analyzing %s Unisim elements for replacement
17*netlist2
12112default:defaultZ29-17h px? 
j
2Unisim Transformation completed in %s CPU seconds
28*netlist2
02default:defaultZ29-28h px? 
x
Netlist was created with %s %s291*project2
Vivado2default:default2
2019.22default:defaultZ1-479h px? 
K
)Preparing netlist for logic optimization
349*projectZ1-570h px? 
?
$Parsing XDC File [%s] for cell '%s'
848*designutils2|
f/home/jlatimer/git/ztec_project/ztec_project.srcs/sources_1/ip/mig/mig/user_design/constraints/mig.xdc2default:default2B
,leon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst	2default:default8Z20-848h px? 
?
-Finished Parsing XDC File [%s] for cell '%s'
847*designutils2|
f/home/jlatimer/git/ztec_project/ztec_project.srcs/sources_1/ip/mig/mig/user_design/constraints/mig.xdc2default:default2B
,leon3_system_1/mig_gen.gen_mig.ddrc/MCB_inst	2default:default8Z20-847h px? 
?
Parsing XDC File [%s]
179*designutils2u
_/home/jlatimer/git/ztec_project/ztec_project.srcs/constrs_1/imports/leon3-ztec_2_13/leon3mp.xdc2default:default8Z20-179h px? 
?
!port, pin or net '%s' not found.
663*	planAhead2
get_nets2default:default2u
_/home/jlatimer/git/ztec_project/ztec_project.srcs/constrs_1/imports/leon3-ztec_2_13/leon3mp.xdc2default:default2
162default:default8@Z12-663h px? 
?
DImplicit search of objects for pattern '%s' matched to '%s' objects.1744*	planAhead2
dcm_clk02default:default2
net2default:default2u
_/home/jlatimer/git/ztec_project/ztec_project.srcs/constrs_1/imports/leon3-ztec_2_13/leon3mp.xdc2default:default2
162default:default8@Z12-2286h px? 
?
Finished Parsing XDC File [%s]
178*designutils2u
_/home/jlatimer/git/ztec_project/ztec_project.srcs/constrs_1/imports/leon3-ztec_2_13/leon3mp.xdc2default:default8Z20-178h px? 
u
)Pushed %s inverter(s) to %s load pin(s).
98*opt2
02default:default2
02default:defaultZ31-138h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2.
Netlist sorting complete. 2default:default2
00:00:002default:default2
00:00:002default:default2
1934.2112default:default2
0.0002default:default2
78302default:default2
130232default:defaultZ17-722h px? 
?
!Unisim Transformation Summary:
%s111*project2?
?  A total of 168 instances were transformed.
  IOBUF => IOBUF (IBUF, OBUFT): 4 instances
  IOBUFDS_DIFF_OUT_INTERMDISABLE => IOBUFDS_DIFF_OUT_INTERMDISABLE (IBUFDS_INTERMDISABLE_INT(x2), INV, OBUFTDS(x2)): 2 instances
  IOBUF_INTERMDISABLE => IOBUF_INTERMDISABLE (IBUF_INTERMDISABLE, OBUFT): 16 instances
  OBUFDS => OBUFDS_DUAL_BUF (INV, OBUFDS(x2)): 1 instance 
  RAM32M => RAM32M (RAMD32(x6), RAMS32(x2)): 145 instances
2default:defaultZ1-111h px? 
?
G%s Infos, %s Warnings, %s Critical Warnings and %s Errors encountered.
28*	vivadotcl2
92default:default2
12default:default2
02default:default2
02default:defaultZ4-41h px? 
]
%s completed successfully
29*	vivadotcl2
link_design2default:defaultZ4-42h px? 
?
r%sTime (s): cpu = %s ; elapsed = %s . Memory (MB): peak = %s ; gain = %s ; free physical = %s ; free virtual = %s
480*common2!
link_design: 2default:default2
00:00:092default:default2
00:00:122default:default2
1934.2112default:default2
463.8402default:default2
78312default:default2
130242default:defaultZ17-722h px? 


End Record