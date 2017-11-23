<?php
	$conn=new mysqli("127.0.0.1","root","As199165&","cf");
	if($conn->connect_errno)
	{
		echo "no";
	}
	else 
	{
		echo "yes";
	}

	$conn->close();
?>
