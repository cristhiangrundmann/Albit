<?php

function pesquisar($search_key){
    exec("./example.out '" . $search_key . "'", $results);
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
        $atual=1;
        $pgs=ceil($qtd/10);
        include 'results.html';
    }
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_POST['search_key'])) {
        if(!empty($_POST['search_key'])){
            pesquisar($_POST['search_key']);
        }
        else{
            include 'index.html';
            echo "<script> confirm('Insira uma pesquisa válida?');</script>";
        }
    }


        if (isset($_POST['previous'])) {
            $atual = max($atual-1,1);
            include 'results.html';
        }

        if (isset($_POST['next'])) {
            $atual = max($atual+1,pgs);
            include 'results.html';
        }

}
else{
    $sugestion = $_GET['sugestion'];
    pesquisar($sugestion);
}

?>