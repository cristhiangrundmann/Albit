<?php

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    
    if (isset($_POST['search_key'])) {
        if(!empty($_POST['search_key'])){
            exec("./example.out '" . $_POST['search_key'] . "'", $results);
            $time=$results[0];
            unset($results[0]);
            foreach ($results as $i => $value) {
                $results[$i]=explode(' => ',$results[$i]);
            }
            $qtd=count($results);
            $atual=1;
            $pgs=ceil($qtd/10);
            include 'results.html';
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
?>