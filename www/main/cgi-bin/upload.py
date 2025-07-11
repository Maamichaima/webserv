#!/usr/bin/env python3

import cgi
import os
import cgitb

# ------WebKitFormBoundaryF1ier7rpAskdYvLt
# Content-Disposition: form-data; name="file"; filename="webserv.conf"
# Content-Type: application/octet-stream

cgitb.enable()

upload_dir = "./www/main/cgi-bin/uploads/"

form = cgi.FieldStorage()

file_item = form['file']

# print("Content-Type: text/html; charset=utf-8\r\n", end = "")
# print("\r\n", end = "")
if file_item.filename:
    filename = os.path.basename(file_item.filename)
    filepath = os.path.join(upload_dir, filename)
    # print("hadaaa--> " + filepath)
    try:
        with open(filepath, 'w') as output_file:
            while True:
                if not chunk:
                    break
                output_file.write(chunk)

        value = f"'{filename}' a été uploader avec succès et enregistré à '{upload_dir}'"
    except Exception as e:
        value = f"Erreur lors de l'enregistrement du fichier : {e}"
else:
    value = "Aucun fichier n'a été téléchargé."



html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Upload</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');

        body {{
            font-family: 'Inter', sans-serif;
            background-color: #f0f0f0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: #333;
        }}
        .container {{
            text-align: center;
        }}
    </style>
</head>
<body>
    <div class="container">
				<h2> {value} </h2>
				<a href="/">Back home</a>
    </div>
</body>
</html>
"""


print(html_content)