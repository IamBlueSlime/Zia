#!/usr/local/bin/php-cgi
<?php
    $login = true;
    $error = null;

    $username = null;
    $password = null;

    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST["username"]) && isset($_POST["password"])) {
            $username = $_POST["username"];
            $password = $_POST["password"];

            if ($username == "Admin" && $password == "passw0rd") {
                $login = false;
            } else {
                $error = "Bad username or password";
            }
        } else {
            $error = "Missing username or password";
        }
    }
?>
<!doctype html>
<html lang="en">
    <head>
        <title>Login example</title>
    </head>

    <?php
        if ($login == true) :
    ?>
    <body>
        <h1>Please login</h1>

        <?php
            if ($error != null) :
        ?>
        <p style="color: red;"><?php echo($error); ?></p>
        <?php
            endif;
        ?>

        <form method="POST">
            <p>Username: <input type="text" name="username" placeholder="Username" /></p>
            <p>Password: <input type="password" name="password" placeholder="Password" /></p>
            <p><input type="submit"></p>
        </form>

        <a href="/">Back to home</a>
    </body>
    <?php
        else :
    ?>
    <body>
        <h1>Hi <?php echo($username); ?>!</h1>
        <p>How are you?</p>
        <a href="/">Back to home</a>
    </body>
    <?php
        endif;
    ?>
</html>