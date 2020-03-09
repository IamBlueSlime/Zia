#!/usr/bin/python3
# coding: utf8

import cgi

form = cgi.FieldStorage()
print(form)
if form.has_key("phrase"):
   text = form["phrase"].value

print("Content-Type: text/html\r\n\r\n", end="")

print("""
<html>
   <head>
      <title>Repeat</html>
   </head>
   
   <body>
      <h1>Repeat</h1>
      <p>Hello!</p>
   </body>
</html>
""".format(text), end="")