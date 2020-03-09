#!/usr/bin/python3
# coding: utf8

from datetime import datetime

print("Status: 200\r\n", end="")
print("Content-Type: text/html\r\n\r\n", end="")

print("""
<!doctype html>
<html lang="en">
   <head>
      <title>Hello</title>
   </head>
   
   <body>
      <h1>Hello from Python!</h1>
      <p>{}</p>
      <a href="/">Back to home</a>
   </body>
</html>
""".format(datetime.now()), end="")