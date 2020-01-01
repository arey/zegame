<?php //Programme à n'exécuter qu'une fois pour créer la base de données.
  $def =
      array(
          array("Nom",      "C",  20),    //Chaîne de 20 caractères
	  array("IP",       "C",  20),	  //Adresse IP de la personne connectée
	  array("Domain",   "C",  40),	  //Nom de l'ordi connecté 
          array("Venu",     "N",  10, 0), //Date (Nombre) de la dernière venue.
          array("Compteur", "N",   5, 0), //Nombre de venues (5 chiffres, 0 chiffres après la virgule)
	  array("Download", "N",   4, 0), //Nombre de fois qu'il a téléchargé la démo
	  array("Langue",   "C",  10)	  // 	   	
      );
  if (!dbase_create("users.dbf", $def)) //Créé la base de données.
      print "<strong>Erreur!</strong>";
  else
      print "Cr&eacute;ation r&eacute;ussie.";
?>
