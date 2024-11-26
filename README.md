# NexShell
A Unix style shell interface that uses the FatFs for a file system and a virtual file system.

The best way to explain this right now is that it is a shell like the microshell overlaid on an actual file system supported by the FatFs.

There will be an OS added and support for Unix style pipes so file/command output can be pipped into another file/command.

There will also be support for devices in the dev folder.

First port will be unknown (Probably one of the dev boards from SMT for the discovery kits), right now it is just a VS 2022 RAM drive with some virtual files supported in dev (only rtc is supported right now, serial will be next).

This is still a work in progress with no official first release.

For documentation visit: https://pic32.github.io/NexShell/
