// preload() and changeImage() function

// Copyright (c) 1999 Robert Rainwater

function preload(obj, src) {
	if (document.images) {
		eval(obj + ' = new Image()')
		eval(obj + '.src = "' + src + '"')
	}
}

function changeImage(src,obj) {
	if (document.images) {
		document.images[src].src = eval(obj+".src")
	}
}