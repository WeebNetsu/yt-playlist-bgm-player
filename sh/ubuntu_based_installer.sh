#!/bin/bash

# running as root will set the $HOME dir to /root/ and not /home/user/
if [[ $EUID -eq 0 ]]; then
   echo "This script should not be run as root"
   exit 1
fi

echo "Adding executable file to $HOME/bin"
chmod +x bgmplayer
[ ! -d "$HOME/bin" ] && echo "Could not find $HOME/bin folder... Creating it." && mkdir -p $HOME/bin 
[ -d "$HOME/bin" ] && cp bgmplayer $HOME/bin

echo "Installing dependencies. (apt & wget) (will need sudo permissions)"
sudo add-apt-repository ppa:mc3man/mpv-tests -y && sudo apt update && sudo apt-get remove mpv -y && sudo apt install mpv python -y 

youtube-dlc --version || ( sudo wget https://github.com/blackjack4494/yt-dlc/releases/latest/download/youtube-dlc -O /usr/local/bin/youtube-dlc && sudo chmod a+rx /usr/local/bin/youtube-dlc )

# echo "Install finished! Run the application with this command: bgmplayer"

read -p 'Install finished! You have to reboot before you can use bgmplayer. Reboot [y/n]: ' reboot

if [[ $reboot == "y" ]]; then
   echo "Will reboot in 15 seconds. (ctrl+c to cancel reboot)"
   sleep 15
   init 6
fi

# STILL WORKING ON THIS
# if [[ uname -eq "Linux" ]]; then
#    echo "Linux!"
#    echo "Adding executable file to /usr/local/bin"
#    chmod +x bgmplayer
#    # [ ! -d "$HOME/bin" ] && echo "Could not find $HOME/bin folder... Creating it." && mkdir -p $HOME/bin 
#    # [ -d "$HOME/bin" ] && cp bgmplayer $HOME/bin
#    sudo cp bgmplayer /usr/local/bin/

#    echo "Installing dependencies. (apt & wget) (will need sudo permissions)"
#    sudo add-apt-repository ppa:mc3man/mpv-tests -y && sudo apt update && sudo apt-get remove mpv -y && sudo apt install mpv python -y 

#    youtube-dlc --version || ( sudo wget https://github.com/blackjack4494/yt-dlc/releases/latest/download/youtube-dlc -O /usr/local/bin/youtube-dlc && sudo chmod a+rx /usr/local/bin/youtube-dlc )

#    # echo "Install finished! Run the application with this command: bgmplayer"

#    read -p 'Install finished! You may have to reboot before you can use bgmplayer. Reboot [y/n]: ' reboot

#    if [[ $reboot == "y" ]]; then
#       echo "Will reboot in 15 seconds. (ctrl+c to cancel reboot)"
#       sleep 15
#       reboot
#    fi
# else
#    echo "This script can only be ran on Linux"
#    exit 1
# fi