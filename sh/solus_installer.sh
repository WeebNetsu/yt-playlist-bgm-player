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

echo "Installing dependencies. (will need sudo permissions)"
sudo eopkg install mpv python3

youtube-dlc --version || ( sudo wget https://github.com/blackjack4494/yt-dlc/releases/latest/download/youtube-dlc -O $HOME/bin/youtube-dlc && sudo chmod a+rx $HOME/bin/youtube-dlc )

echo "Adding $HOME/bin to PATH."
echo -e '\nexport PATH="$HOME/bin:$PATH"' >> $HOME/.bashrc
export PATH="$HOME/bin:$PATH"

echo "Install Finished!"
echo "It might not work until after a reboot!"

read -p 'Would you like to reboot? [y/n]: ' reboot

if [[ $reboot == "y" ]]; then
   echo "Will reboot in 20 seconds. Close any running programs, or ctrl+c to cancel."
   sleep 20
   init 6
fi