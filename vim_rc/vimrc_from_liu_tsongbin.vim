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

" Vim5 and later versions support syntax highlighting. Uncommenting the next
" line enables syntax highlighting by default.
"syntax on

" If using a dark background within the editing area and syntax highlighting
" turn on this option as well
"set background=dark

" Uncomment the following to have Vim jump to the last position when
" reopening a file
"if has("autocmd")
"  au BufReadPost * if line("'\"") > 0 && line("'\"") <= line("$")
"    \| exe "normal g'\"" | endif
"endif

" Uncomment the following to have Vim load indentation rules according to the
" detected filetype. Per default Debian Vim only load filetype specific
" plugins.
"if has("autocmd")
"  filetype indent on
"endif

" The following are commented out as they cause vim to behave a lot
" differently from regular Vi. They are highly recommended though.
"set showcmd		" Show (partial) command in status line.
"set showmatch		" Show matching brackets.
"set ignorecase		" Do case insensitive matching
"set smartcase		" Do smart case matching
"set incsearch		" Incremental search
"set autowrite		" Automatically save before commands like :next and :make
"set hidden             " Hide buffers when they are abandoned
"set mouse=a		" Enable mouse usage (all modes) in terminals

"安装了javacomplete和taglist两个插件，都可用在vim.sf.net上下载

"Set mapleader
let mapleader = ","
let g:mapleader = ","

"显示行号
set nu

"打开语法高亮
syntax on

"set softtabstop=4
set tabstop=4
set shiftwidth=4
"set shiftwidth=4

"关闭自动备份
set nobackup

"自动缩进设置
set cindent
set smartindent
set incsearch
set autoindent

"Show matching bracets
set showmatch

"Get out of VI's compatible mode
set nocompatible

"Have the mouse enabled all the time
set mouse=a

"Set to auto read when a file is changed from the outside
set autoread

"Enable filetype plugin
filetype plugin on
filetype indent on

"设置配色方案为torte
colo desert

"设置支持的文件编码类项，目前设置为utf-8和gbk两种类型
set fileencodings=utf-8,chinese

"设置搜索结果高亮显示
"set hlsearch

"设置记录的历史操作列表
set history=50

"设置折叠
set foldcolumn=2
set foldmethod=indent
set foldlevel=3

"for minibufex plugin
let g:miniBufExplMapWindowNavVim = 1 
let g:miniBufExplMapWindowNavArrows = 1
let g:miniBufExplMapCTabSwitchBufs = 1
let g:miniBufExplModSelTarget = 1 
      
"AutoCommand
"新建.c,.h,.sh,.java文件，自动插入文件头
autocmd BufNewFile *.[ch],*.cpp,*.cc,*.sh,*.java exec ":call SetTitle()"
"新建文件后，自动定位到文件末尾
autocmd BufNewFile * normal G

"设置Java代码的自动补全
"au FileType java setlocal omnifunc=javacomplete#Complete

"绑定自动补全的快捷键<C-X><C-O>到<leader>;
imap <leader>; <C-X><C-O>

"设定开关Taglist插件的快捷键为F4，可以在VIM的左侧栏列出函数列表等
"map <F4> :Tlist<CR>

"设置程序的运行和调试的快捷键F5和Ctrl-F5
map <F5> :call CompileRun()<CR>
map <C-F5> :call Debug()<CR>

"设置tab操作的快捷键，绑定:tabnew到<leader>t，绑定:tabn, :tabp到<leader>n,
"<leader>p
map <leader>t :tabnew<CR>
map <leader>n :tabn<CR>
map <leader>p :tabp<CR>

"用cscope支持
set csprg=/usr/bin/cscope
let Tlist_Ctags_Cmd='/usr/bin/ctags'
let Tlist_Show_One_File=1
let Tlist_Exit_OnlyWindow=1
let Tlist_Use_Right_Window=1
"默认打开Taglist
"let Tlist_Auto_Open=1

"使用<leader>e打开当前文件同目录中的文件
if has("unix")
map ,e :e <C-R>=expand("%:p:h") . "/" <CR>
else
map ,e :e <C-R>=expand("%:p:h") . "\" <CR>
endif

"定义CompileRun函数，用来调用进行编译和运行
func CompileRun()
exec "w"
"C程序
if &filetype == 'c'
exec "!gcc % -g -o %<"
exec "!./%<"
"Java程序
elseif &filetype == 'java'
exec "!javac %"
exec "!java %<"
endif
endfunc
"结束定义CompileRun

"定义Debug函数，用来调试程序
func Debug()
exec "w"
"C程序
if &filetype == 'c'
exec "!gcc % -g -o %<"
exec "!gdb %<"
"Java程序
elseif &filetype == 'java'
exec "!javac %"
exec "!jdb %<"
endif
endfunc
"结束定义Debug

"定义函数SetTitle，自动插入文件头
func SetTitle()
"如果文件类型为.sh文件
if &filetype == 'sh'
call setline(1, "\#!/bin/bash")
call append(line("."), "\#########################################################################")
call append(line(".")+1, "\# Author: LT songbin")
call append(line(".")+2, "\# Created Time: ".strftime("%F %T"))
call append(line(".")+3, "\# File Name: ".expand("%"))
call append(line(".")+4, "\# Description: ")
call append(line(".")+5, "\#########################################################################")
"call append(line(".")+5, "\#!/bin/bash")
call append(line(".")+6, "")
"else
"call setline(1, "/***********************************************************************")
"call append(line("."), "* Filename : ".expand("%"))
"call append(line(".")+1, "* Create   : Songbin LIU ".strftime("%F"))
"call append(line(".")+2, "* Created Time: ".strftime("%F"))
"call append(line(".")+2, "* Description: ")
"call append(line(".")+3, "* Modified   : ")
"call append(line(".")+4, "* Revision of last commit: $Rev$ ")
"call append(line(".")+5, "* Author of last commit  : $Author$ $date$")
"call append(line(".")+6, "* licence :")
"call append(line(".")+7, "$licence$")
"call append(line(".")+8, "* **********************************************************************/")
"call append(line(".")+9, "")
else
call setline(1, "/*************************************************************************")
call append(line("."), "* Author: LT songbin")
call append(line(".")+1, "* Created Time: ".strftime("%F %T"))
call append(line(".")+2, "* File Name: ".expand("%"))
call append(line(".")+3, "* Description: ")
call append(line(".")+4, " ************************************************************************/")
call append(line(".")+5, "")
endif
endfunc

" Source a global configuration file if available
" XXX Deprecated, please move your changes here in /etc/vim/vimrc
if filereadable("/etc/vim/vimrc.local")
  source /etc/vim/vimrc.local
endif