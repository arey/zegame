<?php //Validation de la forme dans enter.php3
require 'verify.inc'; //Pour la fonction verify()

$db = verify(dbase_open("users.dbf", 2), "Lors de l'ouverture.");
$id = dbase_numrecords($db)+1;
verify(dbase_add_record($db, array($thename, time(), 0)), "Lors de l'insertion.");
verify(dbase_close($db), "Lors de la fermeture.");

setcookie("Userid", $id, time()+15552000); /* Défini un cookie pour six mois */
header("Location: enter.php3"); //Retourne dans l'autre document
exit;
?>
