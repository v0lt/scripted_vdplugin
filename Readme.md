# A simple AviSynth / VapourSynth script editor plugin for VirtualDub2
---

It is a port from script editor build in VirtualDubMod with some extra features added and issues fixed.

### Currently implemented for ALL:
|                |                                    |
|----------------|------------------------------------|
| CTRL/G         | Goto (Script Line or Frame Number) |
| CTRL/Z         | Undo                               |
| CTRL/Y         | Redo                               |
| CTRL/F         | Find                               |
| F3             | Find Next                          |
| SHIFT/F3       | Find Prev                          |
| CTRL/N         | New file                           |
| CTRL/O         | Open File                          |
| CTRL/S         | Save File                          |
| F5             | Save and reopen at current frame.  |
| F7             | Save and open.                     |
| Menu Only      | Revert                             |
| CTRL/SHIFT/O   | Insert FileName                    |


### Currently implemented for Avisynth:
* Syntax hi-liting
* external functions hi-liting and completion
* Auto-opening of .avs files

|                  |                                                                       |
|------------------|-----------------------------------------------------------------------|
| CTRL/P           | Insert current frame number                                           |
| CTRL/R           | Insert Range                                                          |
| CTRL/T           | Insert Trim with range                                                |
| CTRL/I           | Import Frameset as Trims (replicate VDub range deletions into script).|
| CTRL/SHIFT/ALT/C | Comment selected.                                                     |
| CTRL/SHIFT/ALT/U | UnComment selected.                                                   |
| CTRL/SPACE       | Start code completion                                                 |


### Currently implemented for VapourSynth:

* Syntax hi-liting
* Auto-opening of .vpy files

|                |                             |
|----------------|-----------------------------|
| CTRL/P         | Insert current frame number |


To access the plugin: Go to **Tools -> Script Editor**

You can set a hotkey for it. Go to **Options -> Keyboard shortcuts** and find **Tools.ScriptEditor**

You may change plugin specific options from Script Editor menu or from VirtualDub main menu: go to **Options -> Plug-ins**.

### Links:
[Description on VirtualDub2 Wiki](https://sourceforge.net/p/vdfiltermod/wiki/scripted/)

## Donate

ЮMoney - https://yoomoney.ru/to/4100115126389817
