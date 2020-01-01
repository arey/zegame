<?php 	// Validation de la forme dans enter.php3
// Download le fichier tout en conservant une trace de ce visiteur

require 'verify.inc'; 	//Pour la fonction verify()

//Ouvre le fichier users.dbf en lecture-�criture (2)
$db = verify(dbase_open("users.dbf", 2), "Lors de l'ouverture.");

//Teste si le cookie est en place et continue jusqu'� un else ou un endif
if ($Userid==""):
// Rmq : on aurait pu utiliser $HTTP_COOKIES_VARS("Userid");
// Si le visiteur n'a pas de Cookies, on lui en cr�e un

// Calcul du prochain ID � attribuer au visiteur
$id = dbase_numrecords($db)+1;
setcookie("Userid", $id, time()+15552000*4); /* D�fini un cookie pour 2ans */

// R�cup�re son adresse IP 
$ip=getenv("REMOTE_ADDR");

// Nom de la machine
$host=gethostbyaddr($ip); 

// R�cup�re la langue du browser
$lang=getenv( "HTTP_ACCEPT_LANGUAGE");


// Ecriture du n-uplet 
verify(dbase_add_record($db, array($thename, $ip, $host, time(), 0, 0, $lang)), "Lors de l'insertion.");
verify(dbase_close($db), "Lors de la fermeture.");

//Tests...
//echo "<H4> Pas de cookies : $lang </H4>";

header("Location: ZeGame.zip"); //Retourne dans l'autre document
exit;

// Sinon, si il est d�j� venu sur ce site, on met � jour son cookies
else: // if ($Userid!="")

// Rallonge la dur�e de Cookies de 2 ans
setcookie("Userid", $Userid, time()+15552000*4); 

// Si �� a plant�, c'est que la base de donn�e a �t� modifi�e, il faut donc r�crire une ligne
$nf  = dbase_numfields($db);
//echo "<H4> nf : $nf </H4>";
//echo "<H4> Userid : $Userid </H4>";
if ($Userid>=$nf):
$id = dbase_numrecords($db)+1;
setcookie("Userid", $id, time()+15552000*4); /* D�fini un cookie pour 2ans */
$ip=getenv("REMOTE_ADDR");	// R�cup�re son adresse IP 
$host=gethostbyaddr($ip); 	// Nom de la machine
$lang=getenv( "HTTP_ACCEPT_LANGUAGE");	// R�cup�re la langue du browser
// Ecriture du n-uplet 
verify(dbase_add_record($db, array($thename, $ip, $host, time(), 0, 0, $lang)), "Lors de l'insertion.");
$Userid = $id;
endif;

$result = dbase_get_record($db, $Userid); //Renvoie la ligne $Userid dans un array

// MaJ les champs de la table
$res = array($result[0], $result[1], $result[2], time(), $result[4], ++$result[5], $result[6]);
//             Nom           IP         Domain    Tps     Compteur      Download     langue
verify(dbase_replace_record($db, $res, $Userid), "Lors du remplacement."); //Remplace la ligne
verify(dbase_close($db), "Lors de la fermeture de la base de donn�es."); //Ferme la base de donn�es

//Tests...
//echo "<H4> Ip : $result[1] </H4>";
//echo "<H4> Domain : $result[2] </H4>";
//echo "<H4> Download : $result[5] </H4>";

header("Location: ZeGame.zip"); //Retourne dans l'autre document
exit;

endif;
?>
