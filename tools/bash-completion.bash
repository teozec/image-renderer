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
			COMPREPLY=($(compgen -W "demo pfm2ldr stack render" -- $cur))
			;;
		esac

	# Otherwise, the completion depends on the action
	else
		local action prevprev
		action=${COMP_WORDS[1]}
		prevprev=${COMP_WORDS[COMP_CWORD-2]}
		case "$action" in
		"pfm2ldr")
			# The argument after afactor, gamma, quality, compression or luminosity does not require autocompletion because it is a number specified by the user
			if [[ "${prev}" == "-a" || "${prev}" == "-g" || "${prev}" == "-q" || "${prev}" == "-c" || "${prev}" == "-l" ||  \
				("${prev}" == "--afactor" && "${cur}" == "=") || \
				("${prevprev}" == "--afactor" && "${prev}" == "=") || \
				("${prev}" == "--gamma" && "${cur}" == "=") || \
				("${prevprev}" == "--gamma" && "${prev}" == "=") || \
				("${prev}" == "--quality" && "${cur}" == "=") || \
				("${prevprev}" == "--quality" && "${prev}" == "=") || \
				("${prev}" == "--compression" && "${cur}" == "=") || \
				("${prevprev}" == "--compression" && "${prev}" == "=") || \
				("${prev}" == "--luminosity" && "${cur}" == "=") || \
				("${prevprev}" == "--luminosity" && "${prev}" == "=") ]]; then
				return 0
			fi

			# Complete filenames
			if [[ $prev == "-o" || ("${prevprev}" == "--outfile" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -A file -- $cur))
			elif [[ "${prev}" == "--outfile" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -A file))

			# Complete formats
			elif [[ $prev == "-f" || ("${prevprev}" == "--format" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -W "bmp gif jpeg png tiff webp" -- $cur))
			elif [[ "${prev}" == "--format" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -W "bmp gif jpeg png tiff webp"))

			# Complete double dash arguments
			elif [[ "${cur}" == --* ]]; then
				COMPREPLY=($(compgen -W "--help --format= --outfile= --luminosity= --afactor= --gamma= --quality= --compression= --palette" -- $cur))
				# Remove space if there is a "=" in completion
				if [[ "${COMPREPLY[@]}" =~ "=" ]]; then
					compopt -o nospace
				fi

			# Complete single dash arguments
			elif [[ "${cur}" == -* ]]; then
				COMPREPLY=($(compgen -W "-h -f -o -l -a -g -q -c -p" -- $cur))

			# Complete input filename
			else
				COMPREPLY=($(compgen -A file -- $cur))
			fi
			;;

		"demo")
			# The argument after width, height, aspectRatio, angleDeg, seed, antialiasing, nRays, depth or roulette does not require autocompletion because it is a number specified by the user
			if [[ "${prev}" == "-w" || "${prev}" == "-a" || "${prev}" == "-D" || "${prev}" == "-A" || \
				"${prev}" == "-s" || "${prev}" == "-n" || "${prev}" == "-d" || "${prev}" == "-r" || \
				("${prev}" == "--width" && "${cur}" == "=") || \
				("${prevprev}" == "--width" && "${prev}" == "=") || \
				("${prev}" == "--height" && "${cur}" == "=") || \
				("${prevprev}" == "--height" && "${prev}" == "=") || \
				("${prev}" == "--aspectRatio" && "${cur}" == "=") || \
				("${prevprev}" == "--aspectRatio" && "${prev}" == "=") || \
				("${prev}" == "--antialiasing" && "${cur}" == "=") || \
				("${prevprev}" == "--antialiasing" && "${prev}" == "=") || \
				("${prev}" == "--seed" && "${cur}" == "=") || \
				("${prevprev}" == "--seed" && "${prev}" == "=") || \
				("${prev}" == "--nRays" && "${cur}" == "=") || \
				("${prevprev}" == "--nRays" && "${prev}" == "=") || \
				("${prev}" == "--depth" && "${cur}" == "=") || \
				("${prevprev}" == "--depth" && "${prev}" == "=") || \
				("${prev}" == "--roulette" && "${cur}" == "=") || \
				("${prevprev}" == "--roulette" && "${prev}" == "=") || \
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

			# Complete renderers
			elif [[ $prev == "-R" || ("${prevprev}" == "--renderer" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -W "path debug onoff flat" -- $cur))
			elif [[ "${prev}" == "--renderer" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -W "path debug onoff flat"))

			# Complete double dash arguments
			elif [[ "${cur}" == --* ]]; then
				COMPREPLY=($(compgen -W "--help --width= --height= --aspectRatio= --projection= --angleDeg= --seed= --antialiasing= --renderer= --outfile= --nRays= --depth= --roulette=" -- $cur))
				# Remove space if there is a "=" in completion
				if [[ "${COMPREPLY[@]}" =~ "=" ]]; then
					compopt -o nospace
				fi

			# Complete single dash arguments
			elif [[ "${cur}" == -* ]]; then
				COMPREPLY=($(compgen -W "-w -h -a -p -D -s -A -R -o -n -d -r" -- $cur))
			fi

			# Demo does not have positional arguments to autocomplete
			;;

		"render")
			# The argument after width, height, aspectRatio, angleDeg, seed, antialiasing, nRays, depth or roulette does not require autocompletion because it is a number specified by the user
			if [[ "${prev}" == "-w" || "${prev}" == "-a" || "${prev}" == "-A" || \
				"${prev}" == "-s" || "${prev}" == "-n" || "${prev}" == "-d" || "${prev}" == "-r" || \
				("${prev}" == "--width" && "${cur}" == "=") || \
				("${prevprev}" == "--width" && "${prev}" == "=") || \
				("${prev}" == "--height" && "${cur}" == "=") || \
				("${prevprev}" == "--height" && "${prev}" == "=") || \
				("${prev}" == "--aspectRatio" && "${cur}" == "=") || \
				("${prevprev}" == "--aspectRatio" && "${prev}" == "=") || \
				("${prev}" == "--antialiasing" && "${cur}" == "=") || \
				("${prevprev}" == "--antialiasing" && "${prev}" == "=") || \
				("${prev}" == "--seed" && "${cur}" == "=") || \
				("${prevprev}" == "--seed" && "${prev}" == "=") || \
				("${prev}" == "--nRays" && "${cur}" == "=") || \
				("${prevprev}" == "--nRays" && "${prev}" == "=") || \
				("${prev}" == "--depth" && "${cur}" == "=") || \
				("${prevprev}" == "--depth" && "${prev}" == "=") || \
				("${prev}" == "--roulette" && "${cur}" == "=") || \
				("${prevprev}" == "--roulette" && "${prev}" == "=") ]]; then
				return 0
			fi

			# Complete filenames
			if [[ $prev == "-o" || ("${prevprev}" == "--outfile" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -A file -- $cur))
			elif [[ "${prev}" == "--outfile" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -A file))

			# Complete renderers
			elif [[ $prev == "-R" || ("${prevprev}" == "--renderer" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -W "path debug onoff flat" -- $cur))
			elif [[ "${prev}" == "--renderer" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -W "path debug onoff flat"))

			# Complete double dash arguments
			elif [[ "${cur}" == --* ]]; then
				COMPREPLY=($(compgen -W "--help --width= --height= --aspectRatio= --seed= --antialiasing= --renderer= --outfile= --nRays= --depth= --roulette=" -- $cur))
				# Remove space if there is a "=" in completion
				if [[ "${COMPREPLY[@]}" =~ "=" ]]; then
					compopt -o nospace
				fi

			# Complete single dash arguments
			elif [[ "${cur}" == -* ]]; then
				COMPREPLY=($(compgen -W "-w -h -a -s -A -R -o -n -d -r" -- $cur))
			fi

			# Demo does not have positional arguments to autocomplete
			;;


		"stack")
			# The argument after nSigma or alpha does not require autocompletion because it is a number specified by the user
			if [[ "${prev}" == "-S" || "${prev}" == "-a" || \
				("${prev}" == "--nSigma" && "${cur}" == "=") || \
				("${prevprev}" == "--nSigma" && "${prev}" == "=") || \
				("${prev}" == "--alpha" && "${cur}" == "=") || \
				("${prevprev}" == "--alpha" && "${prev}" == "=") ]]; then
				return 0
			fi

			# Complete filenames
			if [[ $prev == "-o" || ("${prevprev}" == "--outfile" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -A file -- $cur))
			elif [[ "${prev}" == "--outfile" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -A file))

			# Complete methods
			elif [[ $prev == "-m" || ("${prevprev}" == "--method" && "${prev}" == "=") ]]; then
				COMPREPLY=($(compgen -W "mean median" -- $cur))
			elif [[ "${prev}" == "--method" && "${cur}" == "=" ]]; then
				COMPREPLY=($(compgen -W "mean median"))

			# Complete double dash arguments
			elif [[ "${cur}" == --* ]]; then
				COMPREPLY=($(compgen -W "--help --method= --nSigma= --alpha= --outfile=" -- $cur))
				# Remove space if there is a "=" in completion
				if [[ "${COMPREPLY[@]}" =~ "=" ]]; then
					compopt -o nospace
				fi

			# Complete single dash arguments
			elif [[ "${cur}" == -* ]]; then
				COMPREPLY=($(compgen -W "-h -m -S -a -o" -- $cur))

			# Complete input filename
			else
				COMPREPLY=($(compgen -A file -- $cur))
			fi
			;;
		esac
	fi
	return 0
} &&
complete -F _complete_image_renderer image-renderer
