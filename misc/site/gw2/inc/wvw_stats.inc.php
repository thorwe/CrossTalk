<?php
/**
 * wvw_stats.inc.php
 * created: 05.06.13
 *
 * caution, nasty stuff in here! :o
 */

/*
 * TODO
 */

/**
 * @param array $data         the matchdata received from the API
 *
 * the following parameters are obsolete if you're working with a DB so that you can pull this info within the function
 * @param array $match_worlds the worlds in the current match in order [red, blue, green]
 * @param array $worldnames   an array of the world names in the desired language [world_id -> name, ...]
 *
 * @return array
 */
function parse_stats($data, $match_worlds, $worldnames){
	$out = ['match_id' => $data['match_id']];
	//get world info - you may pull this and additional info from a DB
	foreach(['red', 'blue', 'green'] as $k => $v){
		$out['worlds'][$v] = [
			'id' => $match_worlds[$k],
			'name' => $worldnames[$match_worlds[$k]]
		];
	}
	//map scores
	foreach($data['maps'] as $map){
		$out = parse_mapstats($map, $out);
	}
	return $out;
}


/**
 * @param array  $map the maps part of the API response
 * @param array  $out the output array given from parse_stats()
 *
 * @return array
 */
function parse_mapstats($map, $out){
	$t =  $map['type'];
	// objective data may be pulled from a DB
	$objective_data = file_get_contents('../json/gw2_objectives.json');
	$objective_data = json_decode($objective_data, 1);
	foreach($objective_data as $obj){
		//get basic objective data
		$objectives[$obj['objective_id']] = [
			'id'    => $obj['objective_id'],
			'score' => $obj['score'],
			'name'  => $obj['name_en'],
			'type'  => $obj['type']
		];
	}
	// loop the stats for each world out
	foreach(['red', 'blue', 'green'] as $k => $v){
		//set the objective counts
		$out['worlds'][$v]['scores'][$t] = [
			'score' => $map['scores'][$k],
			'income' => 0,
			'camp' => 0,
			'tower' => 0,
			'keep' => 0,
			'castle' => 0
		];
		// loop the objectives for the given map out
		foreach($map['objectives'] as $o){
			if(strtolower($o['owner']) == $v){
				// set guild and owner
				$objectives[$o['id']]['guild'] = isset($o['owner_guild']) ? $o['owner_guild'] : '';
				$objectives[$o['id']]['owner'] = isset($v) ? $v : 'neutral';
				//add the current objective to the map data
				$out['objectives'][$t][] = $objectives[$o['id']];
				// add the score of that objective to the map income
				$out['worlds'][$v]['scores'][$t]['income'] += $objectives[$o['id']]['score'];
				// count up the objective type
				switch($objectives[$o['id']]['score']){
					case 5 : $out['worlds'][$v]['scores'][$t]['camp']++; break;
					case 10: $out['worlds'][$v]['scores'][$t]['tower']++; break;
					case 25: $out['worlds'][$v]['scores'][$t]['keep']++; break;
					case 35: $out['worlds'][$v]['scores'][$t]['castle']++; break;
				}
			}
		}
		// add the income and type to the total scores
		$out['worlds'][$v]['scores']['total']['score'] += $out['worlds'][$v]['scores'][$t]['score'];
		$out['worlds'][$v]['scores']['total']['income'] += $out['worlds'][$v]['scores'][$t]['income'];
		$out['worlds'][$v]['scores']['total']['camp'] += $out['worlds'][$v]['scores'][$t]['camp'];
		$out['worlds'][$v]['scores']['total']['tower'] += $out['worlds'][$v]['scores'][$t]['tower'];
		$out['worlds'][$v]['scores']['total']['keep'] += $out['worlds'][$v]['scores'][$t]['keep'];
		$out['worlds'][$v]['scores']['total']['castle'] += $out['worlds'][$v]['scores'][$t]['castle'];
	}
	return $out;
}

?>