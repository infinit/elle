#!/bin/sh

read -r -d '' REMOVE_FUSE4X <<END
  # The kext might not be loaded yet, so silently ignore the output
  kextunload -b org.fuse4x.kext.fuse4x 2> /dev/null;

  rm -rf /Library/Frameworks/Fuse4X.framework/;
  rm -rf /Library/Filesystems/fuse4x.fs/;
  rm -f /Library/LaunchAgents/org.fuse4x.autoupdater.plist;
  rm -f ~/Library/Preferences/org.fuse4x.autoupdater.plist;
  rm -f ~root/Library/Preferences/org.fuse4x.autoupdater.plist;
  rm -rf /Library/Extensions/fuse4x.kext/;
  rm -rf /usr/local/include/fuse*;
  rm -f /usr/local/lib/libfuse4x.*;
  rm -rf /usr/local/lib/libfuse*;
  rm -f /usr/local/lib/pkgconfig/fuse.pc;

  pkgutil --forget org.fuse4x.Fuse4X;


  # For backward compatibility. Remove next lines 2012 Q4
  rm -rf /System/Library/Extensions/fuse4x.kext/;
  rm -rf /System/Library/Filesystems/fuse4x.fs/;
  rm -f /System/Library/LaunchAgents/org.fuse4x.autoupdater.plist;
END

if [ `whoami` == 'root' ]
then
  eval "$REMOVE_FUSE4X"
else
  sudo -p "Please enter your password to uninstall fuse4x: " -s -- "$REMOVE_FUSE4X"
fi
