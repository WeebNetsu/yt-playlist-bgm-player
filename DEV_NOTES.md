# DEV NOTES

<!-- This file is notes for the developer -->

## Building a release

`nimble build -d:release` - the resulting executable in the root directory of the project can be uploaded as a release version of the software

## Publish to AUR

<!-- https://www.youtube.com/watch?v=iUz28vbWgVw -->

1. Update package version in `pkgver` in PKGBUILD
1. Test with `makepkg -si`
1. `makepkg --printsrcinfo > .SRCINFO`
1. Make sure you have the SSH key, if not use `ssh-keygen` and save the public key on the AUR in the settings
1. `git clone ssh://aur@aur.archlinux.org/ytbgmplayer-git.git`
1. If you are getting any access errors, you may need to add the following to your `.ssh/config` file

```
Host aur.archlinux.org
    IdentityFile ~/.ssh/aur
```

1. Copy the new `PKGBUILD` and `.SRCINFO` to the cloned AUR package
1. Commit and push
