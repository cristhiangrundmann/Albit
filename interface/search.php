<?php



function pesquisar($search_key){
    exec("./client '" . $search_key . "'", $results, $ret);
    $time=$results[0];
    unset($results[0]);
    if($time==-1){
        exec("./sugestion.out '" . $search_key . "'", $results);
        include 'sugestion.html';
    }
    else{
        foreach ($results as $i => $value) {
            $results[$i]=explode(' => ',$results[$i]);
        }
        $qtd=count($results);
        include 'results.html';
    }
}

if ($_SERVER['REQUEST_METHOD'] === 'GET'){
    if (!empty($_GET['word'])){
        pesquisar($_GET['word']);
    }
    else{
        include 'index.html';
        echo "<script> confirm('Insira uma pesquisa válida.');</script>";
    }
}

?>