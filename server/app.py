import os, hashlib
from flask import Flask, jsonify, request

app = Flask(__name__)

#should be secret, use a .env instead, dont commit it!
#you should also not use static token authentication in production 
TOKEN = "5a1c307ce50c97d2f3ed9590775fed6f449eafa84183557087b403fe84c411a1"

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

#should use a better data structure in production, like a sql DB
file_path = BASE_DIR + "/data/data.txt"

UID_LEN = 16


@app.route("/uuid/verify/<uid>", methods=["GET"])
def verify(uid):
    if len(uid) != UID_LEN:
        return jsonify({"msg":"invalid uid len"}), 400

    file_exists = os.path.isfile(file_path)

    exists = False
    if file_exists:
        with open(file_path, "r") as file:
            data = file.read()
            data = data.split("\n")
            _hash =  hashlib.sha256(uid.encode()).hexdigest()
            if _hash in data:
                exists = True

    if exists == True:
        return jsonify({"msg":"success"}), 200

    return jsonify({"msg":"invalid uid"}), 401

@app.route("/uuid/create")
def create_user():
    token = request.headers.get("Authorization", None)

    #timing attacks?
    if token != TOKEN:
        return jsonify({"msg":"invalid token"}), 401
    
    uid = os.urandom(UID_LEN//2).hex()

    file_exists = os.path.isfile(file_path)
    _hash = hashlib.sha256(uid.encode()).hexdigest()

    with open (file_path, "a") as file:
        file.write(_hash+"\n")

    return jsonify({"uid":uid}), 200

    
if __name__ == "__main__":
    app.run("0.0.0.0", 5000, debug=True)

