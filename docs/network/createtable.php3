<?php //Programme � n'ex�cuter qu'une fois pour cr�er les tables de la base de donn�es dBase
  $def =
      array(
	  array("ServerIP",	 "C",  20),	  // Adresse IP du server actuel (0.0.0.0 <=> no serveur)
	  array("ServerName",	 "C",  64),	  // ID DirectPlay du Server 
          array("ServerVers", 	 "N",  4, 3), 	  // Version du Serveur
          array("ServerDate", 	 "D") 	  	  // Heure a laquelle le serveur a demarre
      );
  if (!dbase_create("zgnetserver.dbf", $def)) 	  // Cr�� la table "zgnetserver.dbf"
      print "<strong>Erreur!</strong>";
  else
      print "Cr&eacute;ation r&eacute;ussie.";
?>
