<?php
// Syntaxe du connect :
// serverconnect=infos		: un client devient serveur
//		params:  * ServerIP		: IP du nouveur serveur
//				 * ServerName	: ID du nouveau serveur
// serverdisconnect=1		: le server actuel se deconnecte	
// serverinfos=1			: un client souhaite connaitre l'@ du server

// TEST
//$ServerIP = "111.222.333.444";
//$ServerName = "Yo";

//Ouvre le fichier users.dbf en lecture-écriture (2)
$db = dbase_open("zgnetserver.dbf", 2);

// Récupère son adresse IP 
$host_ip = getenv("REMOTE_ADDR");

// Nom de la machine
$host_name = gethostbyaddr($ip); 

// Récupère la langue du browser
$host_lang = getenv( "HTTP_ACCEPT_LANGUAGE");

function connect2server ($db, $ip, $name, $version = 1.0)
{	// modifie les information de la table server
	
	// Recupere la 1ere (et unique) ligne de la table
	$rec = dbase_get_record_with_names($db, 1);
	$newEnregistrement = array( $ip , $name , "1.0", time());
	dbase_replace_record ($db, $newEnregistrement, 1);
	echo "[ServerNew]\n";
	echo "ServerStatus=OK\n";
};

// Recupere sur la 1ere ligne de la table "Server" les infos utiles a l'utilisateur
// afin qu'il puisse se connecter au serveur ou en deduire qu'il n'y en a pas
// et qu'il peut alors prendre sa place
function sendserverinfos ($db)
{	$nbrecords = dbase_numrecords ($db);
	if ($nbrecords==0)
	{	initTable($db);
	}

	//echo "serverinfos [enter]";
	$rec = dbase_get_record_with_names($db, 1);
	//echo "serverinfos [get_record]";
	$ServerIP 		= chop( $rec[ServerIP]);
	$ServerName 	= chop( $rec[ServerName]);
	$ServerVersion	= chop( $rec[ServerVers]);
	$ServerDate		= chop( $rec[ServerDate]);
	echo "[ServerInfos]\n";
	echo "ServerIP=$ServerIP\n";
	echo "ServerName=$ServerName\n";
	echo "ServerVersion=$ServerVersion\n";
	echo "ServerDate=$ServerDate\n";
	//echo "serverinfos [exit]";
};


// Intialise la table avec les donnees par defaut
function initTable($db)
{	$nbrecords = dbase_numrecords ($db);
	while ($nbrecords>1)
	{	dbase_delete_record ($db, 1);
		$nbrecords = dbase_numrecords ($db);
		dbase_pack($db);
	}
	// Rajoute la ligne par defaut
	$enregistrement = array( "???.???.???.???" , "<unknown>" , "0.0", time() );
	//echo "<i>host_ip</i> : <b>$host_ip</b><br>";
	dbase_add_record ($db, $enregistrement);
}


//[debug] echo "La valeur de <i>'serverconnect'</i> est : <b>$serverconnect</b><br>";
if ($serverconnect == 1)
{  	//[debug] echo "La valeur de <i>'serverIP'</i> est : <b>$serverIP</b><br>";
	connect2server($db, $ServerIP, $ServerName);
} elseif ($serverconnect == 2)
{	// Intialise la table avec les donnees par defaut
	initTable($db);
}
else if ($serverinfos == 1)
{	sendserverinfos($db);
}
else if ($serverdisconnect == 1)
{	initTable($db);
}

dbase_close ($db);
?>