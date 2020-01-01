<?php
// Syntaxe du connect :
// serverconnect=1		: un client devient serveur
//		params:  * ServerIP	: IP du nouveur serveur (pas obligatoire)
//			 * ServerName	: ID du nouveau serveur
// serverdisconnect=1 ou 2	: le server actuel se deconnecte (1 c'est le serveur lui-meme et 2 pour un autre ordi)
//		params:	 * dateCnx	: Date a laquel le serveur a ete cree
//			 * ServerName	: ID du nouveau serveur (peut etre vide)
// serverinfos=1		: un client souhaite connaitre l'@ du server
//
// clientconnect=1		: un client s'est connecte au serveur
//		params:	 * ClientIP	: IP du client
//			 * ClientName	: ID du client
// clientdisconnect=1		: deconnexion d'un client
//		params:	 * dateCnx	: Date a laquelle il s'etait connecte (permet de mettre a jour la ligne correspondante)

// Declaration de quelques variables globales
$host = "phpmyadmin.free.fr";
$user = "antoine.rey";
$pass = "foh35r9u";
$bd = "antoine.rey";
$zgnetserverTable= "zgnetserver";
$clientSessionTable= "clientsession";
$serverSessionTable= "serversession";

$host_ip = getenv("REMOTE_ADDR");		// Récupère son adresse IP 
$host_name = gethostbyaddr($host_ip); 	// Nom de la machine
$host_lang = getenv( "HTTP_ACCEPT_LANGUAGE");	// Récupère la langue du browser


// Intialise la table avec les donnees par defaut : ???.???.???.??? - <unknown> - 0.0 - NOW()
function initTable($table)
{	// Si la table contient encore des lignes, on la vide
	clearTable($table);
	$dateCnx = date("Y-m-d H:i:s");
	// Rajoute la ligne par defaut
	$query = "INSERT INTO $table(ServerIP, ServerName, ServerVers, ServerDate) 
	 	  VALUES('???.???.???.???','<unknown>', '0.0', '$dateCnx')"; // NOW()
	$result = mysql_query($query); 
}

function clearTable($table)
{	$query = "SELECT * FROM $table";
	$result = mysql_query($query); 		
	$nbrecords = mysql_numrows($result); 
	if ($nbrecords > 0)
	{	$query = "DELETE FROM $table";
		$result = mysql_query($query);
	}
}

// Recupere sur la 1ere ligne de la table "zgnetserver" les infos utiles a l'utilisateur
// afin qu'il puisse se connecter au serveur ou en deduire qu'il n'y en a pas online en 
// ce moment et qu'il peut alors creer une session serveur
function sendserverinfos ($table)
{
	$query = "SELECT * FROM $table";	/* Selectionne la 1ere ligne */
	$result = mysql_query($query); 		
	$nbrecords = mysql_numrows($result); 	/* On recupère le nombre d'enregistrements */ 

	// Si la table est vide, on rajoute la ligne par defaut : ???.???.???.??? - <unknown> - 0.0 - 0/0/00
	if ($nbrecords<1)
	{	initTable($table);
		$query = "SELECT * FROM $table";
		$result = mysql_query($query);
	}

	$val = mysql_fetch_array($result);
	$ServerIP	= chop( $val["ServerIP"] ); 
	$ServerName	= chop( $val["ServerName"] ); 
  	$ServerVersion	= chop( $val["ServerVers"] );
	$ServerDate	= chop( $val["ServerDate"] );

	echo "[ServerInfos]\n";
	echo "ServerIP=$ServerIP\n";
	echo "ServerName=$ServerName\n";
	echo "ServerVersion=$ServerVersion\n";
	echo "ServerDate=$ServerDate\n";
};

// Un joueur cree un serveur
function initserver ($table, $ip, $machinename, $servername,  $version = 1.0)
{	clearTable($table);
	//$dateCnx = date("Y-m-d  H:i:s");
	$dateCnx = date("Y-m-d H:i:s");
	// echo date("d-m-Y H:i:s",time()+24*3600);
	// Rajoute la ligne par defaut
	$query = "INSERT INTO $table(ServerIP, ServerName, ServerVers, ServerDate) 
	 	  VALUES('$ip', '$servername', '$version', '$dateCnx')"; // NOW()
	$result = mysql_query($query); 

	$query = "SELECT * FROM $table";
	$result = mysql_query($query); 		
	$val = mysql_fetch_array($result);
	$dateCnx = chop($val["ServerDate"]);

	echo "[ServerNew]\n";
	echo "ServerStatus=OK\n";
	echo "ServerIP=$ip\n";
	echo "ServerCnx=$dateCnx\n";
};

// Conserve les logs de connexion des serveurs dans la table serversession
function logserveropensession($table, $ip, $name, $vers, $ipdomain)
{	//$dateCnx = date("Y-m-d  H:i:s");
	$query = "INSERT INTO $table(ServerIP, ServerName, ServerVers, SessionCnx, IPDomain) 
	 	  VALUES('$ip', '$name', '$vers', NOW(), '$ipdomain')";
	$result = mysql_query($query); 
};

//  Conserve les logs de deconnexion des serveurs dans la table serversession
function logserverclosesession($table, $ip, $name, $vers, $ipdomain, $dateCnx)
{	$dateDeCnx = date("Y-m-d H:i:s");

	// Date MySQL AAAA-MM-JJ HH:MM:SS
	$heureCnx = substr($dateCnx,11,2);
	$heureDeCnx = substr($dateDeCnx,11,2);
	$minCnx = substr($dateCnx,14,2);
	$minDeCnx = substr($dateDeCnx,14,2);	
	$secCnx = substr($dateCnx,17,2);
	$secDeCnx = substr($dateDeCnx,17,2);
	
	if (($heureDeCnx - $heureCnx) < 0)
	{	$heureCnx = $heureCnx-24;
	}
	if (($minDeCnx - $minCnx) < 0)
	{	$minCnx = $minCnx-60;
		$heureCnx -= 1;
	}
	if (($secDeCnx - $secCnx) < 0)
	{	$secCnx = $secCnx-60;
		$minCnx -= 1;
	}
	
	
	$difHeure = $heureDeCnx - $heureCnx;
	$difMin = $minDeCnx - $minCnx;
	$difSec = $secDeCnx - $secCnx;
	
	$ecart = "$difHeure:$difMin:$difSec";
	
	$query = "UPDATE '$table' SET SessionDeCnx='$dateDeCnx' AND SessionDuration='$ecart'
		  WHERE SessionCnx='$dateCnx'";
	$result = mysql_query($query);

	echo "[ServerDeCnx]\n";
	echo "ServerStatus=OK\n";
	echo "ServerDeCnx=$dateDeCnx \n";
	echo "ServerResult=$result\n";
};

function disconnectserver($table)
{	initTable($table);
};

// Un joueur se connecte au serveur multi-joueur
function connectclient($table, $ip, $machinename, $clientname)
{	//$dateCnx = time();	
	$dateCnx = date("Y-m-d H:i:s");
	// Rajoute la ligne par defaut
	$query = "INSERT INTO $table(ClientIP, SessionCnx, ClientName, IPDomain  ) 
	 	  VALUES('$ip', '$dateCnx', '$clientname', '$machinename')"; 
	$result = mysql_query($query); 

		
	echo "[ClientCnx]\n";
	echo "ClientStatus=OK\n";
	echo "ClientIP=$ip\n";
	echo "ClientCnx=$dateCnx\n";

	return dateCnx;
};

function disconnectclient($table, $ip, $date)
{	$dateDeCnx = time();	
	$ecart_heures = floor(($dateDeCnx - $date) / (60*24));
	// Met a jour la table des connexions

	$query = "UPDATE $table SET SessionDeCnx='$dateDeCnx' AND SessionDuration='$ecart_heures'
		  WHERE ClientIP='$ip'AND SessionCnx=$date";
	$result = mysql_query($query);
	echo "\nClientDeCnx=$dateDeCnx \n";
};


/* connection avec MySQL */
@mysql_connect($host, $user, $pass) or die("Impossible de se connecter à la base de données 1"); // Le @ ordonne a php de ne pas afficher de message d'erreur
@mysql_select_db("$bd") or die("Impossible de se connecter à la base de données 2");


// Le client cree un serveur multi-joueur
if ($serverconnect == 1)
{	// On prends l'IP de la machine serveur
	$ServerIP = $host_ip;
	initserver($zgnetserverTable, $ServerIP, $host_name, $ServerName);
	// Log la connexion dans la table serversession
	logserveropensession($serverSessionTable, $ServerIP, $ServerName, $ServerVers, $host_name);
} 
// Initilise la table avec des valeurs par defaut
elseif ($serverconnect == 2)
{	// Intialise la table avec les donnees par defaut
	initTable($zgnetserverTable);
}

// Renvoie des infos sur le serveur multi-joueur actuel 
elseif ($serverinfos == 1)
{	sendserverinfos($zgnetserverTable);
}
// Deconnexion du serveur en cours
elseif ($serverdisconnect > 0)
{	disconnectserver($zgnetserverTable);
	// Log la deconnexion dans la table serversession
	if ($serverdisconnect == 1)
	{  $ServerIP = $host_ip;
	} else
	{  $ServerIP = "???.???.???.???";
	}	
	logserverclosesession($serverSessionTable, $ServerIP, $ServerName, $ServerVers, $host_name, $dateCnx);
}

// Connexion d'un client au serveur multi-joueur de ZeGame
elseif ($clientconnect == 1)
{	$ClientIP = $host_ip;
	connectclient($clientSessionTable, $ClientIP, $host_name, $ClientName);
}
// Deconnexion d'un client
elseif ($clientdisconnect == 1)
{	$ClientIP = $host_ip;
	disconnectclient($clientSessionTable, $ClientIP, $dateCnx);
}


mysql_close();

?>