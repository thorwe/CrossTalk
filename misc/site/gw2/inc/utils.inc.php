<?php
/**
 * utils.inc.php
 * created: 07.09.13
 */

/**
 * Check if a string is really int
 * @kink http://php.net/manual/function.is-int.php#87670
 *
 * @param $val
 *
 * @return bool
 */
function check_int($val){
	return ($val !== true) && ((string)(int)$val) === ((string)$val);
}


/**
 * Get the chatcode for an item
 *
 * @param int $item_id
 *
 * @return string
 */
function item_code($item_id){
	return '[&'.base64_encode(chr(0x02).chr(0x01).chr($item_id%256).chr((int)($item_id/256)).chr(0x00).chr(0x00)).']';
}


/**
 * Creates pagination links for an array
 * (c) Smiley
 *
 * @param int    $total          total items
 * @param int    $start          start page
 * @param int    $limit          items per page
 * @param string $request        [optional] the page request, pagenumber needs last param like http://domain.tld/index.php?blah=blub&page=
 * @param int    $firstpage      [optional] you may want to begin counting at zero or something else...
 * @param int    $adjacents      [optional]
 * @param int    $adjacents_mid  [optional]
 * @param int    $adjacents_jump [optional]
 *
 * @return array $pages (pages,total,prev,next,currentpage,links,pagination)
 */
function pagination($total, $start, $limit, $request = null, $firstpage = 1, $adjacents = 1, $adjacents_mid = 1, $adjacents_jump = 1){
	$ext = '.html'; // leave this empty if you don't use url-rewriting
	$limit = $limit < 1 ? 1 : $limit; //prevent division by zero
	$pages = [
		'pages' => [],
		'total' => ceil($total/$limit),
		'prev' => '',
		'next' => '',
		'currentpage' => '',
		'links' => '',
		'pagination' => ''
	];
	$lastpage = $pages['total']+($firstpage-1);
	//if first page or less
	if($start <= $firstpage){
		$currentpage = $firstpage;
		$next = $firstpage+1;
		$prev = $lastpage;
	}
	//if last page or higher
	else if($start >= $lastpage){
		$currentpage = $start;
		$next = $firstpage;
		$prev = $lastpage-1;
	}
	//if page in between
	else{
		$currentpage = $start;
		$next = $start+1;
		$prev = $start-1;
	}
	//wanna have links? no problem :D
	//no pages - no links
	if($pages['total'] < 1){
		$pages['prev'] = '';
		$pages['next'] = '';
	}
	//one single page
	else if($pages['total'] == 1){
		$pages['prev'] = '<span class="p-links p-inactive">&#171; prev</span>';
		$pages['next'] = '<span class="p-links p-inactive">next &#187;</span>';
	}
	//more than one page^^
	else{
		$pages['prev'] = '<a class="p-links p-prevnext" href="'.($request === null ? '#' : $request.$prev.$ext).'" data-page="'.$prev.'">&#171; prev</a>';
		$pages['next'] = '<a class="p-links p-prevnext" href="'.($request === null ? '#' : $request.$next.$ext).'" data-page="'.$next.'">next &#187;</a>';
	}
	//loop the startpoints out...
	//$i=sql startline, $j=pagenumber/arr_key
	for($i = 0, $j = $firstpage; $i < $total; $i += $limit, $j++){
		//...and build some links in between
		$pages['pages'][$j] = $i;
		$href = ($request === null ? '#' : $request.$j.$ext);
		//current page
		if($j == $currentpage){
			$pages['links'] .= '<span class="p-links p-current">'.$j.'</span>';
			$pages['currentpage'] = $j;
		}
		//pages between start and current
		else if($j >= ($firstpage+$adjacents) && $j < ($currentpage-$adjacents_mid)){
			//jump-to links between start and current page
			if($j >= floor((($firstpage+$currentpage+$adjacents_mid)-$adjacents)/2)-$adjacents_jump && $j <= floor((($firstpage+$currentpage+$adjacents_mid)-$adjacents)/2)+$adjacents_jump){
				$pages['links'] .= '<a class="p-links p-middle" href="'.$href.'" data-page="'.$j.'">'.$j.'</a>';
			}
			//spaces in between - we can hide them by adding a nodisplay style ;)
#			else{
#				$pages['links'] .= '<a class="p-links p-middle hidden" href="'.$href.'" data-page="'.$j.'">'.$j.'</a>';
#			}
		}
		//pages between current and last
		else if($j > ($currentpage+$adjacents_mid) && $j <= ($lastpage-$adjacents)){
			//jump-to links between current and last page
			if($j >= ceil((($lastpage+$currentpage+$adjacents_mid)-$adjacents)/2)-$adjacents_jump && $j <= ceil((($lastpage+$currentpage+$adjacents_mid)-$adjacents)/2)+$adjacents_jump){
				$pages['links'] .= '<a class="p-links p-middle" href="'.$href.'" data-page="'.$j.'">'.$j.'</a>';
			}
			//spaces in between
#			else{
#				$pages['links'] .= '<a class="p-links p-middle hidden" href="'.$href.'" data-page="'.$j.'">'.$j.'</a>';
#			}
		}
		//first/last page & adjacents
		else{
			$pages['links'] .= '<a class="p-links" href="'.$href.'" data-page="'.$j.'">'.$j.'</a>';
		}
	}
	$pages['pagination'] = $pages['total'] > 1 ? '<div class="p-links-container">'.$pages['prev'].$pages['links'].$pages['next'].'</div>' : '';
	return $pages;
}



?>