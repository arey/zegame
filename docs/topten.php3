<html>
<head>
<title>Top Ten</title>
</head>
<body>
<H1>Top Ten</H1>
<I>Voici le top ten des visiteurs les plus fid&egrave;les!</I>
<?php
require 'verify.inc';

$db = verify(dbase_open("users.dbf", 2), "Lors de l'ouverture de la base de données.");
for ($i=1;$i<=dbase_numrecords($db);$i++) {
  $res[$i] = dbase_get_record($db, $i);
  $sor[$i] = $res[$i][2];
}
verify(dbase_close($db), "Lors de la fermeture de la base de données.");
arsort($sor); //trie
reset ($sor);
?>
<TABLE BORDER=1>
<TR>
  <TD>Nom</TD>
  <TD>Derni&egrave;re venue</TD>
  <TD>Nombre de venues</TD>
</TR>
<?php
for ($i=0;($i<10) && (list($key,) = each($sor));$i++) {
  print "<TR>\n";
  $date = date("d.m.Y, H:i:s", $res[$key][1]);
  $nbre = $res[$key][2];
  print "  <TD>" . $res[$key][0] . "</TD>";
  print "  <TD>" . $date . "</TD>";
  print "  <TD><IMG SRC=\"nbre.php3?$nbre\"></TD>";
  print "</TR>\n";
}
?>
</TABLE>
</body>
</html>
