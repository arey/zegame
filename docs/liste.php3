<html>
<head>
<title>Liste des utilisateurs</title>
</head>
<body>
<H1>Liste des utilisateurs</H1>
<?php
require "verify.inc";

$col=implode($argv,"-"); //Obtient le nombre
if (!$col) {
  $col=0;
}

$db = verify(dbase_open("users.dbf", 2), "Lors de l'ouverture de la base de données.");
for ($i=1;$i<=dbase_numrecords($db);$i++) {
  $res[$i] = dbase_get_record($db, $i);
  $sor[$i] = $res[$i][$col];
}
verify(dbase_close($db), "Lors de la fermeture de la base de données.");
asort($sor);
reset ($sor);
?>
<TABLE BORDER=1>
<TR>
  <TD><A HREF="liste.php3?0">Nom</A></TD>
  <TD><A HREF="liste.php3?1">Derni&egrave;re venue</A></TD>
  <TD><A HREF="liste.php3?2">Nombre de venues</A></TD>
</TR>
<?php
while (list($key,) = each($sor)) {
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
