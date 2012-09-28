" This project setting will automatically open NERDTree

function! StartWithNERDTree()
        NERDTree
        " switch to code window
        :wincmd w
endfunction

autocmd VimEnter * call StartWithNERDTree()

let NERDTreeIgnore = ['\.log', '\.dSYM', '\.d$', '\o$', '\.a$']
