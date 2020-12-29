#! /bin/bash

# echo "Creating Directory in PATH"
# sudo mkdir /usr/bin/ytpbgmp

echo "Adding executable file to PATH"
cp output bgmplayer
sudo mv bgmplayer /usr/bin/

echo "Adding dependencies. Ignore error message if updating application."
cp -r depends bgmplayer_depends
sudo mv bgmplayer_depends /usr/bin/ || rm -r bgmplayer_depends

echo "Install finished! Run the application with this command: bgmplayer"