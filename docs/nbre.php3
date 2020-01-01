<?php
    Header("Content-type: image/gif");
    $nbre=implode($argv," ");
    $x = $nbre * 12;
    $im = imagecreate($x, 20);
    for ($i=0;$i<120;$i++) {
      $col[$i] = ImageColorAllocate($im, $i+120, 0, 0); //alloue deux couleurs, une pour le bord(plus foncée), une pour l'intérieur
      $bcol[$i] = ImageColorAllocate($im, $i+100, 0, 0);
    }
    $a = 0;
    for ($i=0;$i<$x;$i++) {
      if (floor($i / 120) % 2) { //calcule si on doit aller du rouge au noir ou du rouge au noir
        $a--;
        Imageline($im, $i, 0, $i, 1, $bcol[$a]);
        Imageline($im, $i, 2, $i, 17, $col[$a]); //dessine le dégradé
        Imageline($im, $i, 18, $i, 19, $bcol[$a]);
      }
      else {
        Imageline($im, $i, 0, $i, 1, $bcol[$a]);
        Imageline($im, $i, 2, $i, 17, $col[$a]);
        Imageline($im, $i, 18, $i, 19, $bcol[$a]);
        $a++;
      }
    }
    
    $k = round($nbre / 20);
    $k = ($k % 2) == 0 ? 1 : 0;
    
    $color = ImageColorAllocate($im, 255*$k, 0, 0);
    $px = ($x-7.5*strlen($nbre))/2;
    ImageString($im,5,$px,3,$nbre,$color); //dessine le nombre

    ImageGif($im); //affiche l'image
    ImageDestroy($im); //détruit l'image
?>
