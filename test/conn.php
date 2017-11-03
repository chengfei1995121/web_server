<?php
	$conn=mysql_connect("localhost","root","As199165&");
	$c=mysql_select_db("cf",$conn);
	if($c>0)
	{
		echo "yes";
	}
	else 
	{
		echo "no";
	}
?>
