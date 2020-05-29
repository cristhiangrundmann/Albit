<?php

if(isset($_POST['search_key']) && !empty($_POST['search_key'])){
    $word = addslashes($_POST['search_key']);
    include 'results.html';
}
else{
    echo("Erro no código...");
}


?>