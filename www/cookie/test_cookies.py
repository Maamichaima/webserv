#!/usr/bin/env python3

import os
import cgi
import uuid
import time
import http.cookies

# Ensure the cookie database directory exists
def check_path(path):
    if not os.path.exists(path):
        os.makedirs(path)

check_path("./www/cookie/database")

print("Content-Type: text/html; charset=utf-8\r\n", end="")

# Generate a unique session ID
def generateId():
    return str(uuid.uuid4())

# Generate expiration date for cookies (30 days)
def generateExpirationDate():
    expiration_time = time.time() + 60 * 60 * 24 * 30
    return time.strftime("%a, %d-%b-%Y %H:%M:%S GMT", time.gmtime(expiration_time))

# Create a new cookie and store it
def createNewCookie():
    user_id = generateId()
    expiration_date = generateExpirationDate()
    print(f"Set-Cookie: session_id={user_id}; Expires={expiration_date}; Path=/\r\n", end="")
    with open(f"./www/cookie/database/{user_id}.txt", "w") as file:
        file.write("light")  # Default theme storage
    return user_id

# Read cookies from the browser
cookies = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
user_id = cookies.get("session_id")

# If no session ID, create a new one
if user_id:
    user_id = user_id.value
else:
    user_id = createNewCookie()

# Get form data
form = cgi.FieldStorage()
cookie_name = form.getvalue("cookie_name")
cookie_value = form.getvalue("cookie_value")

# Set a new cookie if submitted
if cookie_name and cookie_value:
    expiration_date = generateExpirationDate()
    print(f"Set-Cookie: {cookie_name}={cookie_value}; Expires={expiration_date}; Path=/\r\n", end="")

# Read stored cookies again after setting a new one
cookies = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))

# HTML Output
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Cookie Test</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 50px;
        }}
        form {{
            margin-bottom: 20px;
        }}
        input, button {{
            padding: 10px;
            margin: 5px;
        }}
    </style>
</head>
<body>
    <h2>Cookie Test</h2>
    <form method="post">
        <label>Cookie Name: <input type="text" name="cookie_name"></label><br>
        <label>Cookie Value: <input type="text" name="cookie_value"></label><br>
        <button type="submit">Set Cookie</button>
    </form>
    
    <h3>Stored Cookies:</h3>
    <ul>
"""

# Display stored cookies
if cookies:
    for key, morsel in cookies.items():
        html_content += f"<li>{key}: {morsel.value}</li>"
else:
    html_content += "<li>No cookies found.</li>"

html_content += """
    </ul>
</body>
</html>
"""

print("\r\n", end="")
print(html_content)
