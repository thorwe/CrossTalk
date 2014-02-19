<?php
/**
 * color.inc.php
 * created: 05.06.13
 */

/*
 * TODO
 */

/**
 * An approach to get the correct RGB values from the GW2 color API as Cliff Spradlin described over here:
 * @link https://forum-en.guildwars2.com/forum/community/api/How-To-Colors-API/2148826
 *
 * The described function was split up in 2 functions to improve performance within long run loops e.g. image processing
 * Suggested by Dr Ishmael over here:
 * @link https://forum-en.guildwars2.com/forum/community/api/API-Suggestion-Guilds/2155578
 *
 * @param array $hslbc basically the content of the arrays [cloth,leather,metal] which are returned by the API
 *
 * @return array matrix for calculation in apply_color_transform()
 */
function get_color_matrix($hslbc){
	$h = ($hslbc['hue']*pi())/180;
	$s = $hslbc['saturation'];
	$l = $hslbc['lightness'];
	$b = $hslbc['brightness']/128;
	$c = $hslbc['contrast'];

	// 4x4 identity matrix
	$matrix = [
		[1, 0, 0, 0],
		[0, 1, 0, 0],
		[0, 0, 1, 0],
		[0, 0, 0, 1]
	];

	if($b != 0 || $c != 1){
		// process brightness and contrast
		$t = 128*(2*$b+1-$c);
		$mult = [
			[$c,  0,  0, $t],
			[ 0, $c,  0, $t],
			[ 0,  0, $c, $t],
			[ 0,  0,  0,  1]
		];
		$matrix = matrix_multiply($mult, $matrix);
	}

	if($h != 0 || $s != 1 || $l != 1){
		// transform to HSL
		$multRgbToHsl = [
			[ 0.707107, 0,        -0.707107, 0],
			[-0.408248, 0.816497, -0.408248, 0],
			[ 0.577350, 0.577350,  0.577350, 0],
			[ 0,        0,         0,        1]
		];
		$matrix = matrix_multiply($multRgbToHsl, $matrix);

		// process adjustments
		$cosHue = cos($h);
		$sinHue = sin($h);
		$mult = [
			[ $cosHue * $s, $sinHue * $s,  0, 0],
			[-$sinHue * $s, $cosHue * $s,  0, 0],
			[            0,            0, $l, 0],
			[            0,            0,  0, 1]
		];
		$matrix = matrix_multiply($mult, $matrix);

		// transform back to RGB
		$multHslToRgb = [
			[ 0.707107, -0.408248, 0.577350, 0],
			[ 0,         0.816497, 0.577350, 0],
			[-0.707107, -0.408248, 0.577350, 0],
			[ 0,         0,        0,        1]
		];
		$matrix = matrix_multiply($multHslToRgb, $matrix);
	}

	return $matrix;
}

/**
 * @param array $matrix the matrix returned by get_color_matrix()
 * @param array $base the base color provided in the API response, or calculated for the emblem colors
 *
 * @return array calculated RGB values
 */
function apply_color_transform($matrix, $base){
	// apply the color transformation
	$bgrVector = [
		[$base[2]],
		[$base[1]],
		[$base[0]],
		[1]
	];
	$matrix =  matrix_multiply($matrix, $bgrVector);

	// clamp the values
	$rgb = [
		floor(max(0, min(255, $matrix[2][0]))),
		floor(max(0, min(255, $matrix[1][0]))),
		floor(max(0, min(255, $matrix[0][0])))
	];

	return $rgb;
}

/**
 * @author Moturdrn.2837
 * @link https://gist.github.com/moturdrn/9d03a0cd4967828ac6cc
 *
 * @param resource $im image
 * @param array $col the [material] array from the color API response
 */
function imagehue($im, $col){
	$w = imagesx($im);
	$h = imagesy($im);
	$m = get_color_matrix($col);
	for($x = 0; $x < $w; $x++){
		for($y = 0; $y < $h; $y++){
			$ci = imagecolorsforindex($im, imagecolorat($im, $x, $y));
			if($ci['alpha'] < 127){
				$rgb = apply_color_transform($m, [$ci['red'],$ci['green'],$ci['blue']]);
#				$rgb = color_shift($m, [$ci['red'],$ci['green'],$ci['blue']]);
				imagesetpixel($im, $x, $y, imagecolorallocatealpha($im, $rgb[0], $rgb[1], $rgb[2], $ci['alpha']));
			}
		}
	}
}


/**
 * First approach to calculate RGB values from the given HSL like Cliff Spradlin described over here:
 * @link https://forum-en.guildwars2.com/forum/community/api/API-Suggestion-Colors/2141987
 * There is still a problem applying brightness and contrast it seems. The returned colors are correct in tone but too bright.
 *
 * @param array $hslbc basically the content of the arrays [cloth,leather,metal] which are returned by the API
 * @param array $base the base color, also provided in the API response, does not apply to emblem colors
 *
 * @return array calculated RGB values
 */
function color_shift($hslbc, $base){
	$h = $hslbc['hue'];
	$s = $hslbc['saturation'];
	$l = $hslbc['lightness'];
	$b = $hslbc['brightness'];
	$c = $hslbc['contrast'];

	//apply brightness and contrast to the base RGB and clamp to 0-255
	foreach($base as $k => $v){
		$base[$k] = floor(max(0, min(255, (($v+$b)-128)*$c+128)));
	}

	//convert to HSL
	$hsl = rgb2hsl($base);

	//apply shifts
	$hsl[0] = ($hsl[0]*360+$h)/360;
	$hsl[1] = $hsl[1]*$s;
	$hsl[2] = $hsl[2]*$l;

	//convert back to RGB
	$rgb = hsl2rgb($hsl);

	//clamp RGB values
	foreach($rgb as $k => $v){
		$rgb[$k] = floor(max(0, min(255, $v)));
	}
	return $rgb;
}


/**
 * rgb2hsl adapted from
 * @link https://github.com/mjijackson/mjijackson.github.com/blob/master/2008/02/rgb-to-hsl-and-rgb-to-hsv-color-model-conversion-algorithms-in-javascript.txt
 *
 * @param array $rgb
 *
 * @return array HSL
 */
function rgb2hsl($rgb){
	$r = $rgb[0]/255;
	$g = $rgb[1]/255;
	$b = $rgb[2]/255;

	$min = min($r, $g, $b);
	$max = max($r, $g, $b);
	$d = $max-$min;

	$h = $s = 0;
	$l = ($max+$min)/2;

	if($d > 0){
		$s = $l > 0.5 ? $d/(2-$max-$min) : $d/($max+$min);
		switch($max){
			case $r: $h = ($g-$b)/$d+($g < $b ? 6 : 0); break;
			case $g: $h = ($b-$r)/$d+2; break;
			case $b: $h = ($r-$g)/$d+4; break;
		}
		$h /= 6;
	}

	return [$h, $s, $l];
}


/**
 * hsl2rgb adapted from
 * @link https://github.com/mjijackson/mjijackson.github.com/blob/master/2008/02/rgb-to-hsl-and-rgb-to-hsv-color-model-conversion-algorithms-in-javascript.txt
 *
 * @param array $hsl
 *
 * @return array RGB
 */
function hsl2rgb($hsl){
	$h = $hsl[0];
	$s = $hsl[1];
	$l = $hsl[2];

	$r = $g = $b = $l;
	if($s <> 0){
		if(!function_exists('hue2rgb')){
			function hue2rgb($p, $q, $t){
				if($t < 0) $t += 1;
				if($t > 1) $t -= 1;
				if($t < 1/6) return $p+($q-$p)*6*$t;
				if($t < 1/2) return $q;
				if($t < 2/3) return $p+($q-$p)*(2/3-$t)*6;

				return $p;
			}
		}
		$q = $l < 0.5 ? $l*(1+$s) : $l+$s-$l*$s;
		$p = 2*$l-$q;
		$r = hue2rgb($p, $q, $h+1/3);
		$g = hue2rgb($p, $q, $h);
		$b = hue2rgb($p, $q, $h-1/3);
	}

	return [$r*255, $g*255, $b*255];
}

/**
 * @param $rgb
 *
 * @return array
 */
function rgb2hsv($rgb){
	$r = $rgb[0]/255;
	$g = $rgb[1]/255;
	$b = $rgb[2]/255;

	$min = min($r, $g, $b);
	$max = max($r, $g, $b);

	switch($max){
		case 0:
			$h = $s = $v = 0;
			break;
		case $min:
			$h = $s = 0;
			$v = $max;
			break;
		default:
			$delta = $max-$min;
			if($r == $max){
				$h = 0+($g-$b)/$delta;
			}
			elseif($g == $max){
				$h = 2+($b-$r)/$delta;
			}
			else{
				$h = 4+($r-$g)/$delta;
			}
			$h *= 60;
			if($h < 0){
				$h += 360;
			}
			$s = $delta/$max;
			$v = $max;
	}
	return [$h, $s, $v];
}

/**
 * @param $hsv
 *
 * @return array
 */
function hsv2rgb($hsv){
	$h = $hsv[0];
	$s = $hsv[1];
	$v = $hsv[2];

	if($s == 0){
		$r = $g = $b = $v;
	}
	else{
		$h /= 60;
		$hi = floor($h);
		$f = $h-$hi;
		$p = ($v*(1-$s));
		$q = ($v*(1-($f*$s)));
		$t = ($v*(1-((1-$f)*$s)));

		switch($hi){
			case 0:
				$r = $v;
				$g = $t;
				$b = $p;
				break;
			case 1:
				$r = $q;
				$g = $v;
				$b = $p;
				break;
			case 2:
				$r = $p;
				$g = $v;
				$b = $t;
				break;
			case 3:
				$r = $p;
				$g = $q;
				$b = $v;
				break;
			case 4:
				$r = $t;
				$g = $p;
				$b = $v;
				break;
			default:
				$r = $v;
				$g = $p;
				$b = $q;
				break;
		}
	}

	return [intval($r*255+0.5),	intval($g*255+0.5),	intval($b*255+0.5)];
}


/**
 * @param array $rgb
 *
 * @return string
 */
function rgb2hex($rgb) {
	return sprintf('%02X%02X%02X', $rgb[0], $rgb[1], $rgb[2]);
}

/**
 * @param string $hex
 *
 * @return array
 */
function hex2rgb($hex) {
	$int = hexdec(str_replace('#','',$hex));
	return [0xFF&($int>>0x10), 0xFF&($int>>0x8), 0xFF&$int];
}

?>