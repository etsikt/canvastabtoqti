<?php
require_once("inc/canvasqti.inc");

$questions = "";
if(IsSet($_POST['questions']))
{
   $questions = $_POST['questions'];
   $title = $_POST['title'];
   
   $filename = generateZipFileFromTab($questions, $title);
   SendZipFileToClient($filename);
//   header("Content-Type: text/xml, charset=utf-8");
//   header('Content-Disposition: attachment; filename=canvasqti.xml');
//   echo $qti;
}
else
{
   $thispage = $_SERVER['PHP_SELF'];

   $stuff = <<<EOT
<?xml version="1.0" encoding="utf-8"?> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 onal//EN"
      "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="content-type" content="text/html; charset=utf-8" />
  <title>Canvas tab to qti</title>
   </body>
   <h2>TAB til QTI konvertering</h2>
   Denne siden hjelper deg &aring; lage en qti fil som kan importeres i Canvas. Det vil da bli
   laget en multiple choice pr&oslash;ve.
   <ul>
   <li>
      Bruk excel til &aring; skrive sp&oslash;rsm&aring;lene i f&oslash;rste kolonne, riktig svar i andre kolonne og feil svar (distraktører) i så mange andre kolonner du vil.
   </li>
   <li>
      Merk kolonnene og kopier teksten vha CTRL+C eller ved &aring; h&oslash;yreklikke og velge kopier.
   </li>
   <li>
      Lim inn teksten nedenfor ved &aring; trykke CTRL+V eller ved &aring; h&oslash;yreklikke og velge lim inn.
   </li>
   <li>
      N&aring;r du trykker "Submit" vil du bli bedt om &aring; lagre filen p&aring; harddisken.
      Endre gjerne filnavnet til noe som beskriver sp&oslash;rsm&aring;lene.
   </li>
   <li>
      I Canvas velger du &aring; importere filen som QTI.
   </li>
   </ul>
   Ta kontakt med <b>erlend.thune@iktsenteret.no</b> hvis du har sp&oslash;rsm&aring;l ang&aring;ende denne siden!
   <br/><br/>
  <form method="post" action="$thispage">
  Tittel på prøven:<input type="text" name="title" size="100"/><br/>
  Oppgaver<br/>
  <textarea cols="160" rows="15" name="questions">$questions</textarea><br/>
  <input type="submit" name="submit" value="Submit"/>
  </form>
  </body>
  </html>
EOT;

   print $stuff;

}
?>

