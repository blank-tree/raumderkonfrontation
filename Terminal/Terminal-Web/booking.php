<?php
require_once('settings.php');


/* Update DB*/
$conn = new mysqli(DB_HOST, DB_USER, DB_PW, DB_NAME);
if ($conn->connect_error) {
	die("Connection failed: " . $conn->connect_error);
}

$date = $_GET['date'];
$time = $_GET['time'];
$nextHour = $time < MAX_TIME ? $time + 1 : 0;
$hourBefore = $time - 1 >= MIN_TIME ? $time - 1 : 0;
$success = false;


$sql = "UPDATE " . DB_TABLE . " SET taken = TRUE, timestamp = " . time() . " WHERE date=" . $date .
	" AND time=" . $time;
if ($conn->query($sql) == true) {
	if ($nextHour != 0) {
		$sql = "UPDATE " . DB_TABLE . " SET taken = TRUE, timestamp = " . time() . " WHERE date=" . $date .
			" AND time=" . $nextHour;
		$conn->query($sql);
	}
	if ($hourBefore != 0) {
		$sql = "UPDATE " . DB_TABLE . " SET taken = TRUE, timestamp = " . time() . " WHERE date=" . $date .
			" AND time=" . $hourBefore;
		$conn->query($sql);
	}
} else {
	echo "Error updating record: " . $conn->error;
}

$conn->close();

/* Send E-Mail */

$message = "NEW BOOKING: " . $date . ".05.2018 at " . $time . ":00";

mail(MAIL_ADDRESS, "NEW BOOKING", $message);


header("Location: http://" . $_SERVER['SERVER_NAME']);
exit();