<?php

if(isset($_POST['search_key']) && !empty($_POST['search_key'])){
    $word = addslashes($_POST['search_key']);
    exec("./example.out $word", $output);
    $test = "testando";
    echo "$output $test";
    //include 'results.html';
}
else{
    echo("Erro no código...");
}


?>