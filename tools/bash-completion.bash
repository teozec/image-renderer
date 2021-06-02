_complete_image_renderer()
{
	local cur prev
	COMPREPLY=()

	# The word being completed
	cur=${COMP_WORDS[COMP_CWORD]}

	# The word before the one being completed
	prev=${COMP_WORDS[COMP_CWORD-1]}

	# If the word being completed is the first, complete with action names
	if [ $COMP_CWORD -eq 1 ]; then
		case "${cur}" in
		--*)	# Double dash argument
			COMPREPLY=($(compgen -W "--help" -- $cur))
			;;
		-*)	# Single dash argument
			COMPREPLY=($(compgen -W "-h" -- $cur))
			;;
		*)	# Action
			COMPREPLY=($(compgen -W "demo pfm2ldr" -- $cur))
			;;
		esac

	# Otherwise, the completion depends on the action
	else
		local action
		action=${COMP_WORDS[1]}
		case "$action" in
		"pfm2ldr")
			# The argument after afactor, gamma, quality or compression does not require autocompletion because it is a number specified by the user
			if [[ "${prev}" == "-a" || "${prev}" == "-g" || "${prev}" == "-q" || "${prev}" == "-c" || \
				"${cur}" == "--afactor=*" || "${cur}" == "--gamma=*" || "${cur}" == "--quality=*" || "${cur}" == "--compression=*" ]]; then
				return 0
			fi

			# Set the suitable autocompletions depending on the action
			local single_dash double_dash no_format
			if [[ " ${COMP_WORDS[@]} " =~ " bmp " ]]; then
				single_dash="-h -a -g"
				double_dash="'--help ' '--afactor=' '--gamma='"
			elif [[ " ${COMP_WORDS[@]} " =~ " gif " ]]; then
				single_dash="-h -a -g"
				double_dash="'--help ' '--afactor=' '--gamma='"
			elif [[ " ${COMP_WORDS[@]} " =~ " jpeg " ]]; then
				single_dash="-h -a -g -q"
				double_dash="'--help ' '--afactor=' '--gamma=' '--quality='"
			elif [[ " ${COMP_WORDS[@]} " =~ " png " ]]; then
				single_dash="-h -a -g -p -c"
				double_dash="'--help ' '--afactor=' '--gamma=' '--palette ' '--compression='"
			elif [[ " ${COMP_WORDS[@]} " =~ " tiff " ]]; then
				single_dash="-h -a -g"
				double_dash="'--help ' '--afactor=' '--gamma='"
			elif [[ " ${COMP_WORDS[@]} " =~ " webp " ]]; then
				single_dash="-h -a -g -q"
				double_dash="'--help ' '--afactor=' '--gamma=' '--quality='"
			else
				single_dash="-h -a -g"
				double_dash="'--help ' '--afactor=' '--gamma='"
				no_format=1
			fi

			# Set autocompletion depending on the kind of argument being completed
			case $cur in
			--*)	# Double dash
				COMPREPLY=($(compgen -W "${double_dash}" -- $cur))
				compopt -o nospace
				;;
			-*)	# Single dash
				COMPREPLY=($(compgen -W "${single_dash}" -- $cur))
				;;
			*)	# Generic argument
				if [[ $no_format -eq 1 ]]; then	# Format not selected yet: autocomplete formats
					COMPREPLY=($(compgen -W "bmp gif jpeg png tiff webp" -- $cur))
				else	# Format already selected: autocomplete filenames
					COMPREPLY=($(compgen -A file -- $cur))
				fi
				;;
			esac
			;;

		"demo")
			# The argument after width, height or angleDeg does not require autocompletion because it is a number specified by the user
			if [[ "${prev}" == "-w" || "${cur}" == "--width=*" || \
				"${cur}" == "--height=*" || "${cur}" == "--angleDeg=*" ]]; then
				return 0
			fi

			if [[ $prev == "-o" || $cur == "--outfile=*" ]]; then	# Complete filenames
				COMPREPLY=($(compgen -A file -- $cur))
			elif [[ $prev == "-p" ]]; then	# Complete projections (-p)
				COMPREPLY=($(compgen -W "perspective orthogonal" -- $cur))
			elif [[ "${cur}" == --projection=* ]]; then	# Complete projections (--projection)
				COMPREPLY=($(compgen -W "--projection=perspective --projection=orthogonal" -- $cur))
			elif [[ "${cur}" == --* ]]; then	# Complete double dash arguments
				COMPREPLY=($(compgen -W '"--help " --width= --height= --projection= --angleDeg= --outfile=' -- $cur))
				compopt -o nospace
			elif [[ "${cur}" == -* ]]; then	# Complete single dash arguments
				COMPREPLY=($(compgen -W "-w -h -p -o" -- $cur))
			fi	# Demo does not have positional arguments to autocomplete
			;;
		esac
	fi
	return 0
}

# Use the function _complete_image_renderer to provide autocompletion for iamge-renderer
complete -F _complete_image_renderer image-renderer
