main:
    eor x0, x0, x0
    ldr x0, flag
    ldr x9, send_to_telemetry
    br x9

flag:
    .dword 0x800000
send_to_telemetry:
    .word 0x40118c

