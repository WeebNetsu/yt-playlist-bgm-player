#!/bin/bash

echo "This script does not yet work for installing Nim. Please make sure you have both Nim and Nimble installed before running this script."

PRESENT_DIR=$(basename `pwd`)

# make sure we're inside the sh folder
if [[ $PRESENT_DIR != "sh" ]]; then
    echo "Please run this script from the sh/ folder."
    exit 1
fi

# running as root will set the $HOME dir to /root/ and not /home/user/
if [[ $EUID -eq 0 ]]; then
   echo "This script should not be run as root"
   exit 1
fi

pushd "$(dirname "$0")"
trap popd EXIT

chmod +x ./check_install.sh && ./check_install.sh && CHECKS_SUCCESS=true

echo "Getting package manager..."

MANAGER="error"
apt version && MANAGER="apt"
pacman --version && MANAGER="pacman"

# we need to check if Nim (and Nimble) is installed first
echo "Installing dependencies. (will need sudo permissions)"
if [[ $MANAGER == "pacman" ]]; then
   if [[ $CHECKS_SUCCESS != true ]]; then
      # pacman nim contains both nim and nimble on install
      pacman -S nim --noconfirm
   fi

   sudo pacman -S mpv --noconfirm --needed
# todo: check if ubuntu based distros have nim version 1.6 or higher
# todo: check if ubuntu based distros have nimble
elif [[ $MANAGER == "apt" ]]; then # idk what cmake is on ubuntu yet, so I can't add it to installation yet
   if [[ $CHECKS_SUCCESS != true ]]; then
      echo "\n\tPlease install Nim (v1.6 or higher) and Nimble, this script does not support installing it yet."
      exit 1
   fi
   sudo add-apt-repository ppa:mc3man/mpv-tests -y && sudo apt update && sudo apt-get remove mpv -y && sudo apt install mpv python -y
else
   echo "ERROR! Could not find your package manager! If you're not running Apt or Pacman, please install MPV, Nim, Nimble and Python 3 manually."
   exit 1
fi

yt-dlp --version || ( sudo wget https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -O /usr/local/bin/yt-dlp && sudo chmod a+rx /usr/local/bin/yt-dlp )

chmod +x ./compile.sh && ./compile.sh && COMPILE_SUCCESS=true

if [[ $COMPILE_SUCCESS != true ]]; then
   echo -e "\n\tError compiling program!\n"
   exit 1
fi

set -e # quits the script as soon as an error happens

echo "Adding executable file to $HOME/bin"
chmod +x bgmplayer
[ ! -d "$HOME/bin" ] && echo "Could not find $HOME/bin folder... Creating it." && mkdir -p $HOME/bin 
[ -d "$HOME/bin" ] && cp bgmplayer $HOME/bin

echo "Adding $HOME/bin to PATH."
echo -e '\nexport PATH="$HOME/bin:$PATH"' >> $HOME/.bashrc
export PATH="$HOME/bin:$PATH"

echo "Install Finished!"
echo "It might not work until after a reboot!"

read -p 'Would you like to reboot? [y/n]: ' reboot

if [[ $reboot == "y" ]]; then
   echo "Will reboot in 20 seconds. Close any running programs, or ctrl+c to cancel."
   sleep 20
   reboot
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
