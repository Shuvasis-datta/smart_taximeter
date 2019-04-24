<!Doctype HTML>
<html>
<head>
	<title>Welcome to Smart Taxi Billing System</title>
</head>

<body>
<header>
<center><h1>Welcome to Smart Taxi Billing System</h1></cenrter><hr/>
</header>
<center style="min-height: 400px">
<?php 
	// Required Files
	require_once( 'functions.php' );

	$con = db_connection();
	
	/* Recieve & truncating unneccessary data from input */
	
	if( $_GET['pID'] ) $pID = trim(strip_tags($_GET['pID']));
	if( $_GET['dID'] ) $dID = trim(strip_tags($_GET['dID']));
	if( $_GET['distance'] ) $distance = trim(strip_tags($_GET['distance']));
	if( $_GET['jamTime'] ) $jamTime = trim(strip_tags($_GET['jamTime']));
	if( $_GET['request'] ) $request = trim(strip_tags($_GET['request']));

	/* Do as asked from input */

	if ($request == "checkBalance"){
		$msg = checkBalance( ($pID)? $pID: ($dID? $dID: -1), $con);
	}
	elseif($request == "payment"){
		$msg = payment($pID, $dID, getFair($distance, $jamTime), $con);
	}
	elseif($request == "getFair"){
		// When off
		$msg = getFair($distance, $jamTime);
	}
	else
	{
		$msg = "Bad Request";
	}
	
	echo $msg;
	
	/* Display All User Information */
	displayAllUserInfo($con);
?>	
</center>
<footer>
	<br/><hr/><center><p style="color:gray;">This project is done by Shuvasis Datta, Md. Shahab Uddin & Mir Rifat Siyam and supervised by Dr. Mohammad Rubaiyat Tanvir Hossain</p></center>
</footer>
</body>
</html>
