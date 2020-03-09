#!/bin/bash

echo -ne "Status: 200\r\n"
echo -ne "Content-Type: text/html\r\n\r\n"

cat <<EOF
<!doctype html>
<html lang="en">
   <head>
      <title>Hello</title>
   </head>
   
   <body>
      <h1>Hello from Bash!</h1>
      <p>$(date)</p>
      <a href="/">Back to home</a>
   </body>
</html>
EOF