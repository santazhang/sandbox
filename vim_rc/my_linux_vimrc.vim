" All system-wide defaults are set in $VIMRUNTIME/debian.vim (usually just
" /usr/share/vim/vimcurrent/debian.vim) and sourced by the call to :runtime
" you can find below.  If you wish to change any of those settings, you should
" do it in this file (/etc/vim/vimrc), since debian.vim will be overwritten
" everytime an upgrade of the vim packages is performed.  It is recommended to
" make changes after sourcing debian.vim since it alters the value of the
" 'compatible' option.

" This line should not be removed as it ensures that various options are
" properly set to work with the Vim-related packages available in Debian.
runtime! debian.vim

" Uncomment the next line to make Vim more Vi-compatible
" NOTE: debian.vim sets 'nocompatible'.  Setting 'compatible' changes numerous
" options, so any other options should be set AFTER setting 'compatible'.
"set compatible

" Vim5 and later versions support syntax highlighting. Uncommenting the
" following enables syntax highlighting by default.
if has("syntax")
  syntax on
endif

" If using a dark background within the editing area and syntax highlighting
" turn on this option as well
set background=dark

" Uncomment the following to have Vim jump to the last position when
" reopening a file
if has("autocmd")
  au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
endif

" Uncomment the following to have Vim load indentation rules and plugins
" according to the detected filetype.
if has("autocmd")
  filetype plugin indent on
endif

" The following are commented out as they cause vim to behave a lot
" differently from regular Vi. They are highly recommended though.
set showcmd		" Show (partial) command in status line.
set showmatch		" Show matching brackets.
"set ignorecase		" Do case insensitive matching
set smartcase		" Do smart case matching
set incsearch		" Incremental search
set autowrite		" Automatically save before commands like :next and :make
set hidden             " Hide buffers when they are abandoned
set mouse=a		" Enable mouse usage (all modes)

" Source a global configuration file if available
if filereadable("/etc/vim/vimrc.local")
  source /etc/vim/vimrc.local
endif

set softtabstop=2
set expandtab
set smartindent
set shiftwidth=2
set nu
set tabstop=2
set hlsearch "highlight search result
set autoread "automatically read file when it is changed by other programs
"set foldcolumn=2
"set foldmethod=indent
"set foldlevel=2

"color desert "change color schemes

":autocmd BufEnter * call DoWordComplete()
" set cursorline " show a line below cursor. 
let Tlist_Use_Right_Window=1
let Tlist_Auto_Open=1 " enable TagList by default

"insert comment header for code files
autocmd BufNewFile *.[ch],*.cpp,*.cc,*.sh,*.rb,*.py exec ":call SetHeader()"
"goto end of file when creating new files
autocmd BufNewFile * normal G

func SetHeader()
  if &filetype == 'sh' || &filetype == 'ruby' || &filetype == 'python'
    if &filetype == 'sh'
      call setline(1, "\#!/bin/bash")
    elseif &filetype == 'ruby'
      call setline(1, "\#!/usr/bin/ruby")
    elseif &filetype == 'python'
      call setline(1, "\#!/usr/bin/python")
    end
    call append(line("."), "\#########################################################################")
    call append(line(".")+1, "\# Author: Santa Zhang")
    call append(line(".")+2, "\# Created Time: ".strftime("%F %T"))
    call append(line(".")+3, "\# File Name: ".expand("%"))
    call append(line(".")+4, "\# Description: ")
    call append(line(".")+5, "\#########################################################################")
    call append(line(".")+6, "")
  else
    call setline(1, "/*************************************************************************")
    call append(line("."), "* Author: Santa Zhang")
    call append(line(".")+1, "* Created Time: ".strftime("%F %T"))
    call append(line(".")+2, "* File Name: ".expand("%"))
    call append(line(".")+3, "* Description: ")
    call append(line(".")+4, " ************************************************************************/")
    call append(line(".")+5, "")
  endif
endfunc
