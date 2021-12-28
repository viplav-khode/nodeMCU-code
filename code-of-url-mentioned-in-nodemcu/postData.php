<?php
//Creates new record as per request
    //Connect to database
    $server 	= "localhost";	// Change this to correspond with your database port
    $username 	= "id17819145_viplav";			// Change if use webhost online
    $password 	= "Cvpp@12345678910";
    $DB 		= "id17819145_smartsnib";			// database name


    // Create connection
    $conn = new mysqli($server, $username, $password, $DB);
    // Check connection
    if ($conn->connect_error) {
        die("Database Connection failed: " . $conn->connect_error);
    }


    //POST current date and time
    date_default_timezone_set('Asia/Kolkata');
    $d = date("Y-m-d");
    $t = date("H:i:s");


    if(isset($_GET['Temperature']) && isset($_GET['Humidity']))
    {
		
		  $sensorData1 = $_GET['Temperature'] ;
        $sensorData2 = $_GET['Humidity'] ;
        $lock_id = $_GET['LockId'] ;
        
		$query ="SELECT COUNT(Temperature) from $lock_id where Temperature=  '".$sensorData1."'" ;	
	
		$result = $conn->query($query);
	
		$count = mysqli_fetch_assoc($result);
		
	
	//	if($count["COUNT(Temperature)"] == 0){
			
			  $sql = "INSERT INTO $lock_id (Temperature, Humidity) VALUES ('".$sensorData1."', '".$sensorData2."')";
			if ($conn->query($sql) === TRUE) {
            echo "1";
          } 
		else {
            echo "Error: " . $sql . "<br>" . $conn->error;
          }
		
	//	}
    //    else{
	//		print($query) ;
	//		echo "\n The Count of".$sensorData1." = ".$count["COUNT(Temperature)"] ;
		  
	//	}
		
		
		
    }

    $conn->close();
?>