<?php
/**
 * math.inc.php
 * created: 05.06.13
 */

/*
 * TODO
 */

/**
 * adapted from
 * @link http://sickel.net/blogg/?p=907
 *
 * @param $m1
 * @param $m2
 *
 * @return array|bool
 */
function matrix_multiply($m1, $m2){
	$r = count($m1);
	$c = count($m2[0]);
	$p = count($m2);
	if(count($m1[0]) != $p){
		return false; //incompatible matrix
	}
	$m3 = [];
	for($i = 0; $i < $r; $i++){
		for($j = 0; $j < $c; $j++){
			$m3[$i][$j] = 0;
			for($k = 0; $k < $p; $k++){
				$m3[$i][$j] += $m1[$i][$k]*$m2[$k][$j];
			}
		}
	}
	return ($m3);
}

?>