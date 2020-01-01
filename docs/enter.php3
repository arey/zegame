<?php if ($Userid!=""): //Teste si le cookie est en place et continue jusqu'à un else ou un endif.
setcookie("Userid",  $Userid,    time()+15552000); // Rallonge le cookie de 6 mois 

require 'verify.inc';

//Ouvre le fichier users.dbf en lecture-écriture (2)
$db = verify(dbase_open("users.dbf", 2), "Lors de l'ouverture de la base de données.");

$result = dbase_get_record($db, $Userid); //Renvoie la ligne $Userid dans un array
$res = array( $result[0], time(), ++$result[2] );
verify(dbase_replace_record($db, $res, $Userid), "Lors du remplacement."); //Remplace la ligne
verify(dbase_close($db), "Lors de la fermeture de la base de données."); //Ferme la base de données
?>
<html>
<head>
<title></title>
</head>
<body>
<H1>Vous êtes maintenant sur mon site!</H1>
<?php //De nouveau du PHP
Function Imprime($name, $value) {
  echo "<H4>$name : $value</H4>"; //echo = print
}
Imprime("Nom", $result[0]);
Imprime("Nombre de visites", $result[2]);
if ($result[2] > 0) //Si l'utilisateur est déjà venu plus d'une fois, on imprime la date de la dernière venue.
  Imprime("Derni&egrave;re visite", date("d.m.Y, H:i:s", $result[1]));
?>
</body>
</html>
<?php 
else: //if ($Userid!=""):
//Si l'utilisateur n'est jamais venu, on envoie une forme pour qu'il rentre son nom
?>
<html>
<head>
<title></title>
</head>
<body>
<H1>Bienvenue</H1>
<form action="start.php3" method="post">
    Nom: <input type="text" name="thename"><br>
    <input type="submit">
    <input type="reset">
</form>
</body>
</html>
<?php endif; ?>
