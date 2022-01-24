== Intro ==

This is the folder for challenge 4: CDH exploit. It uses the comm app and the mqtt message passing bus to operate.

Prerequisites: mosquitto-clients
 - This gives you mosquitto_pub

1) Run mosquitto

2) Run the digital twin version of the cdh

3) run the python script

4) enjoy throwing code at the cdh!

Optional: install mqtt-explorer via "snap install mqtt-explorer"
 - This gives you a GUI application to debug the messages, and is unncessessary.
 - unfortunately, it doesnt work well with null bytes. This is solved by just using the standard mosquitto_pub client, and just using the "-f" switch to send bytes from a file.

== Building shellcode ==

You have to assemble it on the sparc docker container.

Add a hardlink to the exploit.s in the opensatkit/cfs directory so the docker vm has access to the file.

ln <path to digitaltwin>/opensatkit/cfs/exploit

Also, create a hardlink in this directory to the compiled binary in cfs.

ln <path to digitaltwin>/opensatkit/cfs/exploit

 - First, add the line '/bin/bash' to the build_mission.sh script. This will give you an easy shell on it to build with.
 - run ./build.sh in the cfs repo.

This gives you a shell in the rtems docker container.

Compile the assembly into a binary that you can rip from

 - sparc-rtems5-gcc -c exploit.s -o exploit

Make sure the exploit file was updated in the cdh-exploit challenge, otherwise copy the exploit executable from cfs

== Testing ==

1) Run the cdh with hog from digitaltwin/cdh_sparc_emu/cdh_board_image (gdb -x gdb_commands), and make sure mosquitto is connected.

2) Connect to the cdh with gdb (from cdh-exploit folder): gdb-multiarch -x cdhgdb

3) Throw the payload by just running python3 exploitgen.py

4) This will exploit the cdh, causing a buffer overflow and killing the task. The action that teams will have to do is simply to run some shellcode to change the attribution key on the cdh.






