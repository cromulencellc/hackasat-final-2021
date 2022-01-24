
#!/bin/bash

TEAM=${TEAM_NUMBER:-1}

echo "Update Team Orbit File in Inp_Sim.txt"
pushd HAS2
ln -sf "Orb_TEAM_${TEAM}.txt" Orb_HAS2.txt
popd