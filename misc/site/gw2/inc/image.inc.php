<?php
/**
 * image.inc.php
 * created: 06.06.13
 */

/*
 * TODO
 */

function image_error($size){
	global $cfg;
	header('Content-type: image/png');
	$img = imagecreatefrompng($cfg['error_image']);
	$thumb = imagecreatetruecolor($size, $size);
	imagecopyresampled($thumb, $img, 0, 0, 0, 0, $size, $size, 256, 256);
	imagepng($thumb);
	imagedestroy($thumb);
	exit();
}

/**
 * Based on the script by Moturdrn.2837
 * https://gist.github.com/moturdrn/9d03a0cd4967828ac6cc
 *
 * @param resource $img
 * @param string   $type
 *
 * @return resource
 */
function image_flip($img, $type = ''){
	$w = imagesx($img);
	$h = imagesy($img);
	$dest = imagecreatetruecolor($w, $h);
	imagesavealpha($dest, true);
	$trans_colour = imagecolorallocatealpha($dest, 0, 0, 0, 127);
	imagefill($dest, 0, 0, $trans_colour);
	switch($type){
		case 'v':
			for($i = 0; $i < $h; $i++){
				imagecopy($dest, $img, 0, ($h-$i-1), 0, $i, $w, 1);
			}
			break;
		case 'h':
			for($i = 0; $i < $w; $i++){
				imagecopy($dest, $img, ($w-$i-1), 0, $i, 0, 1, $h);
			}
			break;
		default:
			return $img;
	}
	return $dest;
}

?>