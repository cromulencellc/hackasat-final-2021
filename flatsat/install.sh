#!/usr/bin/bash

curDir=$(pwd)
echo "Running FlatSat Install in ${curDir}"

echo "Update user $HOME/.bashrc"
bashrc_updated=$(grep "source env.sh" $HOME/.bashrc | wc -l)
if [ ${bashrc_updated} -eq 0 ]; then
    echo "Update User Bashrc to Start in FlatSat folder and source env.sh"
    cat >> "$HOME/.bashrc" <<EOF

export FLATSAT_HOME=${curDir}
cd \${FLATSAT_HOME}
source env.sh
PS1="${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;35m\][TEAM: \$TEAM_NUMBER]\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ "
export PS1=\$PS1

EOF
else
    echo "$HOME/.bashrc already updated"
fi


echo "Install APT depdendenices"
sudo apt-get update

sudo apt-get -y install git python3-serial python3-pip terminator screen wget qemu-utils jq

echo "Install Docker"
sudo apt-get -y  install \
    ca-certificates \
    curl \
    gnupg \
    lsb-release

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg

echo \
"deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
$(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt-get update
sudo apt-get -y install docker-ce docker-ce-cli containerd.io
#sudo groupadd docker
sudo usermod -aG docker $USER

echo "Install docker-compose"

sudo pip3 install docker-compose construct

echo "Install Raspberry Pi Imager to Burn SD Cards"

wget https://downloads.raspberrypi.org/imager/imager_latest_amd64.deb -P $HOME/Downloads

sudo apt -y install $HOME/Downloads/imager_latest_amd64.deb

echo "Pull and update flatsat repo git submodules"

git submodule update --init --recursive

echo "Install Board Tools Dependendencies"

sudo board_tools/install.sh

echo "Add usb group for ztex tools"
sudo groupadd usb
sudo usermod -aG usb ${USER}

echo "Update udev rules"
sudo cp config/udev/* /etc/udev/rules.d/

echo "Install GNU Radio 3.9"
sudo add-apt-repository ppa:gnuradio/gnuradio-releases
sudo apt-get update && sudo apt-get -y install gnuradio

echo "Check for Docker"
sg docker -c "docker --version"


