#! /bin/bash

# echo "Creating Directory in PATH"
# sudo mkdir /usr/bin/ytpbgmp

echo "Adding executable file to PATH"
cp output bgmplayer
sudo mv bgmplayer /usr/bin/

echo "Adding dependencies"
cp -r depends bgmplayer_depends
sudo mv bgmplayer_depends /usr/bin/

echo "Install finished! Run the application with this command: bgmplayer"