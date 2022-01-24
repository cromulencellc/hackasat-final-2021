To throw the exploit, just run ./sender.py. It has a variable that defines the TEAM number, just modify as necessary to throw against the specific team's container.

To throw against a specific flatsat, and a specific team, you will need to change the connect in the solver to connect to the right IP address.

Then, connect via netcat to the open port on the system: (change localhost to the right IP if necessary)
    nc localhost 136{TEAM}

This will spawn a reverse shell, where you can just echo the flag.





