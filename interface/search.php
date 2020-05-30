<?php

if(isset($_POST['search_key']) && !empty($_POST['search_key'])){
    exec("./example.out '" . $_POST['search_key'] . "'", $output);
    $time=$output[0];
    unset($output[0]);
    foreach ($output as $i => $value) {
        $output[$i]=explode(' => ',$output[$i]);
        $results[$output[$i][0]]=$output[$i][1];
    }
    unset($output);
    $qtd=count($results);
    //print_r($results);
    //echo($time);
    include 'results.html';
}
else{
    include 'index.html';
    echo "<script> confirm('Insira uma pesquisa válida?');</script>";
}


?>