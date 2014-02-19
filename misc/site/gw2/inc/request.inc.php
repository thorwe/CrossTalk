<?php
/**
 * request.inc.php
 * created: 05.06.13
 */


/**
 * A small function to request&receive data from the GW2 API.
 * Doesn't use CURL and doesn't depend on allow_url_fopen.
 *
 * @param string $request
 *
 * @return array|bool
 */
function gw2_api_request($request){
	$url = parse_url('https://api.guildwars2.com/v1/'.$request);
	// open the socket
	if(!$fp = @fsockopen('ssl://'.$url['host'], 443, $errno, $errstr, 5)){
		return false;
		// you may want improved error handling...
#		return 'connection error: '.$errno.', '.$errstr;
	}

	// prepare the request header...
	$nl = "\r\n";
	$header = 'GET '.$url['path'].(isset($url['query']) ? '?'.$url['query'] : '').' HTTP/1.1'.$nl.'Host: '.$url['host'].$nl.'Connection: Close'.$nl.$nl;

	// ...and send it.
	fwrite($fp, $header);
	stream_set_timeout($fp, 5);

	// receive the response
	$response = '';
	do{
		if(strlen($in = fread($fp,1024))==0){
			break;
		}
		$response.= $in;
	}
	while(true);

	// now the nasty stuff... explode the response at the newlines
	$response = explode($nl, $response);

	// you may want some advanced error handling over here, too
	if(isset($response[0]) && $response[0] == 'HTTP/1.1 200 OK'){
		// the response is non chunked, so we can assume the data is contained in the last line
		$response = json_decode($response[count($response)-1], true);
		return $response;
	}
	return false;
}

?>