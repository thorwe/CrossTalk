<?php
/**
 * mysqli.inc.php
 * created: 07.07.13
 */

// mysql connection data
$mysql = [
	'server'   => 'server',
	'user'     => 'user',
	'password' => 'password',
	'dbname'   => 'dbname'
];

$db = mysqli_init();

// connect to the db
if(!mysqli_real_connect($db, $mysql['server'], $mysql['user'], $mysql['password'], $mysql['dbname'])){
	// note: you sould not expose sql errors to the public on a production system. never. ever.
	exit('Could not connect to the database: '.mysqli_connect_errno().' - '.mysqli_connect_error());
}

// set the connection dataset
if(!mysqli_set_charset($db, 'utf8')){
	exit('Error loading character set utf8: '.mysqli_error($db));
}

/**
 * MySQLi prepared statements wrapper
 *
 * @link https://www.owasp.org/index.php/PHP_Security_Cheat_Sheet#MySQLi_Prepared_Statements_Wrapper
 *
 * @param string $query
 * @param array  $values
 * @param bool   $assoc
 *
 * @return array|bool
 */
function sql_query($query, $values = [], $assoc = true){
	global $db;
	// no prepared statement, assumes clean/escaped SQL, not recommended for untrusted input
	if(!is_array($values) || count($values) === 0){
		// catch possible errors
		if(!$result = mysqli_query($db, $query)){
			return false;
		}
		// ok, we have a result with one or more rows, loop out the rows and output as array
		if(!is_bool($result) && mysqli_num_rows($result) > 0){
			$out = [];
			while($r = ($assoc === true) ? mysqli_fetch_assoc($result) : mysqli_fetch_row($result)){
				$out[] = $r;
			}
			mysqli_free_result($result);
			return $out;
		}
		// void result
		return true;
	}
	// prepared statement
	else{
		// catch possible errors
		if(!$stmt = mysqli_prepare($db, $query)){
			return false;
		}
		// copy values to reference for bind_param's sake
		// see: http://www.php.net/manual/en/mysqli-stmt.bind-param.php
		$references = [];
		foreach($values as $k => &$v){
			$references[$k] = & $v;
		}
		// just assume that all params are strings, works well on MySQL and SQLite
		$types = str_repeat('s', count($references));
		// add the types to the top of the $references array
		array_unshift($references, $types);
		// bind the params to $stmt and execute
		call_user_func_array([$stmt, 'bind_param'], $references);
		mysqli_stmt_execute($stmt);
		// any metadata in there?
		$metadata = mysqli_stmt_result_metadata($stmt);
		// void result
		if(!$metadata){
			return true;
		}
		// fetch all results as a 2D array
		$out = [];
		$fields = [];
		$count = 0;
		while($field = mysqli_fetch_field($metadata)){
			if($assoc === true){
				$fields[] = & $out[$field->name];
			}
			else{
				$fields[] = & $out[$count];
			}
			$count++;
		}
		call_user_func_array([$stmt, 'bind_result'], $fields);
		$output = [];
		$count = 0;
		while(mysqli_stmt_fetch($stmt)){
			foreach($out as $k => $v){
				$output[$count][$k] = $v;
			}
			$count++;
		}
		mysqli_stmt_free_result($stmt);
		return ($count === 0) ? true : $output;
	}
}

?>