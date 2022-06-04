#! /bin/bash

set -e # quits the script as soon as an error happens

echo "Removing executable file to PATH"
rm $HOME/bin/bgmplayer

# Since you may use youtube-dlc or mpv, I will not remove it
# echo "Removing dependencies."
# sudo rm -r /usr/bin/bgmplayer_depends

echo "Uninstall finished! Customization file has not been removed, kept in case of re-install."
echo "You can manually remove it: rm -r ~/.ytbgmpcli"