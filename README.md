# CompactLog User Manuel

CompactLog is a command-line tool that compacts multiple sequential same log lines into a single line, such as:
```
  2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe
  
  2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe
  
  2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe
  ```
Into a compact version: 
 ``` 
  2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe * {number of repeated}
```
## Setting Up
After downloading the file from GitHub, initialize the tool in the same folder as shown below before using the given command syntax.
```
gcc CompactLog.c -o CompactLog
```

## Command Syntax
```
CompactLog -i  {input_logfile} -o {output_logfile} 

CompactLog -h  // help syntax

CompactLog // help syntax
```
