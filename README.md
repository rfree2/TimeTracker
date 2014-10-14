## Time Tracker 
Simple Time Tracker written in C++11 based on switching X11 workspaces,
logging time information to readable text files.
(mainly for linux etc - see Comments)

### Building  
In TimeTracker directory:
``` 
cd Debug && make 
``` 

### Needed dependencies 
Libraries: boost_fileststem; boost_date_time; X11 (xlib)

### Other info 
Time Tracker gets X11 desktop name.
Will also use X11 window name.
Currently uses wmctrl with grep and awk to detect current workspace name.

### TODO
* parse in C++11, not with grep
* get deskto name with Xlib directly, not by calling wmctrl program
* get window name
* allow exaxt rules (in code)
* allow rules from simple text file

### Comments 
This is a beta version! Tested on Debian Wheezy and Gnome Classic only!

### Licence

OpenBSD licence.

