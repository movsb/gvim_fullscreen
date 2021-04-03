# gvim_fullscreen

## What it does?

* Toggle GVim on Windows into fullscreen mode.
* Change GVim whole window's transparency.

Inspired by [derekmcloughlin/gvimfullscreen_win32](https://github.com/derekmcloughlin/gvimfullscreen_win32/), but totally written by myself, the easier way.

## How to use

Put gvim_fullscreen.dll into the same directory where gvim.exe located.

In your `_vimrc` file, write down these lines:

```vim
" toggle fullscreen mode by pressing F11
noremap <f11> <esc>:call libcallnr('gvim_fullscreen.dll', 'ToggleFullscreen', 0)<cr>
" toggle window transparency to 247 or 180 by pressing F12
noremap <f12> <esc>:call libcallnr('gvim_fullscreen.dll', 'ToggleTransparency', "247,180")<cr>
```

You'd better remove your gvim's toolbar, scrollbar, menubar to have a better look:

```vim
set guioptions-=T
set guioptions-=r
set guioptions-=L
set guioptions-=m
```

You can get all `guioptions` here: <https://vimhelp.org/options.txt.html#%27guioptions%27> .

## How to compile

Open *Visual Studio Command Prompt*, `cd` into *gvim_fullscrren* and then type `nmake`, you will get `gvim_fullscreen.dll`.
