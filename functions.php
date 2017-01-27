<?php

function db_connection(){

	define("DB_HOST", "localhost");
	define("DB_USER", "cuetrma_test");
	define("DB_PASSWORD", "85lxyogJi24M");
	define("DB_DATABASE", "cuetrma_smarttaximeter");

	 
	$conn = new mysqli( DB_HOST , DB_USER, DB_PASSWORD, DB_DATABASE );

	if( $conn->connect_error ){
		die("Could not connect : " . $conn->connect_error );
	}

	return $conn;
}

function checkBalance($userId, $con){
	$result = $con->query("Select Balance from user where ID={$userId}");
	if($result->num_rows > 0){
		$balance = $result->fetch_assoc()['Balance'];
		return "Balance : ". $balance;
	}
	else{
		return "ID is not found in database";
	}
}

function payment($passenger_id, $driver_id, $fair, $con){
	$result1 = $con->query("Select * from user where ID={$passenger_id}");
	$result2 = $con->query("Select * from user where ID={$driver_id}");
	

	if($result1 && $result2){
		$psgr = $result1->fetch_assoc();
		$drvr = $result2->fetch_assoc(); 
		
		if( $psgr['Balance'] > 0 ){
			$transaction = ($psgr['Balance'] >= $fair)? $fair: ($fair - $psgr['Balance']);

			$result1 = $con->query("Update user Set Balance = Balance - {$transaction}, LastTransaction = {$transaction} WHERE Id={$passenger_id}");
			$result2 = $con->query("Update user Set Balance = Balance + {$transaction}, LastTransaction = {$transaction} WHERE Id={$driver_id}");
			if($result1 && $result2){
				return "Amount ". $transaction . " is transferred to Driver. Your New Balance is ". ($psgr['Balance']-$transaction); 
			}
			else{
				return "Something Wrong in transaction";
			} 
		}
		else
		{
			return "You have no balance in your account";	
		} 
	}
	elseif(!$result1){
		return "Passenger ID is not found in database";
	}
	elseif(!$result1){
		return "Driver ID is not found in database";
	}
	else{
		return "Something Wrong with given ID";
	}
}

function getFair($distance, $jam_time){
/*
   * 1st 2km = 40 Tk
   * then 12 Tk per km  
   * 3 Tk per min of jam
   * Here every measurement in km.
*/
	$fixed_distance = 2;  /* Actually 2km */ 
	$extra_distance = (float)($distance - $fixed_distance);
	
	/* Fair */
  	$fixed_fair = 40.0;
  	$per_km_fair = 12.0;
  	$jam_fair = 0.0; /* actually (time * fair) */
  
  	if( $distance <= $fixed_distance ){
  		return (float)($fixed_fair + $jam_fair);
  	}
  	else{
    		return (float)($fixed_fair + $extra_distance * $per_km_fair + $jam_fair);
  	}
}


function displayAllUserInfo($con){
	$result = $con->query("SELECT * FROM user");
	
	if( $result->num_rows > 0 ){
		echo "<h3>All User Info Table</h3>";
		echo "<style> th, td{padding: 15px; text-align:center;} </style>";
		echo "<table border='1'>
				<tr>
					<th>ID</th>
					<th>Name</th>
					<th>Amount</th>
					<th>Last Transaction</th>
				</tr>
			";
		while( $row = $result->fetch_assoc() ){
			echo "<tr>
					<td>{$row['Id']}</td>
					<td>{$row['Name']}</td>
					<td>{$row['Balance']}</td>
					<td>{$row['LastTransaction']}</td>
				 </tr>
				";
		}
		echo "</table>";
	}

}

?> 