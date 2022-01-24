
target extended-remote localhost:2222

set remote exec-file /apps/opensatkit/cfs/build_osk_cdh/exe/core-cpu1.exe

set follow-fork-mode child

set pagination off

set radix 16

#break Init

#catch load

set pagination off

load

run

