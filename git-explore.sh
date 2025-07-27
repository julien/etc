#!/usr/bin/env bash
set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

main() {
    git rev-parse 2> /dev/null || return 1

    [[ "$EDITOR" =~ n?vim ]] || return 1

    local log="git --no-pager log --oneline --color=always"

    local opts=( "$@" )
    if [[ "${#opts[@]}" -eq 0 ]]; then
        log="$log -n 128"
    else
        for opt in "${opts[@]}"; do log="$log $opt"; done;
    fi

    local cmd="$EDITOR \\
    '+nnoremap q :bd!<CR>' '+nnoremap Q :qa!<CR>' \\
    \"+nnoremap <silent> K 0:tabnew \| setfiletype git \| syntax on \| exe 'read! git --no-pager show <C-r><C-w>' \| norm ggdd<CR>\" \\
    "
    [[ "$EDITOR" == "nvim" ]] && cmd="$cmd \"+term $log\""
    [[ "$EDITOR" == "vim"  ]] && \
        cmd="$cmd \"+call term_start('$log', {'hidden': 1, 'term_finish': 'open', 'term_opencmd': 'buffer %d'})\""

    eval "$cmd"
}

main "$@"
