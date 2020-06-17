<?php

function show_article($id){
    exec("./client article " . $id, $output, $ret);
    
    if (!empty($_GET['title'])){
        echo "<h1>" . $_GET['title'] . "</h1>";
    }

    foreach ($output as $i => $value) {
        echo $value . "<br>";
    }
}

if ($_SERVER['REQUEST_METHOD'] === 'GET'){
    if (!empty($_GET['id'])){
        show_article((int)$_GET['id']);
    }
    else{
        include 'index.html';
    }
}

?>