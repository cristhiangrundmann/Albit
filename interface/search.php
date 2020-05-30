<?php

if(isset($_POST['search_key']) && !empty($_POST['search_key'])){
    $word = addslashes($_POST['search_key']);
    exec("./example.out $word", $output);
    //$results = $output
    //for($i=0;$i<count($output);$i++){
        //echo($output[$i]);
        //echo("\n");
        //$array="1";
    //}
    foreach ($output as $i => $value) {
        $output[$i]=explode(' => ',$output[$i]);
        $results[$output[$i][0]]=$output[$i][1];
    }
    unset($output);

    //$results = explode(' => ',$output[0]);

    //$array = array($results[0] => $results[1]);

    //var_dump($results);
    print_r($results);
    //include 'results.html';
}
else{
    include 'index.html';
    echo "<script> confirm('Insira uma pesquisa válida?');</script>";
}


?>