# Windowless Vimrun.exe

This program is meant to replace the original `vimrun.exe` provided with GVim for Windows.
The original `vimrun.exe` will make a console window flash in the background
which is annoying and slows down command execution.

This version of `vimrun.exe` is able to execute a command in the background without
flashing a window when the `-s` flag is provided.

## Installation

- [Download](https://github.com/2072/Windowless-vimrun/releases) the `vimrun.exe` binary (available in 32 bit and 64 bit).
- Drop it anywhere in your PATH before GVim's directory or simply replace the original `vimrun.exe`.

- add the following commands to your .vimrc:

```
:set shellxescape-=\>
:set shellxescape-=\&
```

These are used to tell VIm which characters to escape when sending commands to
the shell (cmd.exe). Unfortunately many plug-ins are not using these correctly
and end up escaping output redirection commands which then cause them to
fail... I'm not sure why this works when using the default vimrun.exe, maybe
some 'helpful' Microsoft compatibility magic is taking place when using the
system() API call...

## Limitations:

This program has some limitations and requires `cmd.exe` to be used as shell
(which is the default setting in Windows based GVim installations).

### Explanation:

Supported usage (same as the original vimrun.exe):

    vimrun.exe [-s] executable [parameters]


When the `-s` flag is not provided Windowless Vimrun will allow
`executable` to show its window and will assume `executable` to be
`cmd.exe` compatible and will simply add `&& pause` at the end of `parameters`...

This is not satisfactory but does the trick until I spend more time on this to
find a better solution.

**Pull request are very much welcomed!**

## Building:

This can be built manually using MinGW:

    git clone https://github.com/2072/Windowless-vimrun.git
    cd Windowless-vimrun
    make


## Acknowledgements

This program is inspired from [vimrun-silent](https://github.com/leonid-shevtsov/vimrun-silent) written by
Leonid Shevtsov and released under the MIT License.


