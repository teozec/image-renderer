_complete()
{
	local cur prev
	COMREPLY=()
	cur=${COMP_WORDS[COMP_CWORD]}
	prev=${COMP_WORDS[COMP_CWORD-1]}
	if [ $COMP_CWORD -eq 1 ]; then
		COMPREPLY=($(compgen -W "-h --help demo pfm2ldr" -- $cur))
	else
		local action
		action=${COMP_WORDS[1]}
		case "$action" in
		"pfm2ldr")
			if [[ " ${COMP_WORDS[@]} " =~ " bmp " ]]; then
				COMPREPLY=($(compgen -A file -W "-h --help -a --afactor= -g -gamma=" -- $cur))
			elif [[ " ${COMP_WORDS[@]} " =~ " gif " ]]; then
				COMPREPLY=($(compgen -A file -W "-h --help -a --afactor= -g -gamma=" -- $cur))
			elif [[ " ${COMP_WORDS[@]} " =~ " jpeg " ]]; then
				COMPREPLY=($(compgen -A file -W "-h --help -a --afactor= -g -gamma= -q --quality=" -- $cur))
			elif [[ " ${COMP_WORDS[@]} " =~ " png " ]]; then
				COMPREPLY=($(compgen -A file -W "-h --help -a --afactor= -g -gamma= -p --palette -c --compression=" -- $cur))
			elif [[ " ${COMP_WORDS[@]} " =~ " tiff " ]]; then
				COMPREPLY=($(compgen -A file -W "-h --help -a --afactor= -g -gamma=" -- $cur))
			elif [[ " ${COMP_WORDS[@]} " =~ " webp " ]]; then
				COMPREPLY=($(compgen -A file -W "-h --help -a --afactor= -g -gamma= -q --quality=" -- $cur))
			else
				COMPREPLY=($(compgen -W "-h --help -a --afactor= -g -gamma= bmp gif jpeg png tiff webp" -- $cur))
			fi
			;;
		"demo")
			if [[ $prev == "-o" || $prev == "--outfile=" ]]; then
				COMPREPLY=($(compgen -A file -- $cur))
			elif [[ $prev == "p" || $prev == "projection=" ]]; then
				COMPREPLY=($(compgen -W "perspective orthogonal" -- $cur))
			else
				COMPREPLY=($(compgen -W "--help -w --width= -h -height= -p --projection= --angleDeg= -o --outfile=" -- $cur))
			fi
			;;
		esac
	fi
	return 0
}
complete -F _complete image-renderer
