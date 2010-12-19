"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" _
" __ | 
" / | /
" __ | 
" by Amix - http://amix.dk/
"
" Maintainer: Amir Salihefendic <amix3k at gmail.com>
" Version: 2.0
" Last Change: 12/08/06 13:39:28
" Fixed (win32 compatible) by: redguardtoo <chb_sh at gmail.com>
"
" Sections:
" ----------------------
" *> General
" *> Colors and Font
" *> Fileformat
" *> VIM userinterface
" ------ *> Statusline
" *> Visual
" *> Moving around and tab
" *> General Autocommand
" *> Parenthesis/bracket expanding
" *> General Abbrev
" *> Editing mappings etc.
" *> Command-line config
" *> Buffer realted
" *> Files and backup
" *> Folding
" *> Text option
" ------ *> Indent
" *> Spell checking
" *> Plugin configuration
" ------ *> Yank ring
" ------ *> File explorer
" ------ *> Minibuffer
" ------ *> Tag list (ctags) - not used
" ------ *> LaTeX Suite thing
" *> Filetype generic
" ------ *> Todo
" ------ *> VIM
" ------ *> HTML related
" ------ *> Ruby & PHP section
" ------ *> Python section
" ------ *> Cheetah section
" ------ *> Java section
" ------ *> JavaScript section
" ------ *> C mapping
" ------ *> SML
" ------ *> Scheme binding
" *> Snippet
" ------ *> Python
" ------ *> javaScript
" *> Cope
" *> MISC
"
" Tip:
" If you find anything that you can't understand than do this:
" help keyword OR helpgrep keyword
" Example:
" Go into command-line mode and type helpgrep nocompatible, ie.
" :helpgrep nocompatible
" then press <leader>c to see the results, or :botright cw
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => General
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Get out of VI's compatible mode..
set nocompatible

function! MySys()
return "win32"
endfunction
"Set shell to be bash
if MySys() == "linux" || MySys() == "mac"
set shell=bash
else
"I have to run win32 python without cygwin
"set shell=E:\cygwin\bin\sh
endif

"Sets how many lines of history VIM har to remember
set history=400

"Enable filetype plugin
if has("eval")
filetype plugin on
filetype indent on
endif

"Set to auto read when a file is changed from the outside
set autoread

"Have the mouse enabled all the time:
set mouse=a

"Set mapleader
let mapleader = ","
let g:mapleader = ","

"Fast saving
nmap <leader>w :w!<cr>


"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => Colors and Font
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Enable syntax hl
syntax on

"Set font to Monaco 10pt
"if MySys() == "mac"
" set gfn=Bitstream Vera Sans Mono:h14
" set nomacatsui
" set termencoding=macroman
"elseif MySys() == "linux"
" set gfn=Monospace 11
" "set encoding=utf-8
"endif

"internationalization
"I only work in Win2k Chinese version
if has("multi_byte")
set termencoding=chinese
set encoding=utf-8
set fileencodings=ucs-bom,utf-8,chinese
endif

"if you use vim in tty, 
"'uxterm -cjk' or putty with option 'Treat CJK ambiguous characters as wide' on
if has("ambiwidth")
set ambiwidth=double
endif

if has("gui_running")
set guioptions-=m
set guioptions-=T
set guioptions-=l
set guioptions-=L
set guioptions-=r
set guioptions-=R

if MySys()=="win32"
"start gvim maximized
if has("autocmd")
au GUIEnter * simalt ~x
endif
endif
"let psc_style='cool'
"colorscheme ps_color
"colorscheme default
else
"set background=dark
"colorscheme default
endif

"Some nice mapping to switch syntax (useful if one mixes different languages in one file)
map <leader>1 :set syntax=cheetah<cr>
map <leader>2 :set syntax=xhtml<cr>
map <leader>3 :set syntax=python<cr>
map <leader>4 :set ft=javascript<cr>
map <leader>$ :syntax sync fromstart<cr>

"Highlight current
if has("gui_running")
if has("cursorline")
set cursorline
"hi cursorline guibg=#333333 
"hi CursorColumn guibg=#333333
endif
endif

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => Fileformat
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Favorite filetype
set ffs=unix,dos,mac

nmap <leader>fd :se ff=dos<cr>
nmap <leader>fu :se ff=unix<cr>



"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => VIM userinterface
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Set 7 lines to the curors - when moving vertical..
set so=7

"Turn on WiLd menu
set wildmenu

"Always show current position
set ruler

"The commandbar is 2 high
set cmdheight=2

"Show line number
set nu

"Do not redraw, when running macros.. lazyredraw
set lz

"Change buffer - without saving
set hid

"Set backspace
set backspace=eol,start,indent

"Bbackspace and cursor keys wrap to
set whichwrap+=<,>,h,l

"Ignore case when searching
"set ignorecase
set incsearch

"Set magic on
set magic

"No sound on errors.
set noerrorbells
set novisualbell
set t_vb=

"show matching bracet
set showmatch

"How many tenths of a second to blink
set mat=4

"Highlight search thing
set hlsearch

""""""""""""""""""""""""""""""
" => Statusline
""""""""""""""""""""""""""""""
"Always hide the statusline
set laststatus=2

function! CurDir()
let curdir = substitute(getcwd(), '/Users/amir/', "~/", "g")
return curdir
endfunction

"Format the statusline
set statusline=
set statusline+=%f "path to the file in the buffer, relative to current directory
set statusline+=\ %h%1*%m%r%w%0* " flag
set statusline+=\ [%{strlen(&ft)?&ft:'none'}, " filetype
set statusline+=%{&encoding}, " encoding
set statusline+=%{&fileformat}] " file format
set statusline+=\ CWD:%r%{CurDir()}%h
set statusline+=\ Line:%l/%L


""""""""""""""""""""""""""""""
" => Visual
""""""""""""""""""""""""""""""
" From an idea by Michael Naumann
function! VisualSearch(direction) range
let l:saved_reg = @"
execute "normal! vgvy"
let l:pattern = escape(@", '\/.*$^~[]')
let l:pattern = substitute(l:pattern, " $", "", "")
if a:direction == 'b'
execute "normal ?" . l:pattern . "^M"
else
execute "normal /" . l:pattern . "^M"
endif
let @/ = l:pattern
let @" = l:saved_reg
endfunction

"Basically you press * or # to search for the current selection !! Really useful
vnoremap <silent> * :call VisualSearch('f')<CR>
vnoremap <silent> # :call VisualSearch('b')<CR>


"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => Moving around and tab
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Map space to / and c-space to ?
map <space> /
"map <c-space> ?

"Smart way to move btw. window
map <C-j> <C-W>j
map <C-k> <C-W>k
map <C-h> <C-W>h
map <C-l> <C-W>l

"Actually, the tab does not switch buffers, but my arrow
"Bclose function ca be found in "Buffer related" section
map <leader>bd :Bclose<cr>
map <down> <leader>bd
"Use the arrows to something usefull
map <right> :bn<cr>
map <left> :bp<cr>

"Tab configuration
map <leader>tn :tabnew %<cr>
map <leader>tc :tabclose<cr>
map <leader>tm :tabmove 
if has("usetab")
set switchbuf=usetab
endif

if has("stal")
set stal=2
endif

"Moving fast to front, back and 2 sides ;)
imap <m-$> <esc>$a
imap <m-0> <esc>0i
imap <D-$> <esc>$a
imap <D-0> <esc>0i


"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => General Autocommand
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Switch to current dir
map <leader>cd :cd %:p:h<cr>


"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => Parenthesis/bracket expanding
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
vnoremap $1 <esc>`>a)<esc>`<i(<esc>
")
vnoremap $2 <esc>`>a]<esc>`<i[<esc>
vnoremap $3 <esc>`>a}<esc>`<i{<esc>
vnoremap $$ <esc>`>a"<esc>`<i"<esc>
vnoremap $q <esc>`>a'<esc>`<i'<esc>
vnoremap $w <esc>`>a"<esc>`<i"<esc>

"Map auto complete of (, ", ', [
"http://www.vim.org/tips/tip.php?tip_id=153
"
ino ( ()<esc>:let leavechar=")"<cr>i
ino { {}<esc>:let leavechar="}"<cr>i
ino $q ''<esc>:let leavechar="'"<cr>i
ino $w ""<esc>:let leavechar='"'<cr>i
imap <c-l> <esc>:exec "normal f" . leavechar<cr>a

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => General Abbrev
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Comment for C like language
if has("autocmd")
au BufNewFile,BufRead *.js,*.htc,*.c,*.tmpl,*.css ino $c /**<cr> **/<esc>O
endif

"My information
ia xdate <c-r>=strftime("%d/%m/%y %H:%M:%S")<cr>
"iab xname Amir Salihefendic

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" => Editing mappings etc.
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"Remap VIM 0
map 0 ^

"Move a line of text using control
nmap <M-j> mz:m+<cr>`z
nmap <M-k> mz:m-2<cr>`z
vmap <M-j> :m'>+<cr>`<my`>mzgv`yo`z
vmap <M-k> :m'<-2<cr>`>my`