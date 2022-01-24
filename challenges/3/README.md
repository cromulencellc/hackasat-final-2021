# User Segment Exploit

Requirements: docker
Python libraries: pyasn1, gmpy, pycryptodome

## Challenge

User segment takes packets from teams and routes them to the correct comm payload via RF

Periodic messages are also sent by the user segment over time to monitor SLA

Teams exploit RSA encryption bug in the authentication of the messages to be able to send to any team's satellites

## Running Solver

Copy the RSA pem files into the keys directory. Update the TEAM_R_VALUES with the r values (3rd prime) for each of the teams.

Run `./solver.py -t <team_num>` to crack a specific team. Otherwise:

Run `./solver.py` without any arguments to crack all the teams.

It will take a while. Aka 20 minutes per team (if you don't run them concurrently on different machines). Go grab a drink