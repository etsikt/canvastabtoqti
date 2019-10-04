<?php
//http://akrabat.com/php/creating-a-zip-file-with-phps-ziparchive/
//http://php.net/manual/en/zip.examples.php

$g_debug = 0; 

function DebugLog($logstring)
{
  global $g_debug;
  if($g_debug)
	{
	  print("$logstring<br/>\n");
	}
}

function format_QTI_begin($quizId, $title)
{
   DebugLog("format_QTI_begin");
   $stuff = <<<EOT
<?xml version = "1.0" encoding="utf-8"?>
<questestinterop xmlns="http://www.imsglobal.org/xsd/ims_qtiasiv1p2" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.imsglobal.org/xsd/ims_qtiasiv1p2 http://www.imsglobal.org/xsd/ims_qtiasiv1p2p1.xsd">
  <assessment ident="$quizId" title="$title">
    <qtimetadata>
      <qtimetadatafield>
        <fieldlabel>cc_maxattempts</fieldlabel>
        <fieldentry>1</fieldentry>
      </qtimetadatafield>
    </qtimetadata>
    <section ident="root_section">
EOT;
   return $stuff;
}

function format_QTI_end()
{
   $stuff = <<<EOT
  </section>
  </assessment>
</questestinterop>
EOT;
   return $stuff;
}

function format_QTI_test_begin($i)
{
   DebugLog("format_QTI_test_begin");
   $stuff = <<<EOT
      <item ident="tabtoqti_item_$i" title="Spørsmål">
        <itemmetadata>
          <qtimetadata>
            <qtimetadatafield>
              <fieldlabel>question_type</fieldlabel>
              <fieldentry>multiple_choice_question</fieldentry>
            </qtimetadatafield>
            <qtimetadatafield>
              <fieldlabel>points_possible</fieldlabel>
              <fieldentry>1</fieldentry>
            </qtimetadatafield>
            <qtimetadatafield>
              <fieldlabel>assessment_question_identifierref</fieldlabel>
              <fieldentry>tabtoqti_$i</fieldentry>
            </qtimetadatafield>
          </qtimetadata>
        </itemmetadata>
EOT;
   return $stuff;
}

function format_QTI_test_end()
{
   $stuff = <<<EOT
</item>
EOT;
   return $stuff;
}




function format_QTI_presentation_begin()
{
   DebugLog("format_QTI_presentation_begin");
   $stuff = <<<EOT
<presentation>
EOT;
   return $stuff;
}

function format_QTI_presentation_end()
{
   $stuff = <<<EOT
</presentation>
EOT;
   return $stuff;
}


function format_QTI_question_begin($question)
{
   $stuff = <<<EOT
<material>
<mattext>
<![CDATA[$question]]>
EOT;
   return $stuff;
}

function format_QTI_question_end()
{
   $stuff = <<<EOT
</mattext>
</material>
EOT;
   return $stuff;
}





function format_QTI_answer_begin($task_id)
{
   $stuff = <<<EOT
<response_lid ident="Q$task_id"  rcardinality="Single">
<render_choice>
EOT;
   return $stuff;
}

function format_QTI_answer_option($answer, $answer_id)
{
   $stuff = <<<EOT
<response_label ident="alt$answer_id">
<material>
<mattext><![CDATA[
$answer
]]></mattext></material>
</response_label>
EOT;
   return $stuff;
}

function format_QTI_answer_end()
{
   $stuff = <<<EOT
</render_choice>
</response_lid>
EOT;
   return $stuff;
}

/*
      <resprocessing>
          <outcomes>
            <decvar maxvalue="100" minvalue="0" varname="SCORE" vartype="Decimal"/>
          </outcomes>
          <respcondition continue="No">
            <conditionvar>
              <varequal respident="response1">701</varequal>
            </conditionvar>
            <setvar action="Set" varname="SCORE">100</setvar>
          </respcondition>
        </resprocessing>
*/

function format_QTI_answer_process_begin($task_id)
{
   $stuff = <<<EOT
<resprocessing>
<outcomes>
            <decvar maxvalue="100" minvalue="0" varname="SCORE" vartype="Decimal"/>
</outcomes>
EOT;
   return $stuff;
}

function format_QTI_answer_process_correct_option($task_id,$answer_id)
{
   $stuff = <<<EOT
<respcondition continue="No">
<conditionvar>
<varequal respident="Q$task_id">alt$answer_id</varequal>
</conditionvar>
            <setvar action="Set" varname="SCORE">100</setvar>
</respcondition>
EOT;
   return $stuff;
}

function format_QTI_answer_process_end()
{
   $stuff = <<<EOT
</resprocessing>
EOT;
   return $stuff;
}


function printArray($arr)
{
   $ci = current($arr);
   $no = count($arr);
   $stuff = "";

   for($i=0; $i < $no; $i++)
   {
      $stuff .= $ci . "<br/>";
      $ci = next($arr);
   }
   $stuff .= "<hr>";
   DebugLog($stuff);
}

function convertTabToQti($quizId, $tabtext, $title)
{

   $lines = explode("\r\n", $tabtext);
   $no_of_lines= count($lines);
   DebugLog( "Number of lines: $no_of_lines <br/>");

   $qti = format_QTI_begin($quizId, $title);

   for($linenum= 0; $linenum < $no_of_lines; $linenum++)
   {
      DebugLog( "Analyzing:$lines[$i]<br/>");
      $columns = explode("\t", $lines[$linenum]);
      $noOfColumns = count($columns);
	  DebugLog( "Number of columns: $noOfColumns <br/>");
      $question = $columns[0];
      $answer_array = array();
      if($noOfColumns >= 2)
      {
               for($i = 1; $i < $noOfColumns; $i++)
               {
                  $answer = $columns[$i];
                  array_push($answer_array, $answer);
               }

      }
      $qti .= createQtiFileFromArrays($linenum, $question, $answer_array);
   }


   $qti .= format_QTI_end();

   return $qti;

}

function generateZipFileFromTab($tabtext, $title)
{
   $quizId = uniqid();
   DebugLog("Unique id:$quizId");
	$qti = convertTabToQti($quizId, $tabtext, $title);
   DebugLog("convertTabToQti DONE");

	$assMeta = getAssessmentMetaString($title, $quizId);
	DebugLog("ass meta created");
	
	$imsManifest = getImsManifest($quizId);

	$zip = new ZipArchive();
	DebugLog("Zip archive created");
	$zipname = "canvasqti$quizId";
	$filename = "zip/$zipname.zip";

	DebugLog("Zip filename is $filename");
	if ($zip->open($filename, ZipArchive::CREATE)!==TRUE) {
		exit("cannot open $filename\n");
	}
	
    if(!$zip->addEmptyDir("$zipname")) {
        echo 'Could not create the directory $zipname';
    }
    if(!$zip->addEmptyDir("$zipname/non_cc_assessments")) {
        echo 'Could not create the directory $zipname';
    }
    
	$zip->addFromString("$zipname/$quizId/assessment_meta.xml", "$assMeta");
	$zip->addFromString("$zipname/$quizId/$quizId.xml", "$qti");
	$zip->addFromString("$zipname/imsmanifest.xml", "$imsManifest");
	$zip->close();
	return $filename;
}

function SendZipFileToClient($filename)
{
	$finfo = finfo_open(FILEINFO_MIME_TYPE);
	$mimeType = finfo_file($finfo, $filename);
	$size = filesize($filename);
	$name = basename($filename);

	if (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on') {
		// cache settings for IE6 on HTTPS
		header('Cache-Control: max-age=120');
		header('Pragma: public');
	} else {
		header('Cache-Control: private, max-age=120, must-revalidate');
		header("Pragma: no-cache");
	}
	header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // long ago
	header("Content-Type: $mimeType");
	header('Content-Disposition: attachment; filename="' . $name . '";');
	header("Accept-Ranges: bytes");
	header('Content-Length: ' . filesize($filename));

	print readfile($filename);

}

function createQtiFileFromArrays($i, $question, $answer_array)   
{
   printArray($answer_array);
   $qti = "";
   $qti .= format_QTI_test_begin($i);
   $qti .= format_QTI_presentation_begin();
      $qti .= format_QTI_question_begin($question);
      $qti .= format_QTI_question_end();
      $qti .= format_QTI_answer_begin($i);
      $qti .= formatQtiAnswers($answer_array);
      $qti .= format_QTI_answer_end();
      $qti .= format_QTI_presentation_end();
      $qti .= formatQtiAnswerProcessing($i, $answer_array);
      $qti .= format_QTI_test_end();
   return $qti;  
}

function formatQtiAnswerProcessing($ex_id, $answer_array)
{
   $stuff = format_QTI_answer_process_begin($ex_id);

  $stuff .= format_QTI_answer_process_correct_option($ex_id, 0); //0 is correct option
   $stuff .= format_QTI_answer_process_end();
   return $stuff;
}

function formatQtiAnswers($answer_array)
{
$stuff = "";
      foreach ($answer_array as $answer_no => $answer) 
      {
         if($answer != "")
         {
         	$stuff .= format_QTI_answer_option($answer, $answer_no);
         }
      }
   return $stuff;
}

function getAssessmentMetaString($title, $quizId)
{
	DebugLog("BEGIN getAssessmentMetaString");
	$assignmentGroupId = "tabtoqti_groupid";
   $stuff = <<<EOT
<?xml version="1.0" encoding="UTF-8"?>
<quiz identifier="$quizId" xmlns="http://canvas.instructure.com/xsd/cccv1p0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://canvas.instructure.com/xsd/cccv1p0 http://canvas.instructure.com/xsd/cccv1p0.xsd">
  <title>$title</title>
  <description></description>
  <shuffle_answers>false</shuffle_answers>
  <scoring_policy>keep_highest</scoring_policy>
  <quiz_type>assignment</quiz_type>
  <points_possible>2</points_possible>
  <lockdown_browser_monitor_data></lockdown_browser_monitor_data>
  <show_correct_answers>true</show_correct_answers>
  <anonymous_submissions>false</anonymous_submissions>
  <could_be_locked>true</could_be_locked>
  <allowed_attempts>1</allowed_attempts>
  <one_question_at_a_time>false</one_question_at_a_time>
  <cant_go_back>false</cant_go_back>
  <available>true</available>
  <assignment identifier="$quizId">
    <title>Ball</title>
    <assignment_group_identifierref>$assignmentGroupId</assignment_group_identifierref>
    <workflow_state>published</workflow_state>
    <quiz_identifierref>$quizId</quiz_identifierref>
    <points_possible>2</points_possible>
    <grading_type>points</grading_type>
    <all_day>false</all_day>
    <submission_types>online_quiz</submission_types>
    <position>2</position>
    <peer_review_count>0</peer_review_count>
    <peer_reviews_assigned>false</peer_reviews_assigned>
    <peer_reviews>false</peer_reviews>
    <automatic_peer_reviews>false</automatic_peer_reviews>
    <muted>false</muted>
  </assignment>
  <assignment_group_identifierref>$assignmentGroupId</assignment_group_identifierref>
</quiz>
EOT;
	return $stuff;
}

function getImsManifest($quizId)
{
   $stuff = <<<EOT
<?xml version="1.0" encoding="UTF-8"?>
<manifest identifier="i5b9ea7093c68c75a1ed91682c0787621" xmlns="http://www.imsglobal.org/xsd/imsccv1p1/imscp_v1p1" xmlns:lom="http://ltsc.ieee.org/xsd/imsccv1p1/LOM/resource" xmlns:imsmd="http://www.imsglobal.org/xsd/imsmd_v1p2" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.imsglobal.org/xsd/imsccv1p1/imscp_v1p1 http://www.imsglobal.org/xsd/imscp_v1p1.xsd http://ltsc.ieee.org/xsd/imsccv1p1/LOM/resource http://www.imsglobal.org/profile/cc/ccv1p1/LOM/ccv1p1_lomresource_v1p0.xsd http://www.imsglobal.org/xsd/imsmd_v1p2 http://www.imsglobal.org/xsd/imsmd_v1p2p2.xsd">
  <metadata>
    <schema>IMS Content</schema>
    <schemaversion>1.1.3</schemaversion>
    <imsmd:lom>
      <imsmd:general>
        <imsmd:title>
          <imsmd:string>Canvas QTI file</imsmd:string>
        </imsmd:title>
      </imsmd:general>
      <imsmd:lifeCycle>
        <imsmd:contribute>
          <imsmd:date>
            <imsmd:dateTime>2014-11-25</imsmd:dateTime>
          </imsmd:date>
        </imsmd:contribute>
      </imsmd:lifeCycle>
      <imsmd:rights>
        <imsmd:copyrightAndOtherRestrictions>
          <imsmd:value>yes</imsmd:value>
        </imsmd:copyrightAndOtherRestrictions>
        <imsmd:description>
          <imsmd:string>Public Domain - http://en.wikipedia.org/wiki/Public_domain</imsmd:string>
        </imsmd:description>
      </imsmd:rights>
    </imsmd:lom>
  </metadata>
  <organizations/>
  <resources>
    <resource identifier="$quizId" type="imsqti_xmlv1p2">
      <file href="$quizId/$quizId.xml"/>
      <dependency identifierref="i8e789820b10d6c3285c04dc9e8c86289"/>
    </resource>
    <resource identifier="tabtoqti_resource" type="associatedcontent/imscc_xmlv1p1/learning-application-resource" href="$quizId/assessment_meta.xml">
      <file href="$quizId/assessment_meta.xml"/>
    </resource>
  </resources>
</manifest>
EOT;

}

?>
