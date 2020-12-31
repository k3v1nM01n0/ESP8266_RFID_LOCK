## Description

TODO

## Server

### Running Server

```bash
cd server
#install virtual
python3 -m venv virtual
#activate
source virtual/bin/activate
#install deps
pip install -r requirements.txt
#run server
python app.py
```

You can change the server address here

```python
if __name__ == "__main__":
    app.run("0.0.0.0", 5000, debug=True)
```

### Using API

The API is meant to be a proof of concept in reality more work is needed for example user authentication and a better data store like a SQL DB.

#### Create UUID

>> GET /uuid/create -H "Authorization:YOUR_TOKEN"

```bash
curl -i http://127.0.0.1:5000/uuid/create -H "Authorization:YOUR_TOKEN"
HTTP/1.0 200 OK
Content-Type: application/json
Content-Length: 32
Server: Werkzeug/1.0.1 Python/3.8.5
Date: Thu, 31 Dec 2020 08:15:54 GMT

{
  "uid": "ce6d534eaf48148e"
}
```

You can set the token in the code 

```python
#should be secret, use a .env instead, dont commit it!
#you should also not use static token authentication in production 
TOKEN = "YOUR_TOKEN"
```

### Validate UUID

>> GET /uuid/verify/<uid>

```bash
$ curl -i http://127.0.0.1:5000/uuid/verify/ce6d534eaf48148e
HTTP/1.0 200 OK
Content-Type: application/json
Content-Length: 23
Server: Werkzeug/1.0.1 Python/3.8.5
Date: Thu, 31 Dec 2020 08:20:23 GMT

{
  "msg": "success"
}
```