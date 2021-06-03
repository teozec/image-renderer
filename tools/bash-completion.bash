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
		local action prevprev
		action=${COMP_WORDS[1]}
		prevprev=${COMP_WORDS[COMP_CWORD-2]}
		case "$action" in
		"pfm2ldr")
			# The argument after afactor, gamma, quality or compression does not require autocompletion because it is a number specified by the user
			if [[ "${prev}" == "-a" || "${prev}" == "-g" || "${prev}" == "-q" || "${prev}" == "-c" || \
				("${prev}" == "--afactor" && "${cur}" == "=") || \
				("${prevprev}" == "--afactor" && "${prev}" == "=") || \
				("${prev}" == "--gamma" && "${cur}" == "=") || \
				("${prevprev}" == "--gamma" && "${prev}" == "=") || \
				("${prev}" == "--quality" && "${cur}" == "=") || \
				("${prevprev}" == "--quality" && "${prev}" == "=") || \
				("${prev}" == "--compression" && "${cur}" == "=") || \
				("${prevprev}" == "--compression" && "${prev}" == "=") ]]; then
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
				# Remove space if there is a '=' in completion
				if [[ "${COMPREPLY[@]}" =~ "=" ]]; then
					compopt -o nospace
				fi
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
			if [[ "${prev}" == "-w" || \
				("${prev}" == "--width" && "${cur}" == "=") || \
				("${prevprev}" == "--width" && "${prev}" == "=") || \
				("${prev}" == "--height" && "${cur}" == "=") || \
				("${prevprev}" == "--height" && "${prev}" == "=") || \
				("${prev}" == "--angleDeg" && "${cur}" == "=") || \
				("${prevprev}" == "--angleDeg" && "${prev}" == "=") ]]; then
				return 0
			fi

			# Complete filenames
			if [[ $prev == "-o" || ("${prevprev}" == "--outfile" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -A file -- $cur))
			elif [[ "${prev}" == "--outfile" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -A file))

			# Complete projections
			elif [[ $prev == "-p" || ("${prevprev}" == "--projection" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -W "perspective orthogonal" -- $cur))
			elif [[ "${prev}" == "--projection" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -W "perspective orthogonal"))

			# Complete double dash arguments
			elif [[ "${cur}" == --* ]]; then
				COMPREPLY=($(compgen -W '"--help " --width= --height= --projection= --angleDeg= --outfile=' -- $cur))
				# Remove space if there is a '=' in completion
				if [[ "${COMPREPLY[@]}" =~ "=" ]]; then
					compopt -o nospace
				fi

			# Complete single dash arguments
			elif [[ "${cur}" == -* ]]; then
				COMPREPLY=($(compgen -W "-w -h -p -o" -- $cur))
			fi

			# Demo does not have positional arguments to autocomplete
			;;
		esac
	fi
	return 0
} &&
complete -F _complete_image_renderer image-renderer
