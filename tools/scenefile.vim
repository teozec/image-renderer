" Vim syntax file
" Language: image-renderer scenefile

if exists("b:current_syntax")
	finish
endif

syn keyword sceneBase float camera material 
syn keyword sceneShape sphere plane triangle union difference intersection box
syn keyword sceneBrdf diffuse specular dielectric
syn keyword scenePigment uniform checkered image
syn keyword sceneTransformation identity translation rotation_x rotation_y rotation_z scaling
syn keyword sceneProjection orthogonal perspective
syn match sceneComment "#.*$"
syn region sceneString start='"' end='"'
syn match sceneNumber '\d\+'
syn match sceneNumber '[-+]\d\+'
syn match sceneNumber '\d\+\.\d*'
syn match sceneNumber '[-+]\d\+\.\d*'

let b:current_syntax = "scenefile"
hi def link sceneBase Statement
hi def link sceneShape Statement
hi def link sceneBrdf PreProc
hi def link scenePigment PreProc
hi def link sceneTransformation PreProc
hi def link sceneProjection PreProc
hi def link sceneComment Comment
hi def link sceneString Constant
hi def link sceneNumber Constant
