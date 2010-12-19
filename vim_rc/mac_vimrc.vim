" This is Santa Zhang's vimrc file.
" santa1987@gmail.com

" enable syntax 
syntax on

" set color scheme
set background=light

" enable mouse in all modes
set mouse=a

"enable filetype plugin
if has("eval")
filetype plugin on
filetype indent on
endif

" automatically read file when it is changed from the outside
set autoread

" automatically write when calling commands like :next and :make
set autowrite

" smart case when searching
set smartcase

" incremental search
set incsearch

" show (partial) command in status line
set showcmd

" internationalization
if has("multi_byte")
set termencoding=chinese
set encoding=utf-8
set fileencodings=ucs-bom,utf-8,chinese
endif

" turn on WiLd menu
set wildmenu

" always show current position
set ruler

" command bar is 2 high
set cmdheight=2

" show line number
set nu

" set backspace
set backspace=eol,start,indent

" backspace and cursor keys wrap to
set whichwrap+=<,>,h,l

" set magic on
set magic

" show matching bracet
set showmatch

" highlight search
set hlsearch

" set tab and spaces
set softtabstop=2
set expandtab
set smartindent
set shiftwidth=2
set tabstop=2

" set up the taglist plugin
let Tlist_Use_Right_Window=1
let Tlist_Auto_Open=1

" personalized color scheme, based on peachpuff
hi clear
let g:colors_name="santa_zhang"

hi SpecialKey term=bold ctermfg=4 guifg=Blue
hi NonText term=bold cterm=bold ctermfg=4 gui=bold guifg=Blue
hi Directory term=bold ctermfg=4 guifg=Blue
hi ErrorMsg term=standout cterm=bold ctermfg=7 ctermbg=1 gui=bold guifg=White guibg=Red
hi IncSearch term=reverse cterm=reverse gui=reverse
hi Search term=reverse ctermbg=3 guibg=Gold2
hi MoreMsg term=bold ctermfg=2 gui=bold guifg=SeaGreen
hi ModeMsg term=bold cterm=bold gui=bold
hi LineNr term=underline ctermfg=6 guifg=Red3
hi Question term=standout ctermfg=2 gui=bold guifg=SeaGreen
hi StatusLine term=bold,reverse cterm=bold,reverse gui=bold guifg=White guibg=Black
hi StatusLineNC term=reverse cterm=reverse gui=bold guifg=PeachPuff guibg=Gray45
hi VertSplit term=reverse cterm=reverse gui=bold guifg=White guibg=Gray45
hi Title term=bold ctermfg=5 gui=bold guifg=DeepPink3
hi Visual term=reverse cterm=reverse gui=reverse guifg=Grey80 guibg=fg
hi VisualNOS term=bold,underline cterm=bold,underline gui=bold,underline
hi WarningMsg term=standout ctermfg=1 gui=bold guifg=Red
hi WildMenu term=standout ctermfg=0 ctermbg=3 guifg=Black guibg=Yellow
hi Folded term=standout ctermfg=4 ctermbg=7 guifg=Black guibg=#e3c1a5
hi FoldColumn term=standout ctermfg=4 ctermbg=7 guifg=DarkBlue guibg=Gray80
hi DiffAdd term=bold ctermbg=4 guibg=White
hi DiffChange term=bold ctermbg=5 guibg=#edb5cd
hi DiffDelete term=bold cterm=bold ctermfg=4 ctermbg=6 gui=bold guifg=LightBlue guibg=#f6e8d0
hi DiffText term=reverse cterm=bold ctermbg=1 gui=bold guibg=#ff8060
hi Cursor guifg=bg guibg=fg
hi lCursor guifg=bg guibg=fg

" Colors for syntax highlighting
hi Comment term=bold ctermfg=4 guifg=#406090
hi Constant term=underline ctermfg=1 guifg=#c00058
hi Special term=bold ctermfg=5 guifg=SlateBlue
hi Identifier term=underline ctermfg=1 guifg=DarkCyan
hi Statement term=bold ctermfg=5 gui=bold guifg=Brown
hi PreProc term=underline ctermfg=5 guifg=Magenta3
hi Type term=underline ctermfg=4 gui=bold guifg=SeaGreen
hi Ignore cterm=bold ctermfg=7 guifg=bg
hi Error term=reverse cterm=bold ctermfg=7 ctermbg=1 gui=bold guifg=White guibg=Red
hi Todo term=standout ctermfg=0 ctermbg=3 guifg=Blue guibg=Yellow
