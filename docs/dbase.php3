<?php //Programme � n'ex�cuter qu'une fois pour cr�er la base de donn�es.
  $def =
      array(
          array("Nom",      "C",  20),    //Cha�ne de 20 caract�res
	  array("IP",       "C",  20),	  //Adresse IP de la personne connect�e
	  array("Domain",   "C",  40),	  //Nom de l'ordi connect� 
          array("Venu",     "N",  10, 0), //Date (Nombre) de la derni�re venue.
          array("Compteur", "N",   5, 0), //Nombre de venues (5 chiffres, 0 chiffres apr�s la virgule)
	  array("Download", "N",   4, 0), //Nombre de fois qu'il a t�l�charg� la d�mo
	  array("Langue",   "C",  10)	  // 	   	
      );
  if (!dbase_create("users.dbf", $def)) //Cr�� la base de donn�es.
      print "<strong>Erreur!</strong>";
  else
      print "Cr&eacute;ation r&eacute;ussie.";
?>
